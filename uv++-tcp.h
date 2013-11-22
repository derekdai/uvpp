#include "uv++.h"

namespace Uv
{
    class Tcp: public Stream
    {
    public:
        class InConnectHandler
        {
        public:
            virtual void OnConnect(/* [in] */ Tcp *source,
                                   /* [in] */ Tcp *conn,
                                   /* [in] */ int status) = 0;
        };

        class OutConnectHandler
        {
        public:
            virtual void OnConnected(/* [in] */ Tcp *source,
                                     /* [in] */ int status) = 0;
        };

    public:
        Tcp()
        {
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
            assert(Address::Type_Unknown != addr.GetType());

            return uv_tcp_bind(*this, addr);
        }

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
        //void OnConnect(uv_stream_t *peer, )
        //{
        //}

    private:
    };
}
