#pragma once
#include "Generated\CPP\ITest.hpp"

class TestImpl : public erpc::ITest
{
public:
  erpc::Array mData;
  uint16_t mValue;
  void DoThis(const erpc::Array& array)
  {
    mData = array;
  }
  uint16_t DoubleOutput(uint16_t i)
  {
      return i*2;
  }
  bool IO_In(uint16_t value)
  {
      mValue = value;
      return true;
  }
  bool IO_Out(uint16_t& value)
  {
      value = mValue;
      return true;
  }
  void IO_InOut(uint16_t& value)
  {
      uint16_t temp = mValue;
      mValue = value;
      value = temp;
  }
};