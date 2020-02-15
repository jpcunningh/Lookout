#ifndef ENCRYPTOR_HPP
#define ENCRYPTOR_HPP

#include <crypto++/cryptlib.h>
using CryptoPP::Exception;

#include <crypto++/filters.h>
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include <crypto++/aes.h>
using CryptoPP::AES;

#include <crypto++/modes.h>
using CryptoPP::ECB_Mode;

class Encryptor
{
  private:
    unsigned short forceAlignment;
    byte key[16];
    byte plain[16];
    std::string cipher;

  public:
    Encryptor(std::vector<unsigned char> challenge, std::string serial);
    std::string calculateHash();
};

#endif

