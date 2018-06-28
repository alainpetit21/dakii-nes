#ifndef CTX_DAKIINES_H
#define CTX_DAKIINES_H

#include "dfcContext.h"
#include "dfcEntity.h"
#include "dfc3DEntity.h"
#include "dfcLabel.h"
#include "tool.h"
#include "video.h"
#include "game.h"

class CRenderScreen : public  CPhysObj
{
public :
	IMPLEMENT_RTTI(RenderScreen, PhysObj);

	virtual void Update(void)
	{
		CMngGraphic *mngGraphic= GetMngGraphic();
		
		void *pImage= video_GetEmuScreen();
		
		mngGraphic->DrawBitmap(0, 332 - (video_GetEmuScreenWidth()>>1), 256 - (video_GetEmuScreenHeight()>>1), video_GetEmuScreenWidth(), video_GetEmuScreenHeight(), 0, 1, 0, pImage);
	};
};

class CCtxDakiines : public CContext
{
public:
	CCtxDakiines(char* p_pRes):CContext(p_pRes){};

	virtual CPhysObj*	UserInstall(char *p_dataSource, char *p_id);
	virtual void 		Init(void);
	virtual void 		Activate(void);
	virtual void 		Deactivate(void);
	virtual void 		Manage(unsigned long p_nTimeTick);
	virtual void 		Update(void);

	CRenderScreen	*m_objRender;
	rom_file		*m_romfile;
	int 			m_system_type;
	
	s32				m_nDelayToEmule;
};

#endif //CTX_DAKIINES_H
