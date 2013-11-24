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

    int Stream::Write(/* [in] */ Buffer *buffer,
                      /* [in] */ WriteHandler *handler)
    {
        assert(buffer);
        assert(IsOpened());
        assert(! m_pWriteBuffer);

        int result = uv_write(&m_writeReq, *this, *buffer, 1, OnWrite);
        if(! result) {
            Ref();

            m_pWriteBuffer = buffer->Ref();
            m_pWriteBuffer->Lock();

            m_pWriteHandler = handler;
        }

        return result;
    }

    void Stream::OnWrite(/* [in] */ uv_write_t *req,
                         /* [in] */ int status)
    {
        Stream *self = (Stream *) req->data;
        if(self->m_pWriteHandler) {
            self->m_pWriteHandler->OnWrite(self, status);
            self->m_pWriteHandler = NULL;
        }

        self->m_pWriteBuffer->Unlock();
        self->m_pWriteBuffer->Unref();
        self->m_pWriteBuffer = NULL;

        if(status) {
            self->Close();
        }

        self->Unref();
    }

    int Stream::ReadStart(ReadHandler &handler)
    {
        assert(IsOpened());
        assert(! m_pReadHandler);

        int result = uv_read_start(*this, OnAlloc, OnRead);
        if(! result) {
            Ref();
            m_pReadHandler = &handler;
        }

        return result;
    }

    int Stream::ReadStop()
    {
        assert(IsOpened() || IsClosing());
        assert(m_pReadHandler);

        int result = uv_read_stop(*this);

        m_pReadHandler = NULL;
        Unref();

        return result;
    }

    void Stream::OnRead(/* [in] */ uv_stream_t *peer,
                        /* [in] */ ssize_t nread,
                        /* [in] */ const uv_buf_t *buf)
    {
        Stream *self = (Stream *) peer->data;
        Buffer *buffer = NULL;
        if(nread > 0) {
            buffer = new Buffer(buf, nread);
        }
        self->m_pReadHandler->OnRead(self,
                                     buffer,
                                     nread < 0 ? nread : 0);

        if(buffer) {
            buffer->Unref();
        }

        if(nread < 0) {
            self->ReadStop();
            self->Close();
        }
    }

    void Stream::DoClose()
    {
        cout << "Stream::DoClose()" << endl;

        if(IsReadStarted()) {
            ReadStop();
        }

        if(IsListenStarted()) {
            m_pInConnectHandler = NULL;
            Unref();
        }
    }
}
