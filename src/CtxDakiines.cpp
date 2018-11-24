#include "lib.h"
#include "libTypes.h"
#include "libMngInput.h"
#include "libMngSound.h"
#include "dfcApp.h"

#include "CtxDakiines.h"
#include "video.h"
#include "ui.h"
#include "game.h"
#include "snd.h"

CPhysObj*
CCtxDakiines::UserInstall(char *p_dataSource, char* p_id)
{
CPhysObj *pRet=0;

	if(strcmp(p_id, "CLSID_CRenderScreen") == 0)
		pRet= new CRenderScreen;

	return pRet;
}

void
CCtxDakiines::Init(void)
{
	SetNbChilds(32);
}

void
CCtxDakiines::Activate(void)
{
CMngSound	*mngSound= GetMngSound();

	CContext::Activate();

	video_Init();
//	m_romfile = read_romimage("roms/nes/gzs.nes");
//	m_romfile = read_romimage("roms/nes/Final Fantasy (U).nes");
	m_romfile = read_romimage("roms/nes/Final Fantasy (U).nes");
//	m_romfile = read_romimage("roms/nes/Lunar Pool (U).nes");
//	m_romfile = read_romimage("roms/coleco/Burgertime (1982-84).col");
//	m_romfile = read_romimage("roms/genesis/Wonder Boy in Monster World.bin");
//	m_romfile = read_romimage("roms/pce/Parodius (J).pce");
//	m_romfile = read_romimage("roms/sms/WONDERML.sms");
//	m_romfile = read_romimage("roms/sms/Wonder Boy 3.gg");
	m_system_type = guess_system(m_romfile);
	m_objRender= new CRenderScreen;

	AddChild(m_objRender, false);

	joypad_Init(m_system_type);
	activate_system(m_system_type, m_romfile);

	m_nDelayToEmule= 16;

	CFont::Find("Font8.font");

	mngSound->SetUserFetchSound(snd_UserFetchSound);
}

void
CCtxDakiines::Deactivate(void)
{
	emulation_Shutdown();
	video_Shutdown();
	CContext::Deactivate();
}

void
CCtxDakiines::Manage(u32 p_nTimeTick)
{
CMngInput *mngInput= GetMngInput();

	if(mngInput->IsLogicKeyDown(IK_ESCAPE))
		System_Quit();

	if((m_nDelayToEmule-= p_nTimeTick) < 0)
	{
		emulation_Timeslice(0);
		m_nDelayToEmule+= 16;
	}

	CContext::Manage(p_nTimeTick);
}

void
CCtxDakiines::Update(void)
{
	CContext::Update();
}
