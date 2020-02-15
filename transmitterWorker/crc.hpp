#ifndef CRC_HPP
#define CRC_HPP

class CRC
{
  private:
    std::vector<unsigned char> data;

  public:
    CRC(std::vector<unsigned char> data_v);
    unsigned short crc16(int crc);
};

#endif

