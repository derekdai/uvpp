#include "uv++.h"

namespace Uv
{
    Signal * Signal::New(Loop &loop)
    {
        Signal *self = new Signal();
        if(! self) {
            return NULL;
        }

        if(! self->Open(loop)) {
            return self;
        }

        delete self;

        return NULL;
    }

    int Signal::Start(/* [in] */ int signum,
                      /* [in] */ SignalHandler &handler)
    {
        assert(IsOpened());
        assert(! m_pHandler);

        int result = uv_signal_start(*this, OnSignal, signum);
        if(! result) {
            Ref();
            m_pHandler = &handler;
        }

        return result;
    }

    int Signal::Stop()
    {
        assert(IsOpened() || IsClosing());
        assert(IsStarted());

        int result = uv_signal_stop(*this);

        m_pHandler = NULL;
        Unref();

        return result;
    }

    void Signal::OnSignal(/* [in] */ uv_signal_t *peer,
                          /* [in] */ int signum)
    {
        Signal *self = (Signal *) peer->data;
        self->m_pHandler->OnSignal(self, signum);
    }

    size_t Signal::GetPeerSize() const
    {
        return sizeof(uv_signal_t);
    }

    int Signal::DoOpen(/* [in] */ Loop &loop, /* [in] */ uv_handle_t *peer)
    {
        return uv_signal_init(loop, (uv_signal_t *) peer);
    }

    void Signal::DoClose()
    {
        if(IsStarted()) {
            Stop();
        }
    }
}
