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
#include "GameTypes.h"

class IGameVersion;

class DllExport Game
{
public:
    enum eVersion
    {
        VersionAutoDetect,
        Version101,
        Version102,
        Version103,
        Version104,
        Version104u5,
        VersionMax,
    };

public:

    static void Initialize(eVersion version);

    static u32 GetBase();

    static IGameVersion *GetInstance();

    static ptr GetNativeAddress(const ch *name);
    static ptr GetNativeAddressFromHash(u32 hash);

    static ptr GetGlobalAddress(u32 index);

    static eVersion GetVersion();

    template <typename T>
    static T GetGlobalValue(u32 index)
    {
        return *ptr_cast<T>(GetGlobalAddress(index));
    }

    template <typename T>
    static void SetGlobalValue(u32 index, T value)
    {
        *ptr_cast<T>(GetGlobalAddress(index)) = value;
    }
};
