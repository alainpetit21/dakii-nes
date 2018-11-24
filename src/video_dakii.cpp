/*
 * video_dakii.cpp
 *
 * Display screen management. Interface over Dakii engine
 */

#include <stdio.h>
#include <stdlib.h>

#include "lib.h"
#include "libMacros.h"
#include "libMngGraphic.h"

#include "video.h"
#include "tool.h"
#include "ui.h"

u8	*vid_pre_xlat;
u16	*xlatepal16;
u32	*xlatepal32;

void	*imageDst= NULL;
u8		*imageSrc= NULL;
u32		imageWidth;
u32		imageHeight;

void
video_Init(void)
{
	vid_pre_xlat= 0;
	xlatepal16= 0;
	xlatepal32= 0;
}

void
video_Shutdown(void)
{
	SAFE_DELETE(vid_pre_xlat);
	SAFE_DELETE(xlatepal16);
	SAFE_DELETE(xlatepal32);
}

void
video_InitTranslate16(int nbColors, int *red, int *green, int *blue)
{
int i;

	vid_pre_xlat = new u8[nbColors];
	xlatepal16 = new u16[nbColors << 1];

	for(i = 0; i < nbColors; i++)
	{
		vid_pre_xlat[i] = i;
		xlatepal16[i] = ((red[i]>>3)<<11)|((green[i]>>2)<<5)|(blue[i]>>3);
	}
}

void
video_InitTranslate32(int nbColors, int *red, int *green, int *blue)
{
int i;

	vid_pre_xlat = new u8[nbColors];
	xlatepal32 = new u32[nbColors << 2];

	for(i = 0; i < nbColors; i++)
	{
		vid_pre_xlat[i] = i;
		xlatepal32[i] = (red[i]<<16)|(green[i]<<8)|(blue[i]);
	}
}

void
video_Translate16(u16 *dest, u8 *src, int size)
{
int i;

	for(i = 0; i < size; ++i)
		dest[i]= xlatepal16[src[i]];
}

void
video_Translate32(u32 *dest, u8 *src, int size)
{
int i;

	for(i = 0; i < size; ++i)
		dest[i]= xlatepal32[src[i]];
}

void
video_SetSize(int x, int y)
{
CMngGraphic	*pRenderer	= GetMngGraphic();

	if(imageDst)
		pRenderer->FreeBitmap(&imageDst);
	if(imageSrc)
		delete [] imageSrc;

	imageDst= pRenderer->NewBitmap(imageWidth= x, imageHeight= y, 1);
	imageSrc= new u8[x*y];
}

void
video_BuildFrameBuffer(void)
{
CMngGraphic	*pRenderer	= GetMngGraphic();
u32 		*pixel		= (u32*)pRenderer->LockSurface(imageDst);

	video_Translate32(pixel, (u8*)imageSrc, imageWidth*imageHeight);

	pRenderer->UnLockSurface(imageDst, pixel);
}

u8*
video_GetScanline(int line)
{
	return &imageSrc[line * imageWidth];
}

void
video_SetPal(int num_colors, int *red, int *green, int *blue)
{
	if(GetMngGraphic()->GetPixelMode() == 16)
		video_InitTranslate16(num_colors, red, green, blue);
	else if(GetMngGraphic()->GetPixelMode() == 32)
		video_InitTranslate32(num_colors, red, green, blue);

}

void*
video_GetEmuScreen(void)
{
	return imageDst;
}

u32
video_GetEmuScreenWidth(void)
{
	return imageWidth;
}

u32
video_GetEmuScreenHeight(void)
{
	return imageHeight;
}

