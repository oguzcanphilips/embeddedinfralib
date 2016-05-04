#ifndef LOCALPACKETCOMMUNICATION_HPP
#define LOCALPACKETCOMMUNICATION_HPP

#include "infra\threading\public\Thread.hpp"
#include "infra\threading\public\Lock.hpp"
#include "PacketCommunication.hpp"
#include <list>

class LocalPacketCommunication : public erpc::PacketCommunication
{
  enum
  {
    StartToken = 0x100
  };

  infra::Lock mLock;
  std::list<uint16_t> mBuffer;
  LocalPacketCommunication* mLink;
public:
  LocalPacketCommunication()
  : mLink(0)
  {
  }
  void Link(LocalPacketCommunication* comm)
  {
    comm->mLink = this;
    mLink = comm;
  }

  void ProcessReceive()
  {
    {
      infra::Lock::ScopedLock lock(mLock);
      
      if(mBuffer.empty()) return;
      uint16_t r = mBuffer.front();
      mBuffer.pop_front();
      if(r != StartToken)
      {
        return;
      }
    }
    Receive();
  }

  bool IsPacketEnded()
  {
      return true;
  }
  void PacketStart()
  {
    infra::Lock::ScopedLock lock(mLink->mLock);
    mLink->mBuffer.push_back(StartToken);
  }

  void PacketEnd()
  {
    mLink->ProcessReceive();
  }

  void WriteByte(uint8_t data)
  {
    infra::Lock::ScopedLock lock(mLink->mLock);
    mLink->mBuffer.push_back(data);
  }

  bool ReadByte(uint8_t& v)
  {
    bool dataAvailable = false;
    uint32_t cnt = 100;
    while(!dataAvailable && cnt--)
    {
      {
        infra::Lock::ScopedLock lock(mLock);
        if(!mBuffer.empty())
        {
          dataAvailable = true;
        }
      }
        if (!dataAvailable) infra::Thread::Sleep(100);
    }

    infra::Lock::ScopedLock lock(mLock);
    if(mBuffer.empty()) 
    {
      return false;
    }
    uint16_t r = mBuffer.front();
    mBuffer.pop_front();
    v = r & 0xff;
    return (r == v);
  }
};

#endif