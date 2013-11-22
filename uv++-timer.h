namespace Uv
{
    class Timer: public Handle
    {
    public:
        class TimeoutHandler
        {
        public:
            virtual void OnTimeout(Timer *source, int status) = 0;
        };

    public:
        Timer(): m_pHandler(NULL)
        {
        }

        int Start(/* [in] */ TimeoutHandler &handler,
                  /* [in] */ uint64_t interval)
        {
            assert(IsOpened());

            return Start(handler, interval, interval);
        }

        int Start(/* [in] */ TimeoutHandler &handler,
                  /* [in] */ uint64_t interval,
                  /* [in] */ uint64_t delay);

        int Stop();

        bool IsStarted()
        {
            return NULL != m_pHandler;
        }

        virtual ~Timer()
        {
            cout << "~Timer()" << endl;
        }

        Timer * Ref()
        {
            return (Timer *) Handle::Ref();
        }

    protected:
        virtual int DoOpen(Loop &loop, uv_handle_t *peer)
        {
            return uv_timer_init(loop, (uv_timer_t *) peer);
        }

        virtual void DoClose()
        {
            if(IsStarted()) {
                Stop();
            }
        }

        virtual size_t GetPeerSize() const
        {
            return sizeof(uv_timer_t);
        }

        operator uv_timer_t *()
        {
            return (uv_timer_t *) GetPeer();
        }

        static void OnTimeout(uv_timer_t *peer, int status);

    private:
        TimeoutHandler *m_pHandler;
    };
}
