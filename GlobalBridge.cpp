#include "Global.h"
#include "GlobalBridge.h"

extern Global *globalObject;

bool GlobalBridge::componentKeyInRange(long key)
{
    return globalObject->componentKeyInRange(key);
}

char* GlobalBridge::allocClearedMemory(long bufferSize)
{
    return (char *)globalObject->allocClearedMemory(bufferSize);
}

void GlobalBridge::freeMemory(char *ptr)
{
    globalObject->freeMemory(ptr);
}

void GlobalBridge::log(std::stringstream &ss)
{
    globalObject->log(ss);
}

size_t GlobalBridge::getTypeIndex(std::string  name)
{
    return globalObject->getTypeIndex(name);
}

long GlobalBridge::getComponentBase(size_t index)
{
    return globalObject->componentBase[index];
}

long GlobalBridge::getComponentCounter(size_t index)
{
    return globalObject->componentCounter[index];
}





