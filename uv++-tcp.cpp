#include "uv++.h"

namespace Uv
{
    Tcp * Tcp::New(Loop &loop)
    {
        Tcp *self = new Tcp();
        if(! self) {
            return NULL;
        }

        if(! self->Open(loop)) {
            return self;
        }

        delete self;

        return NULL;
    }

    int Tcp::Accept(/* [out] */ Stream **conn)
    {
        assert(IsOpened());
        assert(conn && ! * conn);

        Tcp *newConn = New(GetLoop());
        if(! newConn) {
            return UV_ENOMEM;
        }

        int result = uv_accept(*this, *newConn);
        if(! result) {
            *conn = newConn;
        }
        else {
            newConn->Unref();
        }

        return result;
    }

    int Tcp::Connect(/* [in] */ const Address &addr,
                     /* [in] */ OutConnectHandler *handler)
    {
        assert(IsOpened());

        int result = uv_tcp_connect(&m_connectReq,
                                    *this,
                                    addr,
                                    OnConnected);
        if(! result) {
            Ref();
            m_pOutConnectHandler = handler;
        }

        return result;
    }

    void Tcp::OnConnected(uv_connect_t *req, int status)
    {
        Tcp *self = (Tcp *) req->data;
        if(self->m_pOutConnectHandler) {
            self->m_pOutConnectHandler->OnConnected(self, status);
        }

        if(status) {
            self->Close();
        }

        self->Unref();
    }
    Tcp::~Tcp()
    {
        cout << "~Tcp()" << endl;
    }

    size_t Tcp::GetPeerSize() const
    {
        return sizeof(uv_tcp_t);
    }

    int Tcp::DoOpen(/* [in] */ Loop &loop, /* [in] */ uv_handle_t *peer)
    {
        return uv_tcp_init(loop, (uv_tcp_t *) peer);
    }

    void Tcp::DoClose()
    {
        cout << "Tcp::DoClose()" << endl;

        Stream::DoClose();
    }
}