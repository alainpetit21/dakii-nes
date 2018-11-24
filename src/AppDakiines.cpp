#include "lib.h"
#include "libTypes.h"
#include "libMacros.h"
//#include "libMngMemory.h"
#include "AppDakiines.h"

#include "CtxDakiines.h"

CAppDakiines	*g_TheAppDakiines;

CAppDakiines::CAppDakiines():
CApp("AppDakiines.application")
{
}

void
CAppDakiines::OnPostCreate(void)
{
	Init(MAX_CTX);

	RegisterContext(CTX_DAKIINES, new CCtxDakiines("CtxDakiines.context"));

	SetCurContext(CTX_DAKIINES);

	CApp::OnPostCreate();
}

CApp*
newGameApp(void)
{
//	GetMngMem()->SetSize(2*1024*1024);
	return g_TheAppDakiines= new CAppDakiines();
}


ENTRY_POINT("Dakiines");
