namespace Uv
{
    class Signal: public Handle
    {
    public:
        class SignalHandler
        {
        private:
            virtual void OnSignal(/* [in] */ Signal *source,
                                  /* [in] */ int signum) = 0;

            friend class Signal;
        };

    public:
        int Start(int signum, SignalHandler &handler);

        int Stop();

        static Signal * New(Loop &loop = Loop::Get());

        bool IsStarted()
        {
            return !!m_pHandler;
        }

    protected:
        Signal(): m_pHandler(NULL)
        {
        }

        virtual size_t GetPeerSize() const;

        virtual int DoOpen(/* [in] */ Loop &loop,
                           /* [in] */ uv_handle_t *peer);

        virtual void DoClose();

        operator uv_signal_t * ()
        {
            return (uv_signal_t *) GetPeer();
        }

        static void OnSignal(/* [in] */ uv_signal_t *peer,
                             /* [in] */ int signum);

    private:
        SignalHandler *m_pHandler;
    };
}
