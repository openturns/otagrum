//                                               -*- C++ -*-
/**
 *  @brief The StratifiedCache is a cache for TTest with level
 *
 *  Copyright 2010-2018 Airbus-LIP6-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTAGRUM_STRATIFIEDCACHE_HXX
#define OTAGRUM_STRATIFIEDCACHE_HXX

#include <sstream>
#include <string>

#include <agrum/core/hashTable.h>

#include <openturns/Sample.hxx>

namespace OTAGRUM {

class StratifiedCache : public OT::Object {
private:
  gum::HashTable<std::string, OT::Point> cache_;
  std::vector<std::vector<std::string>> stratified_keys_;

  // for internal statistical purpose
  mutable long get_;
  mutable long set_;

public:
  StratifiedCache();

  ~StratifiedCache();

  bool exists(const std::string &key) const;

  const OT::Point get(const std::string &key) const;

  void set(OT::UnsignedInteger level, const std::string &key, const OT::Point sample);

  void clearLevel(unsigned long level);

  void clear();

  int size() const;
  int size(int level) const;

  int maxLevel() const;

  std::string __str__(const std::string& offset="") const;
};
} // OTAGRUM
#endif // OTAGRUM_STRATIFIEDCACHE_H
