//                                               -*- C++ -*-
/**
 *  @brief The StratifiedCache is a cache for TTest with level
 *
 *  Copyright 2010-2018 Airbus-LIP6-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include <agrum/core/hashTable.h>

#include "otagrum/StratifiedCache.hxx"

namespace OTAGRUM {

StratifiedCache::StratifiedCache() : get_(0u), set_(0u){};

StratifiedCache::~StratifiedCache() { clear(); };

bool StratifiedCache::exists(const std::string &key) const {
  return cache_.exists(key);
}

const OT::Point StratifiedCache::get(const std::string &key) const {
  get_++;
  return cache_[key];
};

void StratifiedCache::set(OT::UnsignedInteger level, const std::string &key,
                          const OT::Point sample) {
  set_++;
  if (cache_.exists(key)) {
    return; // throw something ?
  }
  cache_.insert(key, sample);

  while (level >= stratified_keys_.size())
    stratified_keys_.push_back(std::vector<std::string>());
  stratified_keys_[level].push_back(key);
};

void StratifiedCache::clearLevel(int level) {
  if (level < stratified_keys_.size()) {
    for (auto &elt : stratified_keys_[level])
      cache_.erase(elt);
    stratified_keys_[level].clear();
  }
};

void StratifiedCache::clear() {
  for (int level = 0; level < stratified_keys_.size(); ++level) {
    clearLevel(level);
  }
};

int StratifiedCache::size() const { return cache_.size(); };
int StratifiedCache::size(int level) const {
  return stratified_keys_[level].size();
}

int StratifiedCache::maxLevel() const { return stratified_keys_.size() - 1; };

std::string StratifiedCache::__str__(const std::string &offset) const {
  std::stringstream ss;
  for (int i = 0; i <= maxLevel(); i++) {
    if (stratified_keys_[i].size() > 0) {
      ss << offset << i << " ";
      char delim = ':';
      for (const auto key : stratified_keys_[i]) {
        ss << delim << ' ' << key;
        delim = ',';
      }
      ss << std::endl;
    }
  }
  return ss.str();
}
} // namespace OTAGRUM
