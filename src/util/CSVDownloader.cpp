/*
Copyright (c) 2018, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of trase.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "CSVDownloader.hpp"
#include "util/Exception.hpp"

#include <curl/curl.h>
#include <curl/easy.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <iterator>
#include <cctype>

namespace trase {
template <typename Out>
void split(const std::string &s, char delim, Out result) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    if (!item.empty()) {
      *(result++) = item;
    }
  }
}
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
  std::string string_data((const char *)ptr, (size_t)size * nmemb);
  *((std::stringstream *)stream) << string_data;
  return size * nmemb;
}

CSVDownloader::CSVDownloader() : m_delim(',') {
  m_curl = curl_easy_init();
}

CSVDownloader::~CSVDownloader() {
  curl_easy_cleanup(m_curl);
}

CSVDownloader::data_t
CSVDownloader::parse_csv(std::stringstream &out,
                         const std::vector<std::string> &labels_in) {
  CSVDownloader::data_t store;
  // parse the stringstream as a csv with given delimiter
  std::string line;
  std::vector<std::string> labels;

  // if input labels is empty, assume 1st line of data are labels
  if (labels_in.empty()) {
    std::getline(out, line);
    split(line, m_delim, std::back_inserter(labels));
  } else {
    labels.resize(labels_in.size());
    std::copy(labels_in.begin(), labels_in.end(), labels.begin());
  }

  // parse data
  while (std::getline(out, line)) {
    // ignore all whitespace lines
    if (std::all_of(line.begin(), line.end(),
                    [](unsigned char c) { return std::isspace(c); })) {
      continue;
    }

    // split up line according to delimiter
    std::vector<std::string> elems;
    split(line, m_delim, std::back_inserter(elems));
    if (elems.size() != labels.size()) {
      throw Exception("CSVDownloader found differing line lengths");
    }

    // store individual elements from this line
    for (size_t i = 0; i < labels.size(); ++i) {
      store[labels[i]].push_back(elems[i]);
    }
  }
  return store;
}

CSVDownloader::data_t
CSVDownloader::download(const std::string &url,
                        const std::vector<std::string> &labels) {
  // use curl to read url to a stringstream
  curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
  /* Do not check certificate*/
  curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, 0L);
  /* tell libcurl to follow redirection */
  curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(m_curl, CURLOPT_NOSIGNAL,
                   1); // Prevent "longjmp causes uninitialized stack frame" bug
  curl_easy_setopt(m_curl, CURLOPT_ACCEPT_ENCODING, "deflate");
  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
  std::stringstream out;
  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &out);
  /* Perform the request, res will get the return code */
  CURLcode res = curl_easy_perform(m_curl);
  /* Check for errors */
  if (res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }

  return parse_csv(out, labels);
}

} // namespace trase
