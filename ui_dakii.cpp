/*
 * ui_dakii.cpp
 *
 * Emulation frontend and input managment interface over Dakii engine
 */

#include "lib.h"
#include "libTypes.h"
#include "libMacros.h"
#include "libMngInput.h"

#include "ui.h"
#include "system.h"

u32 keyColeco[12]= 
	{IK_0, IK_1, IK_2, IK_3, IK_4, IK_5, IK_6, IK_7, IK_8, IK_9, IK_A, IK_B};

u32 joyOnKeyNes[2][8]= 
{
	{IK_Z, IK_X, IK_SPACE, IK_RETURN, IK_ARROWUP, IK_ARROWDOWN, IK_ARROWLEFT, IK_ARROWRIGHT}, 
	{0, 0, 0, 0, 0, 0, 0, 0}
};

u32 joyOnKeyPce[2][8]= 
{
	{IK_Z, IK_X, IK_SPACE, IK_RETURN, IK_ARROWUP, IK_ARROWRIGHT, IK_ARROWDOWN, IK_ARROWLEFT}, 
	{0, 0, 0, 0, 0, 0, 0, 0}
};

u32 joyOnKeyGen[2][11]= 
{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

u32 joyOnKeySms[2][7]	= 
{
	{IK_ARROWUP, IK_ARROWDOWN, IK_ARROWLEFT, IK_ARROWRIGHT, IK_Z, IK_X, IK_RETURN}, 
	{0, 0, 0, 0, 0, 0, 0}
};

u32 joyOnKeyGG[7]= 
	{IK_ARROWUP, IK_ARROWDOWN, IK_ARROWLEFT, IK_ARROWRIGHT, IK_Z, IK_X, IK_RETURN};

u32 joyOnKeyColeco[6]= 
	{IK_ARROWUP, IK_ARROWRIGHT, IK_ARROWDOWN, IK_ARROWLEFT, IK_Z, IK_X};
	
u32 joyOnKeyMsx[8]		= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnKeySG1000[8]	= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnKeySC3000[8]	= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnKeyGB[8]		= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnKeySnes[5][12]	= 
{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

u32 joyOnKeyLynx[8]		= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnKeyJaguar[8]	= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnKeyApple2[8]	= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnKeyMac[8]		= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnKeyPsx[14]		= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnKeyN64[13]		= 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnKeySaturn[8]	= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnJoyNes[2][8]= 
{	//A	B Select Start Up Down Left Right
	{IJ_BTN2, IJ_BTN1, IJ_BTN8, IJ_BTN9, IJ_UP, IJ_DOWN, IJ_LEFT, IJ_RIGHT}, 
	{0, 0, 0, 0, 0, 0, 0, 0}
};

u32 joyOnJoyPce[2][8]= 
{
	{IJ_BTN2, IJ_BTN1, IJ_BTN8, IJ_BTN9, IJ_UP, IJ_RIGHT, IJ_DOWN, IJ_LEFT}, 
	{0, 0, 0, 0, 0, 0, 0, 0}
};

u32 joyOnJoyGen[2][11]= 
{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

u32 joyOnJoySms[2][7]	= 
{
	{IJ_UP, IJ_DOWN, IJ_LEFT, IJ_RIGHT, IJ_BTN2, IJ_BTN1, IJ_BTN9}, 
	{0, 0, 0, 0, 0, 0, 0}
};

u32 joyOnJoyGG[7]= 
	{IJ_UP, IJ_DOWN, IJ_LEFT, IJ_RIGHT, IJ_BTN2, IJ_BTN1, IJ_BTN9};

u32 joyOnJoyColeco[6]= 
	{IJ_UP, IJ_RIGHT, IJ_DOWN, IJ_LEFT, IJ_BTN2, IJ_BTN1};
	
u32 joyOnJoyMsx[8]		= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnJoySG1000[8]	= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnJoySC3000[8]	= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnJoyGB[8]		= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnJoySnes[5][12]	= 
{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

u32 joyOnJoyLynx[8]		= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnJoyJaguar[8]	= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnJoyApple2[8]	= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnJoyMac[8]		= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnJoyPsx[14]		= 
	{0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnJoyN64[13]		= 
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
u32 joyOnJoySaturn[8]	= 
	{0, 0, 0, 0, 0, 0, 0, 0};

u32 *AllJoyOnKey[ST_END][5]= 
{
	{0, 0, 0, 0, 0},
	{joyOnKeyNes[0], joyOnKeyNes[1], 0, 0, 0},
	{joyOnKeyPce[0], joyOnKeyPce[1], 0, 0, 0},
	{joyOnKeyGen[0], joyOnKeyGen[1], 0, 0, 0},
	{joyOnKeySms[0], joyOnKeySms[1], 0, 0, 0},
	{joyOnKeyGG, joyOnKeyGG, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{joyOnKeyColeco, 0, 0, 0, 0},
	{joyOnKeyMsx, 0, 0, 0, 0},
	{joyOnKeySG1000, 0, 0, 0, 0},
	{joyOnKeySC3000, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
};
u32 *AllJoyOnJoy[ST_END][5]= 
{
	{0, 0, 0, 0, 0},
	{joyOnJoyNes[0], joyOnJoyNes[1], 0, 0, 0},
	{joyOnJoyPce[0], joyOnJoyPce[1], 0, 0, 0},
	{joyOnJoyGen[0], joyOnJoyGen[1], 0, 0, 0},
	{joyOnJoySms[0], joyOnJoySms[1], 0, 0, 0},
	{joyOnJoyGG, joyOnJoyGG, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{joyOnJoyColeco, 0, 0, 0, 0},
	{joyOnJoyMsx, 0, 0, 0, 0},
	{joyOnJoySG1000, 0, 0, 0, 0},
	{joyOnJoySC3000, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
};
u32 AllJoyLenght[ST_END]= 
{
	0,
	8,
	8,
	11,
	7,
	7,
	0,
	0,
	0,
	0,
	0,
	0,
	6,
	8,
	8,
	8,
	0,
	0,
	0,
	0,
};

u32 *AllKey[ST_END][5]= 
{
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{keyColeco, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0},
};
u32 AllKeyLenght[ST_END]= 
{
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	12,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};

u32 *joyOnKeyRegistered[5];
u32 *joyOnJoyRegistered[5];
u32 joyRegisteredLenght[5];

u32 *keyRegistered[5];
u32 keyRegisteredLenght[5];

void (*dn_shutdown)(void)= 0;
void (*timeslice)(void *)= 0;
void *timeslice_data;

void 
emulation_SetTimeslice(void (*proc)(void *), void *data)
{
	timeslice = proc;
	timeslice_data = data;
}

int 
emulation_Timeslice(void* client_data)
{
	if(timeslice)
	{
		timeslice(timeslice_data);
		return 1;
	}

	return 0;
}

void
emulation_Shutdown(void)
{
	if(dn_shutdown)
		dn_shutdown();
}

void
emulation_SetShutdown(void (*proc)(void))
{
	dn_shutdown= proc;
}

void 
joypad_Init(u32 p_idSystem)
{
	for(int i= 0; i < 5; ++i)
	{
		joyOnKeyRegistered[i]	= AllJoyOnKey[p_idSystem][i];
		joyOnJoyRegistered[i]	= AllJoyOnJoy[p_idSystem][i];
		joyRegisteredLenght[i]	= AllJoyLenght[p_idSystem];
		
		keyRegistered[i]		= AllKey[p_idSystem][i];
		keyRegisteredLenght[i]	= AllKeyLenght[p_idSystem];
	}
}

void 
joypad_Update(u32 p_index, u32 *pad)
{
CMngInput *mngInput= GetMngInput();

	(*pad)= 0;

	for(u32 i= 0; i < joyRegisteredLenght[p_index]; ++i)
	{
		if(mngInput->IsKeyDown(joyOnKeyRegistered[p_index][i]))
			(*pad)|= 1<<i;
		if(joyOnJoyRegistered[p_index])
			if(mngInput->IsJoystickKeyDown(p_index, joyOnJoyRegistered[p_index][i]))
				(*pad)|= 1<<i;			
	}
}

void 
keypad_Update(u32 p_index, u32 *pad)
{
CMngInput *mngInput= GetMngInput();

	(*pad)= 0;

	for(u32 i= 0; i < keyRegisteredLenght[p_index]; ++i)
		if(mngInput->IsKeyDown(keyRegistered[p_index][i]))
			(*pad)|= 1<<i;
}
