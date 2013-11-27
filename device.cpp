#include <uv++.h>
#include <iostream>

#include "daemon.h"
#include "config.h"

using namespace std;
using namespace Uv;

class Device: public Daemon,
                public Udp::RecvHandler,
                public Udp::SendHandler,
                public Stream::InConnectHandler
{
public:
    Device(): m_pUdp(NULL), m_pTcp(NULL)
    {
    }

    virtual ~Device()
    {
        if(m_pUdp) {
            m_pUdp->Unref();
        }

        if(m_pTcp) {
            m_pTcp->Unref();
        }
    }

private:
    void LookupDevices()
    {
        Buffer *buf = new Buffer(REQ_REG_LOOKUP);
        m_pUdp->Send(*buf, Ip4Address(REG_UDP_MCAST_GROUP, REG_UDP_PORT));
        buf->Unref();
    }

    virtual int DoInit()
    {
        m_pUdp = Udp::New();
        if(! m_pUdp) {
            return UV_ENOMEM;
        }

        int result = m_pUdp->Bind(Ip4Address("0.0.0.0", DEV_UDP_PORT));
        if(result) {
            goto unrefUdp;
        }

        result = m_pUdp->RecvStart(*this);
        if(result) {
            goto unrefUdp;
        }

        result = m_pUdp->SetMulticastTtl(UDP_MCAST_TTL);
        if(result) {
            goto unrefUdp;
        }

        result = m_pUdp->JoinMulticastGroup(DEV_UDP_MCAST_GROUP,
                                            "0.0.0.0");
        if(result) {
            goto unrefUdp;
        }

        m_pTcp = Tcp::New();
        if(! m_pTcp) {
            goto stopUdpRecv;
        }

        result = m_pTcp->Bind(Ip4Address("0.0.0.0", DEV_TCP_PORT));
        if(result) {
            goto unrefTcp;
        }

        result = m_pTcp->Listen(*this);
        if(result) {
            goto unrefTcp;
        }

        MonitorHandle(m_pUdp);
        MonitorHandle(m_pTcp);

        LookupDevices();

        goto end;

    unrefTcp:
        m_pTcp->Unref();
        m_pTcp = NULL;
    stopUdpRecv:
        m_pUdp->RecvStop();
    unrefUdp:
        m_pUdp->Unref();
        m_pUdp = NULL;
    end:
        return result;
    }

    void OnConnect(/* [in] */ Stream *source,
                   /* [in] */ Stream *conn,
                   /* [in] */ int status)
    {
        cout << "New connection accepted" << endl;

        MonitorHandle(conn);
    }

    void OnRegLookupAck(/* [in] */ Udp *source,
                        /* [in] */ Buffer *buf,
                        /* [in] */ const Address &addr)
    {
        cout << "found registry @" << addr.ToString() << endl;
    }

    void OnDevLookupReq(/* [in] */ Udp *source,
                        /* [in] */ Buffer *buf,
                        /* [in] */ const Address &addr)
    {
        OnRegLookupAck(source, buf, addr);

        buf = new Buffer(ACK_DEV_LOOKUP, STRLEN(ACK_DEV_LOOKUP));
        if(! buf) {
            cout << "out of resource" << endl;
        }

        if(source->Send(*buf, addr)) {
            source->Close();
        }
        buf->Unref();
    }

    void OnRecv(/* [in] */ Udp *source,
                /* [in] */ Buffer *buf,
                /* [in] */ const Address &addr,
                /* [in] */ unsigned int flags,
                /* [in] */ int status)
    {
        if(status) {
            return;
        }

        if(! strncmp(REQ_DEV_LOOKUP, *buf, STRLEN(REQ_DEV_LOOKUP))) {
            OnDevLookupReq(source, buf, addr);
        }
        else if(! strncmp(ACK_REG_LOOKUP, *buf, STRLEN(ACK_REG_LOOKUP))) {
            OnRegLookupAck(source, buf, addr);
        }
    }

    void OnSend(/* [in] */ Udp *source, /* [in] */ int status)
    {
    }

private:
    Udp *m_pUdp;

    Tcp *m_pTcp;
};

int main()
{
    Device *device = new Device();
    int result = device->Init();
    if(result) {
        cout << "failed to initialize device: " << GetErrorStr(result) << endl;
        return result;
    }
    device->Run();
    delete device;

    cout << Handle::count << " handles alive" << endl;

    return 0;
}
