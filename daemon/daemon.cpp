#include "daemon.h"

int Daemon::Init()
{
    assert(! IsInited());

    int result = MonitorSignal(SIGINT);
    if(result) {
        return result;
    }

    result = MonitorSignal(SIGTERM);
    if(result) {
        return result;
    }

    result = DoInit();
    if(! result) {
        m_flags |= Initialized;
    }

    return result;
}

void Daemon::MonitorHandle(/* [in] */ Handle *handle)
{
    //cout << "Monitoring " << handle->GetTypeName() << " (" << handle << ")" << endl;
    handle->SetWeakRef(this);
    handles.insert(handle);
}

void Daemon::UnmonitorHandle(/* [in] */ Handle *handle)
{
    //cout << "Unmonitoring " << handle->GetTypeName() << " (" << handle << ")" << endl;
    set<Handle *>::iterator iter = handles.find(handle);
    handles.erase(iter);
}

void Daemon::CloseHandles()
{
    set<Handle *>::iterator curr = handles.begin();
    set<Handle *>::iterator end = handles.end();
    for(; curr != end; ++ curr) {
        //cout << "Closing " << (* curr)->GetTypeName() << " (" << (* curr) << ")" << endl;
        (* curr)->SetWeakRef(NULL);
        (* curr)->Close();
    }

    Loop::Get().Run(Loop::Once);
}

int Daemon::MonitorSignal(/* [in] */ int signum)
{
    Signal *sig = Signal::New();
    if(! sig) {
        return UV_ENOMEM;
    }

    int result = sig->Start(signum, *this);
    MonitorHandle(sig);
    sig->Unref();

    return result;
}

