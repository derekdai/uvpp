#include "uv++.h"

namespace Uv
{
    class Tcp: public Stream
    {
    public:
        class OutConnectHandler
        {
        public:
            virtual void OnConnected(/* [in] */ Tcp *source,
                                     /* [in] */ int status) = 0;
        };

    public:
        Tcp(): m_pOutConnectHandler(NULL)
        {
            m_connectReq.data = this;
        }

        virtual ~Tcp()
        {
            cout << "~Tcp()" << endl;
        }

        int Bind(/* [in] */ Address::Type type,
                 /* [in] */ const char *ip,
                 /* [in] */ int port)
        {
            return Bind(Address(type, ip, port));
        }

        int Bind(/* [in] */ const Address &addr)
        {
            assert(IsOpened());
            assert(Address::Type_Unknown != addr.GetType());

            return uv_tcp_bind(*this, addr);
        }

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
        virtual size_t GetPeerSize() const
        {
            return sizeof(uv_tcp_t);
        }

        virtual int DoOpen(/* [in] */ Loop &loop, /* [in] */ uv_handle_t *peer)
        {
            return uv_tcp_init(loop, (uv_tcp_t *) peer);
        }

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
