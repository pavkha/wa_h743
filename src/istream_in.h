#pragma once

#include <cstdint>

class istream_in {
  public:
    virtual ~istream_in() = default;

    virtual void receive() = 0;
    virtual int read(uint8_t* buff, uint16_t length) = 0;
};
