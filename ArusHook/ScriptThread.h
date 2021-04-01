/*****************************************************************************\

  Copyright (C) 2009, Aru <oneforaru at gmail dot com>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Please see ScriptHook-License.txt for information on how this source/header
  file is licensed as part of the ScriptHook SDK.

\*****************************************************************************/

#pragma once

#include "Types.h"

class DllExport ScriptThread
{
    friend class ScriptBaseThread;
    friend class ScriptHookController;

private:
    ScriptBaseThread *m_pBaseThread;

protected:
    // Override one of the following:
    virtual void RunTick();         // For NativeThread replacement
    virtual void RunScript();       // For NativeFiberThread replacement

    void Wait(u32 timeMS);			// Override the Wait native
    void TerminateThisScript();		// Override the TerminateThisScript native

    b8 IsThreadAlive();

    virtual void OnStart();
    virtual void OnKill();

    void SetName(ch *szName);
    void Kill();

public:
    ScriptThread();
    virtual ~ScriptThread();
};