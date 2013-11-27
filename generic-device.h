#ifndef __GENERIC_DEVICE_H_
#define __GENERIC_DEVICE_H

#include <uv++.h>
#include <list>
#include "daemon.h"
#include "config.h"

using namespace Uv;

class GenericDevice: public Daemon
{
public:
    GenericDevice(): m_pUdp(NULL), m_pTcp(NULL), m_pTimer(NULL)
    {
    }

    virtual ~GenericDevice()
    {
        if(m_pUdp) {
            m_pUdp->Unref();
        }

        if(m_pTcp) {
            m_pTcp->Unref();
        }

        if(m_pTimer) {
            m_pTimer->Unref();
        }
    }

protected:
    int PingRegistry(/* [in] */ const Address &regAddr)
    {
        Buffer *buf = new Buffer(REQ_PING);
        int result = m_pUdp->Send(*buf,
                                  regAddr);
        buf->Unref();

        return result;
    }

    int AckPingRegistry(/* [in] */ const Address &regAddr)
    {
        Buffer *buf = new Buffer(ACK_PING);
        int result = m_pUdp->Send(*buf,
                                  regAddr);
        buf->Unref();

        return result;
    }

    int LookupDevices()
    {
        Buffer *buf = new Buffer(REQ_DEV_LOOKUP);
        int result = m_pUdp->Send(*buf,
                                  Ip4Address(DEV_UDP_MCAST_GROUP,
                                             DEV_UDP_PORT));
        buf->Unref();

        return result;
    }

    int LookupRegistry(Udp::SendHandler *sendHandler = NULL)
    {
        Buffer *buf = new Buffer(REQ_REG_LOOKUP);
        int result = m_pUdp->Send(*buf,
                                  Ip4Address(REG_UDP_MCAST_GROUP,
                                             REG_UDP_PORT),
                                  sendHandler);
        buf->Unref();

        return result;
    }

    int Listen(/* [in] */ const Address &bindAddr,
               /* [in] */ Tcp::InConnectHandler &handler)
    {
        assert(! m_pTcp);

        m_pTcp = Tcp::New();
        if(! m_pTcp) {
            return UV_ENOMEM;
        }

        int result = m_pTcp->Bind(bindAddr);
        if(result) {
            goto unrefTcp;
        }

        result = m_pTcp->Listen(handler);
        if(result) {
            goto unrefTcp;
        }

        MonitorHandle(m_pTcp);

        goto end;

    unrefTcp:
        m_pTcp->Unref();
        m_pTcp = NULL;
    end:
        return result;
    }

    void ListenStop()
    {
        assert(m_pTcp);

        m_pTcp->Unref();
        m_pTcp = NULL;
    }

    int JoinMulticastGroup(/* [in] */ const Address &bindAddr,
                           /* [in] */ const char *multicastGroup,
                           /* [in] */ unsigned char multicastTtl,
                           /* [in] */ Udp::RecvHandler &handler)
    {
        assert(! m_pUdp);

        m_pUdp = Udp::New();
        if(! m_pUdp) {
            return UV_ENOMEM;
        }

        int result = m_pUdp->Bind(bindAddr);
        if(result) {
            goto unrefUdp;
        }

        result = m_pUdp->RecvStart(handler);
        if(result) {
            goto unrefUdp;
        }

        result = m_pUdp->SetMulticastTtl(multicastTtl);
        if(result) {
            goto stopRecv;
        }

        //result = m_pUdp->SetMulticastLoop(false);
        //if(result) {
            //goto stopRecv;
        //}

        result = m_pUdp->JoinMulticastGroup(multicastGroup, "0.0.0.0");
        if(result) {
            goto stopRecv;
        }

        MonitorHandle(m_pUdp);

        goto end;

    stopRecv:
        m_pUdp->RecvStop();
    unrefUdp:
        m_pUdp->Unref();
        m_pUdp = NULL;
    end:
        return result;
    }

    void LeaveMulticastGroup()
    {
        assert(m_pUdp);

        m_pUdp->Unref();
        m_pUdp = NULL;
    }

    virtual int DoInit()
    {
        m_pTimer = Timer::New();
        if(! m_pTimer) {
            return UV_ENOMEM;
        }

        MonitorHandle(m_pTimer);

        return 0;
    }

    int StartTimer(uint64_t interval, Timer::TimeoutHandler &handler)
    {
        assert(m_pTimer);

        if(IsTimerStarted()) {
            return 0;
        }

        return m_pTimer->Start(interval, handler);
    }

    bool IsTimerStarted()
    {
        assert(m_pTimer);

        return m_pTimer->IsStarted();
    }

    void StopTimer()
    {
        assert(m_pTimer);

        if(! IsTimerStarted()) {
            return;
        }

        m_pTimer->Stop();
    }

protected:
    Udp *m_pUdp;

    Tcp *m_pTcp;

    Timer *m_pTimer;
};

#endif /* __GENERIC_DEVICE_H_ */
