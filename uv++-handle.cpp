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
        cout << "Handle::OnClose()" << endl;

        Handle *self = (Handle *) peer->data;
        if(self->m_pWeakRef) {
            self->m_pWeakRef->OnClose(self);
        }

        free(self->m_pPeer);
        self->m_pPeer = NULL;

        self->ClearFlags(Flags_Closing);

        if(! self->m_refCount) {
            cout << "Handle::OnClose(): delete self" << endl;
            delete self;
        }
    }

    void Handle::Unref()
    {
        assert(0 < m_refCount);

        cout << "Handle(" << this << ")::Unref: " << m_refCount - 1<< endl;

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
}
