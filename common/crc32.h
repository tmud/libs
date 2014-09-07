#pragma once

class CRC32
{
public:
    CRC32();
    void process(const void* buf, int len);
    unsigned int getCRC32();
private:
    unsigned int crc;
};
