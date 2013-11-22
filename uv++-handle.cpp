#include "uv++.h"

namespace Uv
{
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
        assert(IsOpened());

        DoClose();
        uv_close(m_pPeer, OnClose);

        // wait for pendding close request
        Loop::Get().Run(Loop::Once);

        cout << "Handle::Close()" << endl;
    }

    void Handle::OnClose(uv_handle_t *peer)
    {
        Handle *self = (Handle *) peer->data;
        if(self->m_pWeakRef) {
            self->m_pWeakRef->OnClose(self);
        }

        free(self->m_pPeer);
        self->m_pPeer = NULL;

        cout << "Handle::OnClose()" << endl;
    }

    void Handle::Unref()
    {
        assert(0 < m_refCount);

        m_refCount --;
        cout << "Refcount: " << m_refCount << endl;
        if(m_refCount) {
            return;
        }

        delete this;
    }
}
