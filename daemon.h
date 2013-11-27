#ifndef __DAEMON_H_
#define __DAEMON_H_

#include <iostream>
#include <set>
#include <uv++.h>

using namespace std;
using namespace Uv;

class Daemon: private Signal::SignalHandler,
              private Handle::WeakRef
{
private:
    enum Flags
    {
        None        = 0,
        Initialized = 1 << 0,
    };

public:
    Daemon(): m_flags(None)
    {
    }

    virtual ~Daemon()
    {
        CloseHandles();
    }

    int Init();

    bool IsInited()
    {
        return !! (Initialized & m_flags);
    }

    int Run()
    {
        return Loop::Run();
    }

    void Stop()
    {
        Loop::Get().Stop();
    }

    void MonitorHandle(/* [in] */ Handle *handle);

    void UnmonitorHandle(/* [in] */ Handle *handle);

private:
    virtual int DoInit() = 0;

    void CloseHandles();

    int MonitorSignal(/* [in] */ int signum);

    void OnSignal(/* [in] */ Signal *source, /* [in] */ int signum)
    {
        Stop();
    }

    void OnClose(/* [in] */ Handle *source)
    {
        UnmonitorHandle(source);
    }

private:
    unsigned int m_flags;

    set<Handle *> handles;
};

#endif /* __DAEMON_H_ */
