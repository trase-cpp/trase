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

#include "backend/Backend.hpp"

#include <cctype>
#include <dirent.h>

namespace trase {

FontManager::FontManager() {
#ifdef _WIN32
  const char sep = '\\';
#else
  const char sep = '/';
#endif
  add_font_dir(std::string(TRASE_SOURCE_DIR) + sep + "font");
  add_font_dir(std::string(TRASE_INSTALL_DIR) + sep + "font");
}

void FontManager::clear_font_dirs() {
  m_font_dirs.clear();
  m_list_of_available_fonts.clear();
}

void FontManager::add_system_fonts() {
#ifdef _WIN32
  const std::string system_font_dir = "c:\\Windows\\Fonts";
#elif __APPLE__
  const std::string system_font_dir = "/Library/Fonts/";
#elif __linux__
  const std::string system_font_dir = "/usr/share/fonts/";
#elif __unix__ // all unices not caught above
  const std::string system_font_dir = "/usr/share/fonts/";
#elif defined(_POSIX_VERSION)
  const std::string system_font_dir = "/usr/share/fonts/";
#else
#error "Unknown system"
#endif
  add_font_dir(system_font_dir);
}

void FontManager::add_font_dir(const std::string &path) {
  m_font_dirs.push_back(path);
  list_fonts(m_font_dirs.back());
}

void FontManager::list_fonts(const std::string &path) {
#ifdef _WIN32
  const char sep = '\\';
#else
  const char sep = '/';
#endif

  if (auto dir = opendir(path.c_str())) {
    while (auto f = readdir(dir)) {
      if (static_cast<char *>(f->d_name) == nullptr || f->d_name[0] == '.') {
        continue;
      }
      if (f->d_type == DT_DIR) {
        list_fonts(path + sep + static_cast<char *>(f->d_name));
      }
      if (f->d_type == DT_REG && static_cast<char *>(f->d_name) != nullptr) {
        std::string name(static_cast<char *>(f->d_name));
        std::string ext("ttf");
        if (name.length() >= ext.length() &&
            0 ==
                name.compare(name.length() - ext.length(), ext.length(), ext)) {
          m_list_of_available_fonts.push_back(path + sep +
                                              static_cast<char *>(f->d_name));
        }
      }
    }
    closedir(dir);
  }
}

std::string FontManager::find_font(const std::string &name1,
                                   const std::string &name2) {
  auto it = std::find_if(m_list_of_available_fonts.begin(),
                         m_list_of_available_fonts.end(), [&](std::string i) {
                           if (i.find(name1) == std::string::npos) {
                             return false;
                           }
                           if (name2.empty()) {
                             return true;
                           }
                           std::string lower = i;
                           std::transform(
                               lower.begin(), lower.end(), lower.begin(),
                               [](unsigned char c) { return std::tolower(c); });
                           return lower.find(name2) != std::string::npos;
                         });
  if (it == m_list_of_available_fonts.end()) {
    return "";
  }
  return *it;
}

} // namespace trase
