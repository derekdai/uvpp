namespace Uv
{
    class Socket: public Handle
    {
    public:
        int Bind(/* [in] */ Address::Type type,
                 /* [in] */ const char *ip,
                 /* [in] */ int port,
                 /* [in] */ unsigned int flags = 0)
        {
            return Bind(Address(type, ip, port), flags);
        }

        virtual int Bind(/* [in] */ const Address &addr,
                         /* [in] */ unsigned int flags = 0) = 0;

    protected:
        static void OnAlloc(/* [in] */ uv_handle_t *handle,
                            /* [in] */ size_t suggested_size,
                            /* [in] */ uv_buf_t *buf);
    };
}
