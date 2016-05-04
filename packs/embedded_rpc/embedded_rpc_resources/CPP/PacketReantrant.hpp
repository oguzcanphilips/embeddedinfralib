#ifndef PACKETREANTRANT_HPP
#define PACKETREANTRANT_HPP

#include "infra\threading\public\Event.hpp"
#include "infra\threading\public\Lock.hpp"
#include "infra\threading\public\Thread.hpp"

namespace erpc
{
    template<typename BASE>
    class PacketReantrant : public BASE, public infra::Thread
    {
        infra::Lock mLock;
        infra::Event mReceive;
        infra::Event mReceiveDone;
        volatile bool mRun;
        bool mErrorOccured;
        const uint32_t mTimeout;
    public:
        PacketReantrant(uint32_t timeout)
            : BASE()
            , mRun(true)
            , mErrorOccured(false)
            , mTimeout(timeout)
        {
        }
        template<typename T1>
        PacketReantrant(uint32_t timeout, T1* t1)
            : BASE(t1)
            , mRun(true)
            , mErrorOccured(false)
            , mTimeout(timeout)
        {
        }
        template<typename T1>
        PacketReantrant(uint32_t timeout, T1& t1)
            : BASE(t1)
            , mRun(true)
            , mErrorOccured(false)
            , mTimeout(timeout)
        {
        }
        template<typename T1, typename T2>
        PacketReantrant(uint32_t timeout, T1& t1, T2& t2)
            : BASE(t1, t2)
            , mRun(true)
            , mErrorOccured(false)
            , mTimeout(timeout)
        {
        }

        virtual ~PacketReantrant()
        {
            Stop();
        }

        void Stop()
        {
            if (mRun)
            {
                mRun = false;
                Join();
            }
        }

        bool ErrorOccured() const
        {
            return mErrorOccured;
        }

        void ResetError()
        {
            mErrorOccured = false;
        }
        void FunctionLock(){ mLock.Get(); }
        void FunctionUnlock(){ mLock.Release(); }

        virtual void EventReceiveSet()  { mReceive.Set(); }
        virtual void EventReceiveReset(){ mReceive.Reset(); }
        virtual void EventReceiveWait()
        {
            if (!mReceive.Wait(mTimeout))
            {
                mErrorOccured = true;
            }
        }
        virtual void EventReceiveDoneSet()  { mReceiveDone.Set(); }
        virtual void EventReceiveDoneReset(){ mReceiveDone.Reset(); }
        virtual void EventReceiveDoneWait() { mReceiveDone.Wait(mTimeout); }

        void HandleReceiveError()
        {
            mErrorOccured = true;
        }

        void Run()
        {
            while (mRun)
            {
                BASE::ProcessReceive();
            }
        }
    };
}
#endif
