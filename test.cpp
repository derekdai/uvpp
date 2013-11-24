#include "uv++.h"
#include <set>
#include <iostream>

using namespace Uv;
using namespace std;

class MyTimeoutHandler: public Timer::TimeoutHandler,
                        public Handle::WeakRef
{
public:
    MyTimeoutHandler(): count(3) {}

    void AddHandle(Handle *handle)
    {
        handles.insert(handle);
        handle->SetWeakRef(this);
    }

private:
    void OnTimeout(Timer *source, int status)
    {
        if(status) {
            cout << GetErrorStr(status) << endl;
            return;
        }

        count --;
        if(0 >= count) {
            source->Stop();

            set<Handle *>::iterator iter = handles.begin();
            set<Handle *>::iterator end = handles.end();
            for(; iter != end; ++iter) {
                (*iter)->SetWeakRef(NULL);
                (*iter)->Close();
            }
        }
    }

private:
    void OnClose(Handle *handle)
    {
        set<Handle *>::iterator iter = handles.find(handle);
        if(handles.end() == iter) {
            return;
        }

        cout << "Unwatching " << *iter << endl;
        handles.erase(iter);
    }

private:
    int count;

    set<Handle *> handles;
};

MyTimeoutHandler timeoutHandler;

class Server: public Stream::InConnectHandler,
                     Stream::ReadHandler,
                     Stream::WriteHandler
{
private:
    virtual void OnConnect(Stream *server,
                           Stream *conn,
                           int status)
    {
        cout << "New connection (" << conn << ")" << endl;

        timeoutHandler.AddHandle(conn);
        conn->ReadStart(*this);
    }

    virtual void OnRead(Stream *conn, Buffer *buf, int status)
    {
        cout << "Server Read: " << string(buf->GetBase(), buf->GetSize()) << endl;

        buf = new Buffer("World\n");
        conn->Write(buf, this);
        buf->Unref();

        conn->ReadStop();
    }

    virtual void OnWrite(Stream *conn, int status)
    {
        cout << "Server::OnWrite()" << endl;
        conn->Close();

        cout << "Disconnected (" << conn << ")" << endl;
    }
};

class Client: public Tcp::OutConnectHandler,
                     Stream::WriteHandler,
                     Stream::ReadHandler
{
public:
    Client(): count(0)
    {
    }

private:
    void OnConnected(Tcp *conn, int status)
    {
        if(status) {
            cout << "Clinet::OnConnected: " << GetErrorStr(status) << endl;
            return;
        }

        cout << "Client Write: Hello" << endl;

        Buffer *buf = new Buffer("Hello");
        conn->Write(buf, this);
        buf->Unref();

        conn->ReadStart(*this);
    }

    void OnWrite(Stream *conn, int status)
    {
        Inc(conn);
    }

    void OnRead(Stream *conn, Buffer *buf, int status)
    {
        Inc(conn);
    }

    void Inc(Stream *conn)
    {
        count ++;
        if(2 == count) {
            conn->Unref();
            //conn->Close();
        }
    }

private:
    int count;
};

class SignalHandler: public Signal::SignalHandler
{
private:
    void OnSignal(Signal *source, int signum)
    {
        cout << "Bye" << endl;

        source->Close();
        source->GetLoop().Stop();
    }
};

int main()
{
    SignalHandler signalHandler;
    Signal *signal = Signal::New();
    timeoutHandler.AddHandle(signal);
    assert(! signal->Start(SIGINT, signalHandler));
    signal->Unref();
    signal = Signal::New();
    timeoutHandler.AddHandle(signal);
    assert(! signal->Start(SIGTERM, signalHandler));
    signal->Unref();

    Server serverEventHandler;
    Tcp *server = Tcp::New();
    timeoutHandler.AddHandle(server);
    assert(! server->Bind(Ip4Address("0.0.0.0", 1234)));
    assert(! server->Listen(serverEventHandler));
    server->Unref();

    //Client clientEventHandler;
    //Tcp *client = Tcp::New();
    //timeoutHandler.AddHandle(client);
    //assert(! client->Connect(Ip4Address("127.0.0.1", 1234), &clientEventHandler));
    //client->Unref();

    Timer *timer = Timer::New();
    assert(timer);
    assert(! timer->Start(timeoutHandler, 1000));
    timer->Unref();

    Loop::Run();

    cout << Handle::count << " alive" << endl;

    return 0;
}
