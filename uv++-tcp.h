#include "uv++.h"

namespace Uv
{
    class Tcp: public Stream
    {
    public:
        class OutConnectHandler
        {
        private:
            virtual void OnConnected(/* [in] */ Tcp *source,
                                     /* [in] */ int status) = 0;
            friend class Tcp;
        };

    public:
        static Tcp * New(Loop &Loop = Loop::Get());

        virtual int Bind(/* [in] */ const Address &addr,
                         /* [in] */ unsigned int flags = 0);

        virtual int Accept(/* [out] */ Stream **conn);

        Tcp * Ref()
        {
            return (Tcp *) Handle::Ref();
        }

        int Connect(/* [in] */ Address::Type addrType,
                    /* [in] */ const char *ip,
                    /* [in] */ int port,
                    /* [in] */ OutConnectHandler *handler = NULL)
        {
            return Connect(Address(addrType, ip, port), handler);
        }

        int Connect(/* [in] */ const Address &addr,
                    /* [in] */ OutConnectHandler *handler);

    protected:
        Tcp(): m_pOutConnectHandler(NULL)
        {
            m_connectReq.data = this;
        }

        virtual size_t GetPeerSize() const;

        virtual int DoOpen(/* [in] */ Loop &loop,
                           /* [in] */ uv_handle_t *peer);

        virtual void DoClose();

        operator uv_tcp_t *()
        {
            return (uv_tcp_t *) GetPeer();
        }

    private:
        static void OnConnected(uv_connect_t *req, int status);

    private:
        uv_connect_t m_connectReq;
        OutConnectHandler *m_pOutConnectHandler;
    };
}
