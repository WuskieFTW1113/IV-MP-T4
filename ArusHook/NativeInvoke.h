/*****************************************************************************\

  Copyright (C) 2009, Aru <oneforaru at gmail dot com>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Please see ScriptHook-License.txt for information on how this source/header
  file is licensed as part of the ScriptHook SDK.

\*****************************************************************************/

#pragma once

// SpeedyInvoke:
//   Uncomment/comment to enable/disable experimental SpeedyInvoke.
//
//   SpeedyInvoke rewrites parts of your executing code to 
//   enable direct Native calling without repeated lookups.
//
//   Should only be enabled on fully optimized release builds!!
#if !defined(_DEBUG)
#define USE_SPEEDY_INVOKE
#endif

#include "Types.h"
#include "NativeContext.h"
#include "Game.h"
#include "Log.h"

class NativeInvoke
{
private:
    typedef void (_cdecl *NativeCall)(GameTypes::scrNativeCallContext *cxt);

    // SpeedyInvoke(TM)
    static void DllExport SpeedyInvoke(NativeContext *cxt, u32 hash);

    static inline void Invoke(NativeContext *cxt, u32 hash)
    {
        ptr fn = Game::GetNativeAddressFromHash(hash);

        if (fn != 0)
        {
            NativeCall call = (NativeCall)fn;
            call(cxt);
        }
    }

    static inline void Invoke(NativeContext *cxt, ch *name)
    {
        ptr fn = Game::GetNativeAddress(name);

        // Commented out to reduce debug spam.
        //LogDebug("Invoking native: %s", name);

        if (fn != 0)
        {
            NativeCall call = (NativeCall)fn;
            call(cxt);
        }
    }

public:

    // Here be repetitive ugly code... x16 (thanks copy and paste)
    // Only because I didn't want to use variadic functions

    template <u32 Hash, typename R>
    static inline R Invoke()
    {
        NativeContext cxt;

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1>
    static inline R Invoke(T1 p1)
    {
        NativeContext cxt;

        cxt.Push(p1);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2>
    static inline R Invoke(T1 p1, T2 p2)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3>
    static inline R Invoke(T1 p1, T2 p2, T3 p3)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10, typename T11>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10, T11 p11)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);
        cxt.Push(p11);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10, typename T11, typename T12>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10, T11 p11, T12 p12)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);
        cxt.Push(p11);
        cxt.Push(p12);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10, typename T11, typename T12, typename T13>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10, T11 p11, T12 p12, T13 p13)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);
        cxt.Push(p11);
        cxt.Push(p12);
        cxt.Push(p13);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10, typename T11, typename T12, typename T13, typename T14>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10, T11 p11, T12 p12, T13 p13, T14 p14)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);
        cxt.Push(p11);
        cxt.Push(p12);
        cxt.Push(p13);
        cxt.Push(p14);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10, typename T11, typename T12, typename T13, typename T14,
                typename T15>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10, T11 p11, T12 p12, T13 p13, T14 p14, T15 p15)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);
        cxt.Push(p11);
        cxt.Push(p12);
        cxt.Push(p13);
        cxt.Push(p14);
        cxt.Push(p15);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }


    template <u32 Hash, typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10, typename T11, typename T12, typename T13, typename T14,
                typename T15, typename T16>
    static inline R Invoke(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10, T11 p11, T12 p12, T13 p13, T14 p14, T15 p15, T16 p16)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);
        cxt.Push(p11);
        cxt.Push(p12);
        cxt.Push(p13);
        cxt.Push(p14);
        cxt.Push(p15);
        cxt.Push(p16);

#ifdef USE_SPEEDY_INVOKE
        SpeedyInvoke(&cxt, Hash);
#else
        Invoke(&cxt, Hash);
#endif
        return cxt.GetResult<R>();
    }

    //////////////////////////////////////////////////////////////////////////
    // Here be the old slow name-based invokes
    
    template <typename R>
    static inline R Invoke(ch *name)
    {
        NativeContext cxt;

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1>
    static inline R Invoke(ch *name, T1 p1)
    {
        NativeContext cxt;

        cxt.Push(p1);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2>
    static inline R Invoke(ch *name, T1 p1, T2 p2)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10, typename T11>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10, T11 p11)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);
        cxt.Push(p11);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10, typename T11, typename T12>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10, T11 p11, T12 p12)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);
        cxt.Push(p11);
        cxt.Push(p12);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10, typename T11, typename T12, typename T13>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10, T11 p11, T12 p12, T13 p13)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);
        cxt.Push(p11);
        cxt.Push(p12);
        cxt.Push(p13);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10, typename T11, typename T12, typename T13, typename T14>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10, T11 p11, T12 p12, T13 p13, T14 p14)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);
        cxt.Push(p11);
        cxt.Push(p12);
        cxt.Push(p13);
        cxt.Push(p14);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }

    template <typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10, typename T11, typename T12, typename T13, typename T14,
                typename T15>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10, T11 p11, T12 p12, T13 p13, T14 p14, T15 p15)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);
        cxt.Push(p11);
        cxt.Push(p12);
        cxt.Push(p13);
        cxt.Push(p14);
        cxt.Push(p15);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }


    template <typename R, typename T1, typename T2, typename T3, typename T4,
                typename T5, typename T6, typename T7, typename T8, typename T9,
                typename T10, typename T11, typename T12, typename T13, typename T14,
                typename T15, typename T16>
    static inline R Invoke(ch *name, T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8,
                        T9 p9, T10 p10, T11 p11, T12 p12, T13 p13, T14 p14, T15 p15, T16 p16)
    {
        NativeContext cxt;

        cxt.Push(p1);
        cxt.Push(p2);
        cxt.Push(p3);
        cxt.Push(p4);
        cxt.Push(p5);
        cxt.Push(p6);
        cxt.Push(p7);
        cxt.Push(p8);
        cxt.Push(p9);
        cxt.Push(p10);
        cxt.Push(p11);
        cxt.Push(p12);
        cxt.Push(p13);
        cxt.Push(p14);
        cxt.Push(p15);
        cxt.Push(p16);

        Invoke(&cxt, name);
        return cxt.GetResult<R>();
    }
};
