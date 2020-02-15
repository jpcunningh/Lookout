#include <tinyb.hpp>

#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>

#include "crc.hpp"

#include "transmitterWorker.hpp"

#define poly 0x1021

using namespace TransmitterWorker;

CRC::CRC(std::vector<unsigned char> data_v)
{
    data = data_v;
}

unsigned short CRC::crc16(int crc)
{
    // Calculating XMODEM CRC-16 in 'C'
    // ================================
    // Reference model for the translated code


    // On entry, addr=>start of data
    //           num = length of data
    //           crc = incoming CRC
    int i;

    for (unsigned int num = 0; num < data.size(); num++)  // Step through bytes in memory
    {
        crc = crc ^ (data[num] << 8);        // Fetch byte from memory, XOR into CRC top byte*/
        for (i=0; i<8; i++)                  // Prepare to rotate 8 bits */
        {
            crc = crc << 1;                  // rotate */
            if (crc & 0x10000)               // bit 15 was set (now bit 16)... */
                crc = (crc ^ poly) & 0xFFFF; // XOR with XMODEM polynomic */
                                             // and ensure CRC remains 16-bit value */
        }                                    // Loop for 8 bits */
    }                                        // Loop until num=data.size() */

    return((unsigned short)crc);             // Return updated CRC */
}

