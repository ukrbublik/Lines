///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Author: Luuk Weltevreden
// Date: 23-07-2004
//
// Modified a lot by Elmü (http://kickme.to/elmue)
// Date: 01-08-2006
//
// Filename: ExtractT.hpp
//
// Classes:
// - CExtractT<T>
// - CExtract
//
// Purpose: This template extracts files from a cabinet file (CAB) on disk
/*

----------------------------------------------------------------------------------
Using these conventions results in better readable code and less coding errors !
----------------------------------------------------------------------------------

     cName  for generic class definitions
     CName  for MFC     class definitions
     tName  for type    definitions
     eName  for enum    definitions
     kName  for struct  definitions

    e_Name  for enum variables
    E_Name  for enum constant values

    i_Name  for instances of classes
    h_Name  for handles

    T_Name  for Templates
    t_Name  for TCHAR or LPTSTR

    s_Name  for strings
   bs_Name  for BSTR
    f_Name  for function pointers
    k_Name  for contructs (struct)

    b_Name  bool,BOOL 1 Bit

   s8_Name    signed  8 Bit (char)
  s16_Name    signed 16 Bit (SHORT, WCHAR)
  s32_Name    signed 32 Bit (LONG, int)
  s64_Name    signed 64 Bit (LONGLONG)

   u8_Name  unsigned  8 Bit (BYTE)
  u16_Name  unsigned 16 bit (WORD)
  u32_Name  unsigned 32 Bit (DWORD, UINT)
  u64_Name  unsigned 64 Bit (ULONGLONG)

    d_Name  for double

  ----------------

    m_Name  for member variables of a class (e.g. ms32_Name for int member variable)
    g_Name  for global variables            (e.g. gu16_Name for global WORD)
    p_Name  for pointer                     (e.g.   ps_Name  *pointer to string)
   pp_Name  for pointer to pointer          (e.g.  ppd_Name **pointer to double)

*************************************************************************************/

#pragma once

// #################### START Visual Studio 6 ####################

#if _MSC_VER == 1200 // for Visual Studio 6.0 only

  // turn off MS Visual Studio 6 warning: 
  // "Identifier was truncated to '255' characters in the debug information"
  #pragma warning (disable: 4786)

  #define  IS_INTRESOURCE(_r) (((unsigned long)(_r) >> 16) == 0)
  __inline int   PtrToInt(const void* p) { return((int)  (INT_PTR) p); }
  __inline void* IntToPtr(const int   i) { return((void*)(INT_PTR) i); }

#endif

// #################### END Visual Studio 6 ####################

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "Map.hpp"
#include "FDI.h"

#pragma warning(disable: 4996)

// Output of open, close, read, write and seek operations onto a debug viewer
// To view the output set 1 here and use the tool "DebugView" from www.sysinternals.com
// This application MUST be started with STRG + F5 from VisualStudio otherwise you will see nothing
#define _TraceExtract   0

// Extension for enum FDIERROR defined in Microsoft file FDI.H
#define FDIERROR_LOAD_DLL  1000

