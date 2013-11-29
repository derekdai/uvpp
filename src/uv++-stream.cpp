#include "uv++.h"

namespace Uv
{
    int Stream::count = 0;

    int Stream::Listen(/* [in] */ InConnectHandler &handler,
                       /* [in] */ int backlog)
    {
        assert(IsOpened());
        assert(! m_pInConnectHandler);

        int result = uv_listen(*this, backlog, OnConnect);
        if(! result) {
            m_pInConnectHandler = &handler;
            Ref();
        }

        return result;
    }

    void Stream::OnConnect(/* [in] */ uv_stream_t *peer,
                           /* [in] */ int status)
    {
        Stream *self = (Stream *) peer->data;
        Stream *conn = NULL;
        if(! status) {
            status = self->Accept(&conn);
        }
        self->m_pInConnectHandler->OnConnect(self, conn, status);

        conn->Unref();
    }

    int Stream::Send(/* [in] */ Buffer *buffer,
                      /* [in] */ SendHandler *handler)
    {
        assert(buffer);
        assert(IsOpened());
        assert(! m_pSendBuffer);

        int result = uv_write(&m_writeReq, *this, *buffer, 1, OnSend);
        if(! result) {
            Ref();

            m_pSendBuffer = buffer->Ref();
            m_pSendBuffer->Lock();

            m_pSendHandler = handler;
        }

        return result;
    }

    void Stream::OnSend(/* [in] */ uv_write_t *req,
                         /* [in] */ int status)
    {
        Stream *self = (Stream *) req->data;
        if(self->m_pSendHandler) {
            self->m_pSendHandler->OnSend(self, status);
            self->m_pSendHandler = NULL;
        }

        self->m_pSendBuffer->Unlock();
        self->m_pSendBuffer->Unref();
        self->m_pSendBuffer = NULL;

        if(status) {
            self->Close();
        }

        self->Unref();
    }

    int Stream::RecvStart(RecvHandler &handler)
    {
        assert(IsOpened());
        assert(! m_pRecvHandler);

        int result = uv_read_start(*this, OnAlloc, OnRecv);
        if(! result) {
            Ref();
            m_pRecvHandler = &handler;
        }

        return result;
    }

    int Stream::RecvStop()
    {
        assert(IsOpened() || IsClosing());
        assert(m_pRecvHandler);

        int result = uv_read_stop(*this);

        m_pRecvHandler = NULL;
        Unref();

        return result;
    }

    void Stream::OnRecv(/* [in] */ uv_stream_t *peer,
                        /* [in] */ ssize_t nread,
                        /* [in] */ const uv_buf_t *buf)
    {
        Stream *self = (Stream *) peer->data;
        Buffer *buffer = NULL;
        if(nread > 0) {
            buffer = new Buffer(buf, nread);
        }
        self->m_pRecvHandler->OnRecv(self,
                                     buffer,
                                     nread < 0 ? nread : 0);

        if(buffer) {
            buffer->Unref();
        }

        if(nread < 0) {
            self->RecvStop();
            self->Close();
        }
    }

    void Stream::DoClose()
    {
        if(IsRecvStarted()) {
            RecvStop();
        }

        if(IsListenStarted()) {
            m_pInConnectHandler = NULL;
            Unref();
        }
    }
}
