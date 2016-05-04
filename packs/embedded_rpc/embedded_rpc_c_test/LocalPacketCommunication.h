#ifndef LOCALPACKETCOMMUNICATION_H
#define LOCALPACKETCOMMUNICATION_H

#include "Thread.h"
#include "Lock.h"
#include "PacketCommunication.h"
#include <list>

class LocalPacketCommunication : public PacketCommunication
{
  enum
  {
    StartToken = 0x100
  };

  Lock mLock;
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
      Lock::ScopedLock lock(mLock);
      
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

  void PacketStart()
  {
    Lock::ScopedLock lock(mLink->mLock);
    mLink->mBuffer.push_back(StartToken);
  }

  void PacketEnd()
  {
    mLink->ProcessReceive();
  }

  void Write(const uint8_t* data, uint16_t len)
  {
    Lock::ScopedLock lock(mLink->mLock);
    while(len--)
    {
      mLink->mBuffer.push_back(*data++);
    }
  }

  bool Read(uint8_t& v)
  {
    Lock::ScopedLock lock(mLock);
    if(mBuffer.empty()) return false;
    uint16_t r = mBuffer.front();
    mBuffer.pop_front();
    v = r & 0xff;
    return (r == v);
  }
};

#endif