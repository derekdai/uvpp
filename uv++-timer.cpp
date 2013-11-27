#include "uv++.h"

namespace Uv
{
    Timer * Timer::New(/* [in] */ Loop &loop)
    {
        Timer *self = new Timer();
        if(! self) {
            return NULL;
        }

        if(! self->Open(loop)) {
            return self;
        }

        delete self;

        return NULL;
    }

    int Timer::Start(/* [in] */ uint64_t interval,
                     /* [in] */ uint64_t delay,
                     /* [in] */ TimeoutHandler &handler)
    {
        assert(! m_pHandler);

        int result = uv_timer_start(*this,
                                    OnTimeout,
                                    delay,
                                    interval);
        if(! result) {
            m_pHandler = &handler;
            Ref();

        }

        return result;
    }

    int Timer::Stop()
    {
        assert(m_pHandler);

        int result = uv_timer_stop(*this);
        m_pHandler = NULL;

        Unref();

        return result;
    }

    void Timer::OnTimeout(uv_timer_t *peer, int status)
    {
        Timer *self = (Timer *) peer->data;
        self->m_pHandler->OnTimeout(self, status);

        if(status) {
            self->Close();
        }
    }

    int Timer::DoOpen(Loop &loop, uv_handle_t *peer)
    {
        return uv_timer_init(loop, (uv_timer_t *) peer);
    }

    void Timer::DoClose()
    {
        if(IsStarted()) {
            Stop();
        }
    }

    size_t Timer::GetPeerSize() const
    {
        return sizeof(uv_timer_t);
    }
}
