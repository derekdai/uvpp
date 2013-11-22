#include "uv++.h"

namespace Uv
{
    int Timer::Start(/* [in] */ TimeoutHandler &handler,
                     /* [in] */ uint64_t interval,
                     /* [in] */ uint64_t delay)
    {
        assert(! m_pHandler);

        Ref();

        m_pHandler = &handler;
        return uv_timer_start(GetPeer(),
                              OnTimeout,
                              delay,
                              interval);
    }

    int Timer::Stop()
    {
        assert(m_pHandler);

        int result = uv_timer_stop(GetPeer());
        m_pHandler = NULL;

        Unref();

        return result;
    }

    void Timer::OnTimeout(uv_timer_t *peer, int status)
    {
        Timer *self = (Timer *) peer->data;
        self->m_pHandler->OnTimeout(self, status);
        if(status) {
            self->Stop();
            self->Close();
        }
    }
}
