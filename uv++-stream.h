namespace Uv
{
    class Stream: public Handle
    {
    public:
        class InConnectHandler
        {
        public:
            virtual void OnConnect(/* [in] */ Stream *source,
                                   /* [in] */ Stream *conn,
                                   /* [in] */ int status) = 0;
        };

    public:
        int Listen(/* [in] */ InConnectHandler &handler,
                   /* [in] */ int backlog = 10)
        {
            assert(! m_pInConnectHandler);

            m_pInConnectHandler = &handler;

            return uv_listen(*this, backlog, OnConnect);
        }

        virtual int Accept(/* [out] */ Stream **conn) = 0;

        virtual ~Stream()
        {
            cout << "~Stream()" << endl;
        }

        Stream * Ref()
        {
            return (Stream *) Handle::Ref();
        }

    protected:
        Stream(): m_pInConnectHandler(NULL)
        {
        }

    private:
        static void OnConnect(/* [in] */ uv_stream_t *peer,
                              /* [in] */ int status);

    protected:
        operator uv_stream_t *()
        {
            return (uv_stream_t *) GetPeer();
        }

    private:
        InConnectHandler *m_pInConnectHandler;
    };
}
