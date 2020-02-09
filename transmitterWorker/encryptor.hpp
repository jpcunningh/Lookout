#ifndef ENCRYPTOR_HPP
#define ENCRYPTOR_HPP

class Encryptor
{
  private:
    std::string key;
    std::string cipher;
    std::string plain;

  public:
    Encryptor(std::string plain, std::string key);
    std::string calculateHash();
};

#endif

