//                                               -*- C++ -*-
/**
 *  @brief IndicesCombinationIterator
 *
 *  Copyright 2010-2020 Airbus-LIP6-Phimeca
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

#include "otagrum/IndicesManip.hxx"

namespace OTAGRUM
{

IndicesCombinationIterator::IndicesCombinationIterator(const OT::Indices &base,
    OT::UnsignedInteger n)
  : OT::Object()
  , base_(base)
  , size_(n)
{
  if (n > base.getSize())
  {
    throw OT::InvalidArgumentException(HERE)
        << "Error: cannot iterate on subsets of size " << n << " in "
        << base.__str__() << ".";
  }
  coord_.reserve(size_);
  setFirst();
}

OT::UnsignedInteger pos(const OT::Indices &X, OT::UnsignedInteger val)
{
  for (OT::UnsignedInteger p = 0; p < X.getSize(); ++p)
  {
    if (X[p] == val)
      return p;
  }
  return X.getSize();
}

bool isIn(const OT::Indices &X, OT::UnsignedInteger y)
{
  return (pos(X, y) != X.getSize());
}

OT::Indices operator+(const OT::Indices &X, OT::UnsignedInteger y)
{
  OT::Indices res(X);
  if (!isIn(res, y))
  {
    res.add(y);
  }
  return res;
}

OT::Indices operator+(const OT::Indices &X, const OT::Indices &Y)
{
  OT::Indices res(X);
  for(const auto y : Y)
  {
    if (!isIn(res, y))
    {
      res.add(y);
    }
  }
  return res;
}

OT::Indices operator-(const OT::Indices &X, OT::UnsignedInteger y)
{
  OT::Indices res(X);
  auto p = pos(X, y);
  if (p < X.getSize())
  {
    res.__delitem__(p);
  }
  return res;
}


void IndicesCombinationIterator::setFirst()
{
  coord_.clear();
  current_.clear();
  for (int i = 0; i < size_; ++i)
  {
    coord_.push_back(i);
    current_ = current_ + base_[i];
  }
  carry_ = false;
}

bool IndicesCombinationIterator::isLast() const
{
  return carry_;
}

void IndicesCombinationIterator::next()
{
  for (int i = size_ - 1; i >= 0; --i)
  {
    if (coord_[i] < base_.getSize() + i - size_)
    {
      set(i, coord_[i] + 1);

      for (int j = i + 1; j < size_; ++j)
      {
        set(j, coord_[j - 1] + 1);
      }
      return;
    }
  }
  carry_ = true;
}

const OT::Indices &IndicesCombinationIterator::current() const
{
  return current_;
}

void IndicesCombinationIterator::set(OT::UnsignedInteger pos, int val)
{
  coord_[pos] = val;
  current_[pos] = base_[coord_[pos]];
}

std::string IndicesCombinationIterator::__str__(const std::string &delim) const
{
  std::stringstream ss;
  ss << delim << "CombinationIterator :\n";
  ss << delim << "  | base :" << static_cast<const OT::Indices>(base_).__str__()
     << "\n";
  ss << delim << "  | coord :";
  for (const auto &i : coord_)
    ss << i << ", ";
  ss << "\n";
  ss << delim << "  | current :" << current_.__str__() << "\n";
  return ss.str();
}

} // eo namespace
