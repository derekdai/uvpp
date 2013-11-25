#include "uv++.h"

namespace Uv
{
    int Handle::count = 0;

    int Handle::Open(/* [in] */ Loop &loop)
    {
        assert(! IsOpened());

        size_t peerSize = GetPeerSize();
        assert(sizeof(uv_handle_t) <= peerSize);

        m_pPeer = (uv_handle_t *) malloc(peerSize);
        int result = DoOpen(loop, m_pPeer);

        if(result) {
            free(m_pPeer);
            m_pPeer = NULL;
        }

        m_pPeer->data = this;

        return result;
    }

    void Handle::Close()
    {
        if(! IsOpened()) {
            return;
        }

        SetFlags(Flags_Closing);

        DoClose();

        uv_close(m_pPeer, OnClose);
    }

    void Handle::OnClose(uv_handle_t *peer)
    {
        Handle *self = (Handle *) peer->data;
        if(self->m_pWeakRef) {
            self->m_pWeakRef->OnClose(self);
        }

        free(self->m_pPeer);
        self->m_pPeer = NULL;

        self->ClearFlags(Flags_Closing);

        if(! self->m_refCount) {
            delete self;
        }
    }

    void Handle::Unref()
    {
        assert(0 < m_refCount);

        m_refCount --;
        if(m_refCount) {
            return;
        }

        if(IsClosed()) {
            delete this;
            return;
        }

        Close();
    }

    const char * Handle::GetTypeName() const
    {
        assert(IsOpened() || IsClosing());

        switch(GetType()) {
        case UV_IDLE:
            return "Uv::Idle";
        case UV_TIMER:
            return "Uv::Timer";
        case UV_SIGNAL:
            return "Uv::Signal";
        case UV_STREAM:
            return "Uv::Stream";
        case UV_TCP:
            return "Uv::Tcp";
        case UV_UDP:
            return "Uv::Udp";
        case UV_ASYNC:
            return "Uv::Async";
        case UV_CHECK:
            return "Uv::Check";
        case UV_POLL:
            return "Uv::Poll";
        case UV_HANDLE:
            return "Uv::Handle";
        case UV_NAMED_PIPE:
            return "Uv::NamedPipe";
        case UV_PROCESS:
            return "Uv::Process";
        case UV_TTY:
            return "Uv::Tty";
        default:
            return "Unknown";
        }
    }
}
