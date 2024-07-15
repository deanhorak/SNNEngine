#ifndef HTTPSERVERMAIN_H_
#define HTTPSERVERMAIN_H_
#include <string>

//std::string  computeResponse(const std::string content);

std::string bitsToString(char *buffer, long bufferSize);
std::string convertToBinaryString(unsigned char* data, size_t size);

#endif  // HTTPSERVERMAIN_H_