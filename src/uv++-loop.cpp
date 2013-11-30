#include "uv++.h"
#include <cassert>

namespace Uv {
    __thread Loop *Uv::Loop::s_pLoop = NULL;

    Loop & Loop::Get()
    {
        if(! s_pLoop) {
            s_pLoop = new Loop();
        }

        return *s_pLoop;
    }

    int Loop::Run()
    {
        return Get().Run(Default);
    }

    void Loop::Free()
    {
        if(! s_pLoop) {
            return;
        }

        assert(! s_pLoop->IsRunning());

        delete s_pLoop;
        s_pLoop = NULL;
    }

    int Loop::Run(/* [in] */ RunMode mode)
    {
        m_runLevel ++;
        int result = uv_run(m_pPeer, (uv_run_mode) mode);
        m_runLevel --;

        return result;
    }
}
