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

/// \file CSVDownloader.hpp

#ifndef _CSVDownloader_H_
#define _CSVDownloader_H_

#include <map>
#include <string>
#include <vector>

namespace trase {

class CSVDownloader {
public:
  /// columns of csv file returned as a map from string label to a column of
  /// data (vector of strings)
  using data_t = std::map<std::string, std::vector<std::string>>;

  /// constructs a csv downloader with default delimiter of ','
  CSVDownloader();

  ~CSVDownloader();

  /// download a csv file at the url given by @p url
  ///
  /// @param url url to download csv file from
  /// @param labels if empty, the first line of the csv file is assumed to
  /// contain the column labels If not empty, this vector contains the column
  /// labels
  data_t download(const std::string &url,
                  const std::vector<std::string> &labels = {});

  /// set the delimiter for the csv file format
  void set_delim(const char arg) { m_delim = arg; }

private:
  /// parse a csv file given as a `std::stringstream`
  CSVDownloader::data_t parse_csv(std::stringstream &out,
                                  const std::vector<std::string> &labels);

  /// pointer to libcurl data
  void *m_curl;

  /// delimiter for the csv file format
  char m_delim;
};

} // namespace trase

#endif // _CSVDownloader_H_
