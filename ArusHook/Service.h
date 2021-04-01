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

class DllExport IService
{
public:
    virtual ~IService();

    /// Get the name of the service (must be unique, case insensitive)
    virtual const ch *GetServiceName() = 0;

    // Get a comma separated list of services required for this service
    virtual const ch *GetDependencies() = 0;
    
    // Executed when the service is started
    virtual void OnServiceStart() = 0;

    // Executed when the service is stopped
    virtual void OnServiceStop() = 0;
};