namespace Cabinet
{

template <class T> class CExtractT
{
public:
	CExtractT()
	{ 
		mh_FDIContext = 0;
		mh_CabinetDll = 0;
		mu32_OrgError   = 0;
		mk_Error.fError  = FALSE;
	}

	~CExtractT()
	{ 
		DestroyFDIContext(); 
		// Decrement the reference count for the DLL.
		// If another instance is still using the DLL the DLL will not be unloaded!
		if (mh_CabinetDll) FreeLibrary(mh_CabinetDll);
	}

	// This function aborts the currently active operation.
	void AbortOperation()
	{
		mb_Abort = TRUE; 
	}

	// Sets the key for decryption of the CAB file
	// You can also pass binary data here which must be zero terminated
	BOOL SetDecryptionKey(char* s8_DecryptKey)
	{
		int s32_KeyLen = (int) strlen(s8_DecryptKey);

		if (s32_KeyLen >= sizeof(Store().s8_Key))
			return FALSE;

		Store().s32_KeyLen = s32_KeyLen;
		memcpy(Store().s8_Key, s8_DecryptKey, s32_KeyLen);
		return TRUE;
	}

	// Creates a new FDI context and returns TRUE if succesful or FALSE otherwise.
	BOOL CreateFDIContext()
	{
		mu32_OrgError = 0;

		if (mh_FDIContext)
			return FALSE;

		if (!mh_CabinetDll)
		{
			// If the DLL is already attached to this process it will not be loaded a second time!
			mh_CabinetDll = LoadLibraryA("Cabinet.dll");
			mf_FdiCreate    = (fFdiCreate)    GetProcAddress(mh_CabinetDll, "FDICreate");
			mf_FdiCopy      = (fFdiCopy)      GetProcAddress(mh_CabinetDll, "FDICopy");
			mf_FdiIsCabinet = (fFdiIsCabinet) GetProcAddress(mh_CabinetDll, "FDIIsCabinet");
			mf_FdiDestroy   = (fFdiDestroy)   GetProcAddress(mh_CabinetDll, "FDIDestroy");

			if (!mf_FdiCreate || !mf_FdiCopy || !mf_FdiIsCabinet || !mf_FdiDestroy)
			{
				mh_CabinetDll  = 0;
				mk_Error.fError  = TRUE;
				mk_Error.erfOper = FDIERROR_LOAD_DLL;
				return FALSE;
			}
		}

		mh_FDIContext = mf_FdiCreate((PFNALLOC)(FDIAlloc),
		                             (PFNFREE) (FDIFree),
		                             (PFNOPEN) (FDIOpen),
		                             (PFNREAD) (FDIRead),
		                             (PFNWRITE)(FDIWrite),
		                             (PFNCLOSE)(FDIClose),
		                             (PFNSEEK) (FDISeek),
		                             cpu80386,
		                             &mk_Error);

		return (mh_FDIContext != NULL);
	}

	// Destroys the FDI context used by this instance and returns TRUE if succesful or FALSE otherwise.
	BOOL DestroyFDIContext()
	{
		if (!mh_FDIContext)
			return TRUE;

		BOOL bRet = mf_FdiDestroy(mh_FDIContext);
		if (bRet)
			mh_FDIContext = NULL;

		return bRet;
	}

	// This function extracts one or more files from the specified cabinet.
	// Parameters:
	// - LPCSTR s8_CabFile
	//     Full path to the cabinet file.
	// - LPCSTR s8_TargetDir
	//     Target directory.
	// - void* pParam
	//     User defined value which will be passed to the callback function.
	// Returns:
	//   TRUE if the operation is succesful or FALSE otherwise.
	BOOL ExtractFile(const char* s8_CabFile, const char* s8_TargetDir, void* pParam = NULL)
	{
		char s8_Name[MAX_PATH];
		char s8_Path[MAX_PATH];
		char s8_Dir [MAX_PATH] = "";

		mu32_OrgError = 0;

		if (!mh_FDIContext)
			return FALSE;

		// Verify the paths
		if (!strlen(s8_CabFile))
			return FALSE;

		Store().Clear();
		strcpy(Store().s8_CabFile, s8_CabFile);

		if (s8_TargetDir)
		{
			strcpy(s8_Dir, s8_TargetDir);
			if (s8_Dir[strlen(s8_Dir)-1] != '\\')
			{
				strcat(s8_Dir, "\\");
			}
		}

		// Split the path to a filename and path
		char * p = strrchr((char*)s8_CabFile, '\\');
		if (p == NULL)
		{
			strcpy(s8_Name, s8_CabFile);
			strcpy(s8_Path, "");
		}
		else
		{
			strcpy(s8_Name, p+1);
			strncpy(s8_Path, s8_CabFile, (int)(p-s8_CabFile)+1);
			s8_Path[(int)(p-s8_CabFile)+1] = 0;
		}

		// Initialize the class
		mp_Param = pParam;
		mb_Abort = FALSE;
		ms8_TargetDir = s8_Dir;
		strcpy(ms8_LastCab, s8_Name);

		// The information, which files are contained in a CAB file, is not only in the first part
		// of a splitted archive. It is spread over multiple parts of the CAB file.
		// Example:
		// Part_1.cab --> start        of compressed File_1.exe
		// Part_2.cab --> continuation of compressed File_1.exe
		// Part_3.cab --> start        of compressed File_2.dll
		// Part_4.cab --> continuation of compressed File_2.dll
		// We have to extract every CAB file of a splitted archive which starts a new file 
		// (in this example we must call FdiCopy(Part_1.cab) and FdiCopy(Part_3.cab))
		// otherwise not all compressed files will be extracted.
		while (TRUE)
		{
			#if _TraceExtract
				char Buf[500];
				sprintf(Buf, "FDICopy (%s) -> starting extraction", s8_Name);
				OutputDebugStringA(Buf);
			#endif

			// Extract all files which start in the splitted CAB with the name in s8_Name
			if (!mf_FdiCopy(mh_FDIContext, s8_Name, s8_Path, 0, (PFNFDINOTIFY)(FDICallback), NULL, this))
				return FALSE;

			if (stricmp(s8_Name, ms8_LastCab) == 0)
				break; // no more splitted parts following
			else
				strcpy (s8_Name, ms8_LastCab);
		}
		return TRUE;
	}


	// Determines whether the cabinet with the specified handle is a valid cabinet. If it is, the structure
	// which is pointed to by pfdici will be filled with information about the cabinet file. This parameter
	// can be NULL in which case no information about the cabinet file will be returned.
	BOOL IsCabinet(const char* s8_CabFile, PFDICABINETINFO pfdici = NULL)
	{
		mu32_OrgError = 0;

		if (!mh_FDIContext)
			return FALSE;

		Store().Clear();
		strcpy(Store().s8_CabFile, s8_CabFile);

		int  handle = FDIOpen(s8_CabFile, _O_BINARY | _O_RDONLY | _O_SEQUENTIAL, 0);
		if (!handle)
			return FALSE;

		BOOL bRet;
		if (!pfdici)
		{
			 FDICABINETINFO fdici;
			 bRet = mf_FdiIsCabinet(mh_FDIContext, handle, &fdici);
		}
		else bRet = mf_FdiIsCabinet(mh_FDIContext, handle, pfdici);

		FDIClose(handle);
		return bRet;
	}

	// Returns the last error that has occured. Some errors have optional error information. This can be
	// retrieved by specifying a pointer to the integer that is to hold the optional error information.
	int GetLastError(char **ps8_Description = 0, int* pnExtra = NULL)
	{
		// avoid error "User Aborted" if output file could not be written
		if (mu32_OrgError)
			mk_Error.erfOper = mu32_OrgError;

		if (ps8_Description)
		{
			switch (mk_Error.erfOper)
			{
			case FDIERROR_NONE:
				*ps8_Description = "No error"; break;
			case FDIERROR_CABINET_NOT_FOUND:
				*ps8_Description = "Cabinet file (or part of splitted CAB) not found"; break;
			case FDIERROR_NOT_A_CABINET:
				*ps8_Description = "Not a cabinet"; break;
			case FDIERROR_UNKNOWN_CABINET_VERSION:
				*ps8_Description = "Unknown cabinet version"; break;
			case FDIERROR_CORRUPT_CABINET:
				*ps8_Description = "Corrupt cabinet"; break;
			case FDIERROR_ALLOC_FAIL:
				*ps8_Description = "Memory allocation failed"; break;
			case FDIERROR_BAD_COMPR_TYPE:
				*ps8_Description = "Unknown compression type"; break;
			case FDIERROR_MDI_FAIL:
				*ps8_Description = "Failure decompressing data"; break;
			case FDIERROR_TARGET_FILE:
				*ps8_Description = "Failure writing to target file"; break;
			case FDIERROR_RESERVE_MISMATCH:
				*ps8_Description = "Cabinets in set have different RESERVE sizes"; break;
			case FDIERROR_WRONG_CABINET:
				*ps8_Description = "Cabinet returned on fdintNEXT_CABINET is incorrect"; break;
			case FDIERROR_USER_ABORT:
				*ps8_Description = "User aborted"; break;
			case FDIERROR_LOAD_DLL:
				*ps8_Description = "Could not load Cabinet.dll"; break;
			default:
				*ps8_Description = "Unknown error"; break;
			}
		}

		if (!mk_Error.fError)
			return 0;

		if (pnExtra)
			*pnExtra = mk_Error.erfType;

		return mk_Error.erfOper;
	}

	// Structures used as parameters for the notification functions.
	typedef struct kCabinetInfo
	{
		LPSTR   s8_NextCabinet;  // Name of the next cabinet excluding path info
		LPSTR   s8_NextDisk;     // Name of the next disk
		LPSTR   s8_Path;         // Path to the cabinet
		USHORT u16_SetID;        // The set ID of this cabinet
		USHORT u16_Cabinet;      // The cabinet number within the cabinet set (starting at 0)
	};

	typedef struct kCabinetFileInfo
	{
		LPSTR   s8_File;      // Only the filename of the file
		LPSTR   s8_SubFolder; // The subfolder in the CAB file
		LPSTR   s8_Path;      // The full path to the file on disk
		LPSTR   s8_FullPath;  // The full path to the file plus the filename
		LONG   s32_Size;      // Uncompressed file size
		FILETIME k_Time;      // Time and date of the file
		USHORT u16_Attribs;   // Attributes of the file
	};

protected:
	// Allocates a new block of memory and returns the pointer to the newly allocated memory. 
	// This function can be overridden.
	static void* Alloc(UINT size)
		{ return operator new(size); }

	// Frees a block of memory allocated by Alloc. 
	// This function can be overridden.
	static void Free(void* memblock)
		{ operator delete(memblock); }

	// Opens a file. (CAB file and extracted files)
	// This function is overridden in ExtractMemoryT
	static int Open(const char * filename, int oflag, int pmode)
	{ 
		// remove write protection (if file already exists)
		if (oflag & _O_CREAT)
			SetFileAttributesA(filename, FILE_ATTRIBUTE_NORMAL);

		return _open(filename, oflag, pmode); 
	}

	// Reads a file. (CAB file and extracted files)
	// This function is overridden in ExtractMemoryT
	static int Read(int fp, void* memory, UINT count)
		{ return _read(fp, memory, count); }

	// Writes a file. (CAB file is not written in the extraction process)
	// This function is overridden in ExtractMemoryT
	static int Write(int fp, const void* memory, UINT count)
		{ return _write(fp, memory, count); }

	// Closes a file. (CAB file and extracted files)
	// This function is overridden in ExtractMemoryT
	static int Close(int fp)
		{ return _close(fp); }

	// Moves the file pointer. (CAB file and extracted files)
	// This function is overridden in ExtractMemoryT
	static long Seek(int fp, long offset, int seektype)
		{ return _lseek(fp, offset, seektype); }


	// Used to decrypt the CAB file and store handles of CAB files
	typedef struct kStore
	{
		// This map holds the file handles of the CAB file, which is opened TWICE by Cabinet.dll !!
		// It stores the handle of both opened CAB files and their current read / write positions
		CMap i_Map;

		char   s8_Key[5000];         // Holds the Encryption string if it has been set by SetEncryptionKey()
		int   s32_KeyLen;            // The length of the key
		char   s8_CabFile[MAX_PATH]; // The path to the CAB file file which is currently written

		// Constructor
		kStore()
		{
			 s8_Key[0]     = 0;
			s32_KeyLen     = 0;
			 s8_CabFile[0] = 0;
		}

		// clear all stored file handles
		void Clear() 
		{ 
			i_Map.Clear(); 
		}
		// returns TRUE if the given file handle is the CAB file
		BOOL IsCabFile(int s32_FileHandle)
		{
			return (i_Map.Find(s32_FileHandle) > -1);
		}
		// remove the given file handle from the map
		void Erase(int s32_FileHandle)
		{
			i_Map.Erase(s32_FileHandle);
		}
		// set a pair of file handle and file pointer
		void SetPtr(int s32_FileHandle, int s32_FilePointer)
		{
			// ATTENTION: FileHandle may be a negative value!!!
			if (s32_FileHandle == 0 || s32_FileHandle == -1) return;
			i_Map.Set(s32_FileHandle, s32_FilePointer);
		}
		// return the file pointer for the given file handle or -1 if not found
		int GetPtr(int s32_FileHandle)
		{
			int Pos = i_Map.Find(s32_FileHandle);
			if (Pos < 0) return -1;
			return i_Map.GetAt(Pos);
		}
	};

	// The static instance of the CabState structure
	static kStore& Store()
	{
		static kStore k_Store;
		return k_Store;
	}

	// Handle to the FDI context.
	HFDI mh_FDIContext;

	// Struct that is used for storing error information.
	ERF  mk_Error;

	// The origional error which caused an abortion of the extraction process
	UINT mu32_OrgError;

	// User defined parameter to pass to the callback function.
	void* mp_Param;

	// Flag that can be set to abort the current operation.
	BOOL mb_Abort;

	// Holds a pointer to the name of the target directory.
	char* ms8_TargetDir;

	// Stores the last CAB file of a spanned cabinet which has been read
	char  ms8_LastCab[MAX_PATH];

	typedef HFDI (DIAMONDAPI* fFdiCreate)   (PFNALLOC, PFNFREE, PFNOPEN, PFNREAD, PFNWRITE, PFNCLOSE, PFNSEEK, int, PERF);
	typedef BOOL (DIAMONDAPI* fFdiCopy)     (HFDI hfdi, char*, char*, int, PFNFDINOTIFY, PFNFDIDECRYPT, void*);
	typedef BOOL (DIAMONDAPI* fFdiIsCabinet)(HFDI, int, PFDICABINETINFO);
	typedef BOOL (DIAMONDAPI* fFdiDestroy)  (HFDI);

	HINSTANCE      mh_CabinetDll;
	fFdiCreate     mf_FdiCreate;
	fFdiCopy       mf_FdiCopy;
	fFdiIsCabinet  mf_FdiIsCabinet;
	fFdiDestroy    mf_FdiDestroy;

private:
	static int FDIOpen(const char* filename, int oflag, int pmode)
	{ 
		int fd = T::Open(filename, oflag, pmode);

		#if _TraceExtract
			char Buf[1000];
			sprintf(Buf, "FDIOpen (%s) --> Handle= 0x%08X", filename, fd);
			OutputDebugStringA("+++++++");
			OutputDebugStringA(Buf);
		#endif

		// The output CAB file has just been opened -> remember its handle
		if (stricmp(filename, Store().s8_CabFile) == 0)
			Store().SetPtr(fd, 0);

		return fd;
	}

	static int FDIRead(int fd, void* memory, UINT count)
	{ 
		int Read = T::Read(fd, memory, count); 

		#if _TraceExtract
			char Buf[1000];
			sprintf(Buf, "FDIRead (Handle= 0x%08X, Count= %05d) --> %05d Bytes read", fd, count, Read);
			OutputDebugStringA(Buf);
		#endif

		// Decrypt the data block if SetDecryptionKey() has been called before
		int s32_CabPtr = Store().GetPtr(fd);
		if (s32_CabPtr >= 0)
		{
			if (Store().s32_KeyLen)
			{
				DecryptDataBlock(memory, Read, Store().s8_Key, Store().s32_KeyLen, s32_CabPtr);
			}

			// remember the current file pointer in the CAB file
			Store().SetPtr(fd, s32_CabPtr + Read);
		}
		return Read;
	}

	static int FDIWrite(int fd, const void* memory, UINT count)
	{ 
		int Written = T::Write(fd, memory, count); 

		#if _TraceExtract
			char Buf[1000];
			sprintf(Buf, "FDIWrite(Handle= 0x%08X, Count= %05d) --> %05d Bytes written", fd, count, Written);
			OutputDebugStringA(Buf);
		#endif

		// Writing to CAB file does not happen while extracting
		// So we don't have to call Store().SetPtr() here

		return Written;
	}

	static int FDIClose(int fd)
	{ 
		#if _TraceExtract
			char Buf[1000];
			sprintf(Buf, "FDIClose(Handle= 0x%08X)", fd);
			OutputDebugStringA(Buf);
			OutputDebugStringA("-------");
		#endif

		int Err = T::Close(fd); // ORDER FIRST !!!

		// The output CAB file has just been closed -> reset its handle
		Store().Erase(fd); // ORDER AFTERWARDS !!!
		return Err;		
	}

	static long FDISeek(int fd, long offset, int seektype)
	{ 
		int Pos = T::Seek(fd, offset, seektype); 

		#if _TraceExtract
			char Buf[1000];
			switch (seektype)
			{
			case SEEK_SET: sprintf(Buf, "FDISeek (Handle= 0x%08X, SEEK_SET, Offset= %04d) --> Position= %04d", fd, offset, Pos); break;
			case SEEK_CUR: sprintf(Buf, "FDISeek (Handle= 0x%08X, SEEK_CUR, Offset= %04d) --> Position= %04d", fd, offset, Pos); break;
			case SEEK_END: sprintf(Buf, "FDISeek (Handle= 0x%08X, SEEK_END, Offset= %04d) --> Position= %04d", fd, offset, Pos); break;
			}
			OutputDebugStringA(Buf);
		#endif

		// remember the current position in the CAB file
		if (Store().IsCabFile(fd))
			Store().SetPtr(fd, Pos);

		return Pos;
	}

	static void* FDIAlloc(UINT size)
	{ 
		return T::Alloc(size); 
	}

	static void FDIFree(void* memblock)
	{ 
		T::Free(memblock); 
	}

	// Callback function for the FDI context. This will get used when the Copy function is called. This
	// function will call the appropriate handlers which can be overridden to change the default behaviour.
	static int FDICallback(FDINOTIFICATIONTYPE fdint, PFDINOTIFICATION pfdin)
	{
		#if _TraceExtract
			switch (fdint)
			{
				case fdintCABINET_INFO:    OutputDebugStringA("FDICallback(CABINET_INFO)");    break;
				case fdintNEXT_CABINET:    OutputDebugStringA("FDICallback(NEXT_CABINET)");    break;
				case fdintCOPY_FILE:       OutputDebugStringA("FDICallback(COPY_FILE)");       break;
				case fdintCLOSE_FILE_INFO: OutputDebugStringA("FDICallback(CLOSE_FILE_INFO)"); break;
				case fdintPARTIAL_FILE:    OutputDebugStringA("FDICallback(PARTIAL_FILE)");    break;
				case fdintENUMERATE:       OutputDebugStringA("FDICallback(ENUMERATE)");       break;
				default:                   OutputDebugStringA("FDICallback(unknown action)");  break;
			}
		#endif

		int nRet = 0; // Allow unsupported notifications to continue

		// Get pointer to the instance
		CExtractT<T>* pThis = (CExtractT<T>*)(pfdin->pv);

		// Abort if necessary
		if (pThis->mb_Abort)
			return -1;

		// Relay the notifications
		switch (fdint)
		{
		case fdintCABINET_INFO:
			{
				kCabinetInfo ci = {
					pfdin->psz1,
					pfdin->psz2,
					pfdin->psz3,
					pfdin->setID,
					pfdin->iCabinet
				};

				static_cast<T *>(pThis)->OnCabinetInfo(ci, pThis->mp_Param);	
			}
			break;

		case fdintNEXT_CABINET :
			{
				if (pfdin->fdie) // error occurred
				{
					// avoid error "User Aborted" if next cabinet file could not be found
					pThis->mu32_OrgError = pfdin->fdie;
					return -1;
				}

				// this is required to unpack all files from a spanned archive
				strcpy(pThis->ms8_LastCab, pfdin->psz1);

				kCabinetInfo ci = {
					pfdin->psz1,
					pfdin->psz2,
					pfdin->psz3,
					pfdin->setID,
					pfdin->iCabinet
				};

				static_cast<T *>(pThis)->OnNextCabinet(ci, pfdin->fdie, pThis->mp_Param);
			}
			break;

			// return  0 -> skip this file
			// return -1 -> abort extraction
		case fdintCOPY_FILE:
			{
				char   s8_SubFolder[MAX_PATH] = "";
				char   s8_Path     [MAX_PATH];
				char   s8_FullPath [MAX_PATH];
				strcpy(s8_Path, pThis->ms8_TargetDir);

				// Split the path to a filename and path
				char* s8_Slash = strrchr(pfdin->psz1, '\\');
				char* s8_File  = pfdin->psz1;

				if (s8_Slash)
				{
					s8_File = s8_Slash +1;

					strcpy(s8_SubFolder, pfdin->psz1);
					s8_SubFolder[s8_Slash - pfdin->psz1 + 1] = 0;
					
					strcat(s8_Path, s8_SubFolder);
				}

				sprintf(s8_FullPath, "%s%s", s8_Path, s8_File);

				kCabinetFileInfo k_FI;
				k_FI.s8_File      = s8_File;
				k_FI.s8_SubFolder = s8_SubFolder;
				k_FI.s8_Path      = s8_Path;
				k_FI.s8_FullPath  = s8_FullPath;
				k_FI.s32_Size     = pfdin->cb;
				k_FI.u16_Attribs  = pfdin->attribs;
				DosDateTimeToFileTime(pfdin->date, pfdin->time, &k_FI.k_Time);

				// Call notification function
				if (!static_cast<T *>(pThis)->OnBeforeCopyFile(k_FI, pThis->mp_Param))
					nRet = 0;
				else
				{
					char s8_Folder[MAX_PATH];

					// Make sure the directory exists
					s8_Slash = strchr(s8_Path, '\\');
					while (s8_Slash)
					{
						strncpy(s8_Folder, s8_Path, (s8_Slash - s8_Path));
						s8_Folder[s8_Slash - s8_Path] = 0;
						CreateDirectoryA(s8_Folder, NULL);
						s8_Slash = strchr(s8_Slash +1, '\\');
					}

					// Create the file
					strncat(s8_Path, s8_File, MAX_PATH);

					// IMPORTANT:
					// If _O_TRUNC is not set, this class will create corrupt files on disk
					// if the file to be written already exists and is bigger than the one in the CAB
					nRet = FDIOpen(s8_Path, _O_TRUNC | _O_BINARY | _O_CREAT | _O_WRONLY | _O_SEQUENTIAL, _S_IREAD | _S_IWRITE);

					// avoid error "User Aborted" if output file could not be written
					if (nRet <= 0) pThis->mu32_OrgError = FDIERROR_TARGET_FILE;
				}
			}
			break;

		case fdintCLOSE_FILE_INFO:
			{
				char s8_Path[MAX_PATH] = {0};

				// Close the file
				FDIClose((int)pfdin->hf);

				// Set the file attributes
				strcpy (s8_Path, pThis->ms8_TargetDir);
				strncat(s8_Path, pfdin->psz1, MAX_PATH);
				SetAttribsAndDate(s8_Path, pfdin->date, pfdin->time, pfdin->attribs);

				// Call notification function
				static_cast<T *>(pThis)->OnAfterCopyFile(pfdin->psz1, pThis->mp_Param);
				nRet = TRUE;
			}
			break;
		}

		return nRet;
	}

protected:
	// Sets the date and attributes for the specified file.
	static void SetAttribsAndDate(LPCSTR lpszFileName, USHORT uDate, USHORT uTime, USHORT uAttribs)
	{
		HANDLE hFile = CreateFileA(lpszFileName, GENERIC_READ | GENERIC_WRITE,
			                       FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			FILETIME ftFile;
			if (DosDateTimeToFileTime(uDate, uTime, &ftFile))
			{
				FILETIME ftLocal;
				if (LocalFileTimeToFileTime(&ftFile, &ftLocal))
					SetFileTime(hFile, &ftLocal, NULL, &ftLocal);
			}

			CloseHandle(hFile);
		}

		SetFileAttributesA(lpszFileName, uAttribs & (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE));
	}

	// Decrypt the data block
	// You can override this function to use your own decryption algorithm
	static void DecryptDataBlock(void*   p_Memory,  // data in memory to be decrypted
	                             UINT  u32_Count,   // size of memory block in Bytes
						         char*  s8_Key,     // the decryption key
						         int   s32_KeyLen,  // the legth of s8_Key
						         int   s32_CabPtr)  // a pointer indicating the absolute position in the CAB file where this block starts
	{
		// Restore the first 4 bytes which identify the CAB file
		if (s32_CabPtr == 0)
		{
			((char*)p_Memory)[0] = 'M';
			((char*)p_Memory)[1] = 'S';
			((char*)p_Memory)[2] = 'C';
			((char*)p_Memory)[3] = 'F';
		}

		int k = s32_CabPtr;

		// for each byte of the memory block we subtract the corresponding byte of the encryption key
		// and a value which depends on the position in the file
		for (UINT m=0; m<u32_Count; m++, k++)
		{
			k %= s32_KeyLen;

			// We let the first 0x3C Bytes of the file unencrypted (file header)
			if (s32_CabPtr + m < 0x3C)
				continue;

			((unsigned char*)p_Memory)[m] -= ((unsigned char*)s8_Key)[k];         // SUBTRACT
			((unsigned char*)p_Memory)[m] ^= ((s32_CabPtr + m + s8_Key[0]) %101); // EXOR
		}
	}

	// ########################### CALLBACKS #######################


	// This function will be called for each file in a cabinet at the moment it is about to be extracted. This
	// function can be overridden.
	// Return TRUE to extract the file to the path specified by the second parameter or FALSE to skip.
	BOOL OnBeforeCopyFile(kCabinetFileInfo &k_FI, void* p_Param)
	{
		return TRUE;
	}

	// This function will be called when a file is succesfully extracted.
	void OnAfterCopyFile(LPCSTR s8_File, void* p_Param)
	{ 
	}

	// This function will be called exactly once for each cabinet opened by Copy, including continuation
	// cabinets opened due to files spanning cabinet boundaries. This function can be overridden.
	void OnCabinetInfo(kCabinetInfo &ci, void* p_Param)
	{ 
	}

	// This function will be called when the next cabinet file in the sequence needs to be opened. The path
	// has to be verified and can be changed if necessary. If the cabinet file cannot be opened this function
	// will be called again with the second parameter set to an error that describes the problem.
	void OnNextCabinet(kCabinetInfo &ci, FDIERROR s32_Error, void* p_Param)
	{ 
	}
};

class CExtract : public CExtractT<CExtract> { };

} // Namespace Cabinet