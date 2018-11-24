#ifndef _TEST_APP_H
#define _TEST_APP_H

#include "dfcApp.h"


#define CTX_DAKIINES	0
#define MAX_CTX			1

class CAppDakiines : public CApp
{
public:
	CAppDakiines();
	virtual ~CAppDakiines(){};

	virtual void OnPostCreate(void);
};

extern CAppDakiines *g_TheAppDakiines;

#endif //_TEST_APP_H
