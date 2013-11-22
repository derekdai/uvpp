#include "uv++.h"

namespace Uv
{
    int Tcp::Accept(/* [out] */ Stream **conn)
    {
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
}
