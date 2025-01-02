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

// Define object that can be used to represent a Unique Resource Identifier
// which is useful for parsing and request handling

#ifndef URI_H_
#define URI_H_

#include <algorithm>
#include <cctype>
#include <string>
#include <utility>
#include <cstdint>

namespace simple_http_server {

// A Uri object will contain a valid scheme (for example: HTTP), host,
// port, and the actual URI path
class Uri {
 public:
  Uri() = default;
  explicit Uri(const std::string& path) : path_(path) { SetPathToLowercase(); }
  ~Uri() = default;

  inline bool operator<(const Uri& other) const { return path_ < other.path_; }
  inline bool operator==(const Uri& other) const {
    return path_ == other.path_;
  }

  void SetPath(const std::string& path) {
    path_ = std::move(path);
    SetPathToLowercase();
  }

  std::string scheme() const { return scheme_; }
  std::string host() const { return host_; }
  std::uint16_t port() const { return port_; }
  std::string path() const { return path_; }

 private:
  // Only the path is supported for now
  std::string path_;
  std::string scheme_;
  std::string host_;
  std::uint16_t port_;

  void SetPathToLowercase() {
    std::transform(path_.begin(), path_.end(), path_.begin(),
                   [](char c) { return tolower(c); });
  }
};

}  // namespace simple_http_server

#endif  // URI_H_
