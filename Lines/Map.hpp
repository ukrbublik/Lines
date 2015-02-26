///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Author: Elmü (http://kickme.to/elmue)
// Date: 23-11-2006
//
// Filename: Map.hpp
//
// Classes:
// - CMap
//
// Purpose: This tiny class is a simple (and ultra fast) replacement for std::map
// It is strongly recommended to use this class intead, because using #include <map>
// will create a dependency to MSVCP70.DLL or MSVCP71.DLL or MSVCP80.DLL
// If you use std::map you have to deliver one of these DLLs with you application 
// because these DLLs are not available on every computer.
// Using CMap instead removes this problem!
//

#pragma once

namespace Cabinet
{

// Dynamic memory management is not necessary for this simple class
// we will never need more than 2 entries! (the handles of the CAB file)
// If the maximum is exceeded an exception will be thrown, 
// but this will never happen as long as CMap is used only in the cabinet library
#define __MaxEntries 100

class CMap
{
private:
	struct kData
	{
		int Key;
		int Value;
	};

	int    ms32_Count;
	kData  mk_Data[__MaxEntries];

public:
	CMap()
	{
		Clear();
	}

	void Clear()
	{
		ms32_Count = 0;
	}

	int Count()
	{
		return ms32_Count;
	}

	// Returns the zero based index of the Key or -1 if not found
	int Find(int Key)
	{
		for (int i=0; i<ms32_Count; i++)
		{
			if (mk_Data[i].Key == Key)
				return i;
		}
		return -1;
	}

	// Delete the given Key if it exists
	void Erase(int Key)
	{
		int Pos = Find(Key);
		if (Pos < 0)
			return;

		ms32_Count --;
		memmove(&mk_Data[Pos], &mk_Data[Pos+1], sizeof(kData) * (ms32_Count -Pos));
	}

	// Sets a value for the given key
	// If the key already exists, its value is overwritten
	// If the key does not yet exist, it is created
	void Set(int Key, int Value)
	{
		int Pos = Find(Key);
		if (Pos < 0)
		{
			if (ms32_Count >= __MaxEntries)
				throw "Overflow in CMap::Set()";

			Pos = ms32_Count; // create new entry
			ms32_Count ++;
		}

		mk_Data[Pos].Key   = Key;
		mk_Data[Pos].Value = Value;
	}

	// Returns the value of the Key at the given position
	// Before you must retrieve the position with Find(Key)
	// Do not call GetAt() if Find() returned -1 !!
	int GetAt(int Pos)
	{
		if (Pos < 0 || Pos >= ms32_Count)
			throw "Invalid position in CMap::Get()";

		return mk_Data[Pos].Value;
	}
};

} // Namespace Cabinet
