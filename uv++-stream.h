namespace Uv
{
    class Stream: public Socket
    {
    public:
        class InConnectHandler
        {
        private:
            virtual void OnConnect(/* [in] */ Stream *source,
                                   /* [in] */ Stream *conn,
                                   /* [in] */ int status) = 0;
            friend class Stream;
        };

        class RecvHandler
        {
        private:
            virtual void OnRecv(/* [in] */ Stream *source,
                                /* [in] */ Buffer *buffer,
                                /* [in] */ int status) = 0;
            friend class Stream;
        };

        class SendHandler
        {
        private:
            virtual void OnSend(/* [in] */ Stream *source,
                                /* [in] */ int status) = 0;
            friend class Stream;
        };

    public:
        int Listen(/* [in] */ InConnectHandler &handler,
                   /* [in] */ int backlog = 10);

        bool IsListenStarted()
        {
            return !! m_pInConnectHandler;
        }

        int RecvStart(/* [in] */ RecvHandler &handler);

        bool IsRecvStarted()
        {
            return !! m_pRecvHandler;
        }

        int RecvStop();

        int Send(/* [in] */ Buffer *buffer,
                 /* [in] */ SendHandler *handler = NULL);

        virtual int Accept(/* [out] */ Stream **conn) = 0;

        virtual ~Stream()
        {
            count --;
        }

        Stream * Ref()
        {
            return (Stream *) Handle::Ref();
        }

        static int count;

    protected:
        Stream(): m_pInConnectHandler(NULL),
                  m_pRecvHandler(NULL),
                  m_pSendHandler(NULL),
                  m_pSendBuffer(NULL)
        {
            m_writeReq.data = this;

            count ++;
        }

        virtual void DoClose();

    private:
        static void OnConnect(/* [in] */ uv_stream_t *peer,
                              /* [in] */ int status);

        static void OnRecv(/* [in] */ uv_stream_t *peer,
                           /* [in] */ ssize_t nread,
                           /* [in] */ const uv_buf_t *buf);

        static void OnSend(/* [in] */ uv_write_t *req,
                           /* [in] */ int status);

    protected:
        operator uv_stream_t *()
        {
            return (uv_stream_t *) GetPeer();
        }

    private:
        InConnectHandler *m_pInConnectHandler;

        RecvHandler *m_pRecvHandler;

        uv_write_t m_writeReq;
        SendHandler *m_pSendHandler;
        Buffer *m_pSendBuffer;
    };
}
