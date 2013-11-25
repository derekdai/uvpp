namespace Uv
{
    class Udp: public Socket
    {
    public:
        class SendHandler
        {
        private:
            virtual void OnSend(/* [in] */ Udp *source,
                                /* [in] */ int status) = 0;
            friend class Udp;
        };

        class RecvHandler
        {
        private:
            virtual void OnRecv(/* [in] */ Udp *source,
                                /* [in] */ Buffer *buf,
                                /* [in] */ const Address &addr,
                                /* [in] */ unsigned int flags,
                                /* [in] */ int status) = 0;
            friend class Udp;
        };

    public:
        static Udp * New(/* [in] */ Loop &loop = Loop::Get());

        int Bind(/* [in] */ const Address &addr,
                 /* [in] */ unsigned int flags = 0);

        Udp * Ref()
        {
            return (Udp *) Handle::Ref();
        }

        int RecvStart(/* [in] */ RecvHandler &handler);

        bool IsRecvStarted() const
        {
            return !! m_pRecvHandler;
        }

        int RecvStop();

        int Send(/* [in] */ Buffer &buf,
                 /* [in] */ const Address &addr,
                 /* [in] */ SendHandler *handler = NULL);

        bool IsSending() const
        {
            return !! m_pSendBuffer;
        }

    protected:
        Udp(): m_pRecvHandler(NULL),
               m_pSendBuffer(NULL),
               m_pSendHandler(NULL)
        {
            m_sendReq.data = this;
        }

        virtual size_t GetPeerSize() const;

        virtual int DoOpen(/* [in] */ Loop &loop,
                           /* [in] */ uv_handle_t *peer);

        virtual void DoClose();

        operator uv_udp_t * ()
        {
            return (uv_udp_t *) GetPeer();
        }

    private:
        static void OnRecv(/* [in] */ uv_udp_t *peer,
                           /* [in] */ ssize_t nread, 
                           /* [in] */ const uv_buf_t *buf,
                           /* [in] */ const sockaddr *addr,
                           /* [in] */ unsigned flags);

        static void OnSend(/* [in] */ uv_udp_send_t *req,
                           /* [in] */ int status);

    private:
        RecvHandler *m_pRecvHandler;

        uv_udp_send_t m_sendReq;
        Buffer *m_pSendBuffer;
        SendHandler *m_pSendHandler;
    };
}
