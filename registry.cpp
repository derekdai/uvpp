#include <uv++.h>
#include <iostream>

#include "daemon.h"
#include "config.h"

using namespace std;
using namespace Uv;

class Registry: public Daemon,
                public Udp::RecvHandler,
                public Stream::InConnectHandler
{
public:
    Registry(): m_pUdp(NULL), m_pTcp(NULL)
    {
    }

    virtual ~Registry()
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
        Buffer *buf = new Buffer(REQ_DEV_LOOKUP);
        m_pUdp->Send(*buf, Ip4Address(DEV_UDP_MCAST_GROUP, DEV_UDP_PORT));
        buf->Unref();
    }

    virtual int DoInit()
    {
        m_pUdp = Udp::New();
        if(! m_pUdp) {
            return UV_ENOMEM;
        }

        int result = m_pUdp->Bind(Ip4Address("0.0.0.0", REG_UDP_PORT));
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

        result = m_pUdp->JoinMulticastGroup(REG_UDP_MCAST_GROUP,
                                            "0.0.0.0");
        if(result) {
            goto unrefUdp;
        }

        m_pTcp = Tcp::New();
        if(! m_pTcp) {
            goto stopUdpRecv;
        }

        result = m_pTcp->Bind(Ip4Address("0.0.0.0", REG_TCP_PORT));
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

    void OnDevLookupAck(/* [in] */ Udp *source,
                        /* [in] */ Buffer *buf,
                        /* [in] */ const Address &addr)
    {
        cout << "found device @" << addr.ToString() << endl;
    }

    void OnRegLookupReq(/* [in] */ Udp *source,
                        /* [in] */ Buffer *buf,
                        /* [in] */ const Address &addr)
    {
        OnDevLookupAck(source, buf, addr);

        buf = new Buffer(ACK_REG_LOOKUP, STRLEN(ACK_REG_LOOKUP));
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

        if(! strncmp(REQ_REG_LOOKUP, *buf, STRLEN(REQ_REG_LOOKUP))) {
            OnRegLookupReq(source, buf, addr);
        }
        else if(! strncmp(ACK_DEV_LOOKUP, *buf, STRLEN(ACK_DEV_LOOKUP))) {
            OnDevLookupAck(source, buf, addr);
        }
    }

private:
    Udp *m_pUdp;

    Tcp *m_pTcp;
};

int main()
{
    Registry *registry = new Registry();
    int result = registry->Init();
    if(result) {
        cout << "failed to initialize registry: " << GetErrorStr(result) << endl;
        return result;
    }
    registry->Run();
    delete registry;

    cout << Handle::count << " handles alive" << endl;

    return 0;
}
