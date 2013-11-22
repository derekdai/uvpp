#include "uv++.h"

namespace Uv
{
    int Tcp::Accept(/* [out] */ Stream **conn)
    {
        assert(IsOpened());
        assert(conn && ! * conn);

        Tcp *newConn = new Tcp();
        if(! newConn) {
            return ENOMEM;
        }

        int result = newConn->Open();
        if(result) {
            goto deleteNewConn;
        }

        result = uv_accept(*this, *newConn);
        if(result) {
            goto deleteNewConn;
        }

        *conn = newConn;

        goto end;

    deleteNewConn:
        delete newConn;
    end:
        return result;
    }

    int Tcp::Connect(/* [in] */ const Address &addr,
                     /* [in] */ OutConnectHandler *handler)
    {
        assert(IsOpened());
        assert(! m_pOutConnectHandler);

        int result = uv_tcp_connect(&m_connectReq,
                                    *this,
                                    addr,
                                    OnConnected);
        if(! result) {
            m_pOutConnectHandler = handler;
            Ref();
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
}
