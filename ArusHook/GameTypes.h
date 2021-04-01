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

namespace GameTypes
{
    class scrNativeCallContext
    {
    protected:
        typedef struct { f32 X, Y, Z; } Vector3;
        typedef struct { f32 X, Y, Z, W; } Vector4;

        ptr m_pReturn;
        u32 m_nArgCount;
        ptr m_pArgs;
        
        u32 m_nDataCount;
        Vector3 *m_pOriginalData[4];
        Vector4 m_TemporaryData[4];
    };

}
