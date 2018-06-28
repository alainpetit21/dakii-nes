#ifndef UI_H
#define UI_H

#include "types.h"

/*Control Fonctions*/
void	emulation_SetTimeslice		(void (*proc)(void *), void *data);
void	emulation_SetShutdown		(void (*proc)(void));
void	emulation_Shutdown			(void);
int		emulation_Timeslice			(void* client_data);
void	joypad_Update				(u32 p_nIndex, u32 *pad);
void 	joypad_Init					(u32 p_idSystem);
void	keypad_Update				(u32 p_nIndex, u32 *pad);

#endif /* UI_H */
