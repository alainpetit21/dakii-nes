#ifndef __VIDEO_H__
#define __VIDEO_H__

void	video_Init				(void);
void	video_Shutdown			(void);
void	video_BuildFrameBuffer	(void);
void	video_SetSize			(int x, int y);
void	video_SetPal			(int num_colors, int *red, int *green, int *blue);
u8*		video_GetScanline		(int line);
void*	video_GetEmuScreen		(void);
u32		video_GetEmuScreenWidth	(void);
u32		video_GetEmuScreenHeight(void);

extern u8 *vid_pre_xlat;

#endif /* __VIDEO_H__ */
