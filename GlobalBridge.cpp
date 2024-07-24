/*
 * Proprietary License
 * 
 * Copyright (c) 2024 Dean S Horak
 * All rights reserved.
 * 
 * This software is the confidential and proprietary information of Dean S Horak ("Proprietary Information").
 * You shall not disclose such Proprietary Information and shall use it only in accordance with the terms
 * of the license agreement you entered into with Dean S Horak.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are not permitted
 * without express written permission from Dean S Horak.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * If you have any questions about this license, please contact Your Name at dean.horak@gmail.com.
 */

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





