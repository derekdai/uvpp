#include "uv++.h"

namespace Uv
{
    int Stream::Listen(/* [in] */ InConnectHandler &handler,
                       /* [in] */ int backlog)
    {
        assert(IsOpened());
        assert(! m_pInConnectHandler);

        int result = uv_listen(*this, backlog, OnConnect);
        if(! result) {
            m_pInConnectHandler = &handler;
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
}
