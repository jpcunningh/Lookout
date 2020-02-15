#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>

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

#include "encryptor.hpp"

#include "transmitterWorker.hpp"

using namespace TransmitterWorker;

Encryptor::Encryptor(std::vector<unsigned char> challenge, std::string serial)
{
    std::string keyStr = "00" + serial + "00" + serial;

    strncpy((char *)key, keyStr.c_str(), sizeof(key));

    strncpy((char *)plain, (char *)challenge.data(), 8);
    strncpy((char *)plain + 8, (char *)challenge.data(), 8);
}

std::string Encryptor::calculateHash()
{

    ECB_Mode< AES >::Encryption e;
    e.SetKey(key, sizeof(key));

    StringSource(plain,  sizeof(plain), true,
        new StreamTransformationFilter(e,
            new StringSink(cipher)
        ) // StreamTransformationFilter
    ); // StringSource

    return cipher;
}

