/*
 * Proprietary License
 * 
 * Copyright (c) 2024-2025 Dean S Horak
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

#ifndef HTTPSERVERMAIN_H_
#define HTTPSERVERMAIN_H_
#include <string>
#include <vector>
#include <sys/resource.h>
#include <sys/time.h>
#include <math.h>

#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "HTTPMessage.h"
#include "HTTPServer.h"
#include "uri.h"
#include "Global.h"

#include "nlohmann/json.hpp"
#include <boost/algorithm/string.hpp>
#include "TR1Random.h"

using simple_http_server::HttpMethod;
using simple_http_server::HttpRequest;
using simple_http_server::HttpResponse;
using simple_http_server::HttpServer;
using simple_http_server::HttpStatusCode;

//std::string  computeResponse(const std::string content);

std::string bitsToString(char *buffer, long bufferSize);
std::string convertToBinaryString(unsigned char* data, size_t size);
std::stringstream getLastAsciiImage(void);
std::vector<long> getAllComponentIds(void);
HttpResponse show_network_status(const HttpRequest &request);
HttpResponse show_network_view(const HttpRequest &request);

#endif  // HTTPSERVERMAIN_H_