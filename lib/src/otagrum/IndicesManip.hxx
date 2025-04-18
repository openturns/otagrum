//                                               -*- C++ -*-
/**
 *  @brief IndicesCombinationIterator
 *
 *  Copyright 2010-2025 Airbus-LIP6-Phimeca
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

#ifndef OTAGRUM_INDICESMANIP_HXX
#define OTAGRUM_INDICESMANIP_HXX

#include "otagrum/otagrumprivate.hxx"
#include <openturns/Indices.hxx>

namespace OTAGRUM
{

OTAGRUM_API OT::Indices operator+(const OT::Indices &X, OT::UnsignedInteger y);
OTAGRUM_API OT::Indices operator+(const OT::Indices &X, const OT::Indices &Y);
OTAGRUM_API OT::Indices operator-(const OT::Indices &X, OT::UnsignedInteger y);

class OTAGRUM_API IndicesCombinationIterator : public OT::Object
{
public:
  IndicesCombinationIterator(const OT::Indices & base,
                             OT::UnsignedInteger n);
  void setFirst();
  bool isLast() const;
  void next();
  const OT::Indices &current() const;
  std::string __str__(const std::string &delim = "") const;

private:
  void set(OT::UnsignedInteger pos, int val);

  const OT::Indices base_;
  OT::Indices current_;
  std::vector<OT::UnsignedInteger> coord_;
  bool carry_;
  int size_;
};

} // eo namespace

#endif // OTAGRUM_INDICESMANIP_HXX
