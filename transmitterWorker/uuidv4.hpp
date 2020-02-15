#ifndef UUIDV4_HPP
#define UUIDV4_HPP

#include <sstream>
#include <random>
#include <string>
#include <vector>

class uuidv4
{
  private:
    std::vector<unsigned char> uuid;

    unsigned int random_char();

    std::string generate_hex(std::vector<unsigned char> chars);

  public:
    uuidv4();
    std::vector<unsigned char> generate();
    std::string get_hex();
};

#endif

