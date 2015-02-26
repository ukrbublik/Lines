#ifndef __LINES_SKIN__
#define __LINES_SKIN__

#include "atlimage.h"			// ATL-библиотека для изображений


struct Files
{
	CString	strBackground;
	CString	strBalls;
	CString	strNext;
	CString	strScore;
	CString	strTime;
	CString strType;
};

struct Images
{
	CImage	imgBackground;
	CImage	imgBalls;
	CImage	imgNext;
	CImage	imgScore;
	CImage	imgTime;
	CImage	imgType;
};

struct SizesOfElements
{
	CSize	sizeBall;
	CSize	sizeNext;
	CSize	sizeScore;
	CSize	sizeTime;
	CSize	sizeType;
};

struct PlacementOfElements
{
	CPoint	pointUpperLeftBall;
	CSize	sizeBetweenBalls;
	CPoint	pointMyScore;
	UINT	uBetweenMyScore;
	CPoint	pointHighScore;
	UINT	uBetweenHighScore;
	CPoint	pointTime;
	UINT	uBetweenTime;
	UINT	uTimeComma;
	CPoint	pointNext1;
	CPoint	pointNext2;
	CPoint	pointNext3;
	CPoint	pointType;

};

class AnimationOfBalls
{
public:
	UINT	uFramesBall_Total;

	UINT*	pFramesBall_Jump;
	UINT	nFramesBall_Jump;

	UINT*	pFramesBall_Appearence;
	UINT	nFramesBall_Appearence;

	UINT*	pFramesBall_Destruction;
	UINT	nFramesBall_Destruction;

	UINT	uFrameBall_Hint;

	AnimationOfBalls()
	{
		pFramesBall_Jump = NULL;
		pFramesBall_Appearence = NULL;
		pFramesBall_Destruction = NULL;
	}

	~AnimationOfBalls()
	{
		if(pFramesBall_Jump)
			delete [] pFramesBall_Jump;
		if(pFramesBall_Appearence)
			delete [] pFramesBall_Appearence;
		if(pFramesBall_Destruction)
			delete [] pFramesBall_Destruction;
	}
};

struct Delays
{
	UINT	uDelayJump;
	UINT	uDelayAppearence;
	UINT	uDelayDestruction;
	UINT	uDelayMove;
};

struct CopyRights
{
	CString strCopyrightLine1;
	CString strCopyrightLine2;
	CString strDeveloperContactInfo;
};

class Skin
{
public:
	Files files;
	Images images;
	SizesOfElements sizesOfElements;
	PlacementOfElements placementOfElements;
	AnimationOfBalls animationOfBalls;
	Delays delays;
	CopyRights copyrights;
};

#endif //__LINES_SKIN__