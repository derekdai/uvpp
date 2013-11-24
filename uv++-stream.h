namespace Uv
{
    class Socket: public Handle
    {
    protected:
        static void OnAlloc(/* [in] */ uv_handle_t *handle,
                            /* [in] */ size_t suggested_size,
                            /* [in] */ uv_buf_t *buf)
        {
            *buf = uv_buf_init(Buffer::Alloc(suggested_size), suggested_size);
        }
    };

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

        class ReadHandler
        {
        private:
            virtual void OnRead(/* [in] */ Stream *source,
                                /* [in] */ Buffer *buffer,
                                /* [in] */ int status) = 0;
            friend class Stream;
        };

        class WriteHandler
        {
        private:
            virtual void OnWrite(/* [in] */ Stream *source,
                                 /* [in] */ int status) = 0;
            friend class Stream;
        };

    public:
        int Listen(/* [in] */ InConnectHandler &handler,
                   /* [in] */ int backlog = 10);

        bool IsListenStarted()
        {
            return !!m_pInConnectHandler;
        }

        int ReadStart(ReadHandler &handler);

        bool IsReadStarted()
        {
            return !!m_pReadHandler;
        }

        int ReadStop();

        int Write(/* [in] */ Buffer *buffer,
                  /* [in] */ WriteHandler *handler = NULL);

        virtual int Accept(/* [out] */ Stream **conn) = 0;

        virtual ~Stream()
        {
            count --;

            cout << "~Stream()" << endl;
        }

        Stream * Ref()
        {
            return (Stream *) Handle::Ref();
        }

        static int count;

    protected:
        Stream(): m_pInConnectHandler(NULL),
                  m_pReadHandler(NULL),
                  m_pWriteHandler(NULL),
                  m_pWriteBuffer(NULL)
        {
            m_writeReq.data = this;

            count ++;
        }

        virtual void DoClose();

    private:
        static void OnConnect(/* [in] */ uv_stream_t *peer,
                              /* [in] */ int status);

        static void OnRead(/* [in] */ uv_stream_t *peer,
                           /* [in] */ ssize_t nread,
                           /* [in] */ const uv_buf_t *buf);

        static void OnWrite(/* [in] */ uv_write_t *req,
                            /* [in] */ int status);

    protected:
        operator uv_stream_t *()
        {
            return (uv_stream_t *) GetPeer();
        }

    private:
        InConnectHandler *m_pInConnectHandler;

        ReadHandler *m_pReadHandler;

        uv_write_t m_writeReq;
        WriteHandler *m_pWriteHandler;
        Buffer *m_pWriteBuffer;
    };
}
