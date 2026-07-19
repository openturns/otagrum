//                                               -*- C++ -*-
/**
 *  @brief Candidate value type and comparators used by ContinuousMIIC's ranking heaps
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
#ifndef OTAGRUM_GREATER_HXX
#define OTAGRUM_GREATER_HXX

#include <memory>     // std::shared_ptr
#include <utility>    // std::pair
#include <tuple>      // std::tuple
#include <openturns/OT.hxx>  // OT::UnsignedInteger, OT::Indices

#include "otagrum/otagrumprivate.hxx"

namespace OTAGRUM
{
#ifndef SWIG
/* A (X, Y, Z, U) candidate considered by ContinuousMIIC while looking for the
 * best contributor Z to the conditioning set U of the pair (X, Y): X and Y
 * are the two variables being tested, Z is the candidate contributor, and U
 * is the current conditioning set. Wraps a std::shared_ptr so that the value
 * stays cheap to copy (required by gum::Heap, which copies its elements
 * internally) without duplicating the underlying tuple. */
class OTAGRUM_API RankedTriple
{
public:
  typedef std::tuple< OT::UnsignedInteger, OT::UnsignedInteger,
          OT::UnsignedInteger, OT::Indices > TupleType;

  RankedTriple() : data_(std::make_shared<TupleType>()) {}
  RankedTriple(const OT::UnsignedInteger x, const OT::UnsignedInteger y,
              const OT::UnsignedInteger z, const OT::Indices &u)
    : data_(std::make_shared<TupleType>(x, y, z, u)) {}

  OT::UnsignedInteger getX() const
  {
    return std::get<0>(*data_);
  }
  OT::UnsignedInteger getY() const
  {
    return std::get<1>(*data_);
  }
  OT::UnsignedInteger getZ() const
  {
    return std::get<2>(*data_);
  }
  OT::Indices &getU()
  {
    return std::get<3>(*data_);
  }
  const OT::Indices &getU() const
  {
    return std::get<3>(*data_);
  }

private:
  std::shared_ptr<TupleType> data_;
};

class OTAGRUM_API GreaterPairOn2nd
{
public:
  bool operator()(
    const std::pair< RankedTriple, double > & e1,
    const std::pair< RankedTriple, double > & e2) const;
};
#endif

class OTAGRUM_API GreaterTupleOnLast
{
public:
  bool operator()(
    const std::tuple< std::tuple< OT::UnsignedInteger,
    OT::UnsignedInteger,
    OT::UnsignedInteger >*,
    double, double, double >& e1,
    const std::tuple< std::tuple< OT::UnsignedInteger,
    OT::UnsignedInteger,
    OT::UnsignedInteger >*,
    double, double, double >& e2) const;
};

}

#endif // OTAGRUM_GREATER_HXX
