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

Encryptor::Encryptor(std::string challenge, std::string serial)
{
    key = "00" + serial + "00" + serial;
    plain = challenge;
}

std::string Encryptor::calculateHash()
{
    try
    {
        byte keyByte[sizeof(key)];
        strncpy((char *)keyByte, key.c_str(), sizeof(key));

        ECB_Mode< AES >::Encryption e;
        e.SetKey(keyByte, sizeof(keyByte));

        StringSource(plain, true,
            new StreamTransformationFilter(e,
                new StringSink(cipher)
            ) // StreamTransformationFilter
        ); // StringSource
    }
    catch(const CryptoPP::Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    return cipher;
}

