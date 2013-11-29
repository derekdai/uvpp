#include "uv++.h"

namespace Uv
{
    Udp * Udp::New(/* [in] */ Loop &loop)
    {
        Udp *self = new Udp();
        if(! self) {
            return NULL;
        }

        if(! self->Open(loop)) {
            return self;
        }

        delete self;

        return NULL;
    }

    int Udp::RecvStart(/* [in] */ RecvHandler &handler)
    {
        assert(IsOpened());
        assert(! IsRecvStarted());

        int result = uv_udp_recv_start(*this, OnAlloc, OnRecv);
        if(! result) {
            Ref();
            m_pRecvHandler = &handler;
        }

        return result;
    }

    int Udp::RecvStop()
    {
        assert(IsOpened() || IsClosing());
        assert(IsRecvStarted());

        int result = uv_udp_recv_stop(*this);

        m_pRecvHandler = NULL;
        Unref();

        return result;
    }

    size_t Udp::GetPeerSize() const
    {
        return sizeof(uv_udp_t);
    }

    int Udp::DoOpen(/* [in] */ Loop &loop, /* [in] */ uv_handle_t *peer)
    {
        return uv_udp_init(loop, (uv_udp_t *) peer);
    }

    void Udp::DoClose()
    {
        if(IsRecvStarted()) {
            RecvStop();
        }
    }

    void Udp::OnRecv(/* [in] */ uv_udp_t *peer,
                     /* [in] */ ssize_t nread, 
                     /* [in] */ const uv_buf_t *buf,
                     /* [in] */ const sockaddr *addr,
                     /* [in] */ unsigned flags)
    {
        if(! nread) {
            Buffer::Free(buf->base);
            return;
        }

        Udp *self = (Udp *) peer->data;
        Buffer *buffer = NULL;
        if(0 < nread) {
            buffer = new Buffer(buf, nread);
        }

        Address address;
        if(addr) {
            address.Set(addr);
        }

        self->m_pRecvHandler->OnRecv(self,
                                     buffer,
                                     address,
                                     flags,
                                     0 > nread ? nread : 0);

        if(buffer) {
            buffer->Unref();
        }

        if(0 > nread) {
            self->Close();
        }
    }

    int Udp::Send(/* [in] */ Buffer &buf,
                  /* [in] */ const Address &addr,
                  /* [in] */ SendHandler *handler)
    {
        assert(IsOpened());
        assert(! IsSending());

        int result = uv_udp_send(&m_sendReq,
                                 *this,
                                 buf,
                                 1,
                                 addr,
                                 OnSend);
        if(! result) {
            Ref();
            m_pSendBuffer = buf.Ref();
            m_pSendBuffer->Lock();
            m_pSendHandler = handler;
        }

        return result;
    }

    void Udp::OnSend(/* [in] */ uv_udp_send_t *req,
                     /* [in] */ int status)
    {
        Udp *self = (Udp *) req->data;
        Buffer *sendBuffer = self->m_pSendBuffer;
        self->m_pSendBuffer = NULL;

        if(self->m_pSendHandler) {
            SendHandler *sendHandler = self->m_pSendHandler;
            self->m_pSendHandler = NULL;

            sendHandler->OnSend(self, status);
        }

        sendBuffer->Unlock();
        sendBuffer->Unref();

        if(status) {
            self->Close();
        }

        self->Unref();
    }
}
