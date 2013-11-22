namespace Uv
{
    class Stream: public Handle
    {
    public:
        //int Listen([> [in] <] InConnectHandler *handler,
                   //[> [in] <] int backlog = 10)
        //{
            //return uv_listen(*this, backlog);
        //}

        virtual ~Stream()
        {
            cout << "~Stream()" << endl;
        }

    protected:
        operator uv_stream_t *()
        {
            return (uv_stream_t *) GetPeer();
        }
    };
}
