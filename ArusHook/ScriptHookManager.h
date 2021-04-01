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
#include "Service.h"
#include <vector>
#include <windows.h>

class ScriptThread;
class NativeThread;
class IScriptHookController;

class DllExport ScriptHookManager
{
public:
    // Version
    static u32 GetScriptHookVersion();
    static const ch * GetScriptHookVersionIdentifier();

    // Threads
    static void RegisterThread(ScriptThread *thread);
    static void RegisterThread(ScriptThread *thread, HMODULE hMod);     // Using this is recommended if you dynamically allocated the thread
    static void UnregisterThread(ScriptThread *thread);
    static void StartThread(ScriptThread *thread);     // Start a thread manually

    // Services
    static void RegisterService(IService *service);
    static IService * RequestService(const ch *name);
    
    template<class T>
    static T * RequestService(const ch *name)
    {
        return static_cast<T*>( RequestService(name) );
    }

    // Threads (LEGACY SUPPORT)
    static void RegisterThread(NativeThread *thread);
    static void RegisterThread(NativeThread *thread, HMODULE hMod);     // Using this is recommended if you dynamically allocated the thread
    static void UnregisterThread(NativeThread *thread);
    static void StartThread(NativeThread *thread);     // Start a thread manually

    // Internal
    static IScriptHookController * GetController();

};
