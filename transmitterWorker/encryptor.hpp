#ifndef ENCRYPTOR_HPP
#define ENCRYPTOR_HPP

class Encryptor
{
  private:
    std::string key;
    std::string cipher;
    std::string plain;

  public:
    Encryptor(std::vector<unsigned char> challenge, std::string serial);
    std::string calculateHash();
};

#endif

