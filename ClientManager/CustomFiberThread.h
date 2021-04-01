#pragma once

/*#include "..\ArusHook\ScriptThread.h"
#include "..\ArusHook\ScriptingEnums.h"
#include "..\ArusHook\ScriptingTypes.h"

class CustomFiberThread : 
	public ScriptThread
{
private:

protected:
    // We don't want a Tick based script, so we override RunScript.
	void RunScript();

public:
	CustomFiberThread();
	~CustomFiberThread();
};*/

extern unsigned long base;

extern bool mustLaunch;

void StartScript();
void PulseScript();