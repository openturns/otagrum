#include "otagrum/Greater.hxx"

namespace OTAGRUM
{

bool GreaterPairOn2nd::operator()(
  const std::pair <
  std::tuple< OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::Indices >*,
  double > & e1,
  const std::pair <
  std::tuple< OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::Indices >*,
  double > & e2) const
{
  return e1.second > e2.second;
}

bool GreaterTupleOnLast::operator()(
  const std::tuple< std::tuple< OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::UnsignedInteger >*,
  double, double, double >& e1,
  const std::tuple< std::tuple< OT::UnsignedInteger,
  OT::UnsignedInteger,
  OT::UnsignedInteger >*,
  double, double, double >& e2) const
{
  double p1XZ = std::get< 2 >(e1);
  double p1YZ = std::get< 3 >(e1);
  double p2XZ = std::get< 2 >(e2);
  double p2YZ = std::get< 3 >(e2);
  double I1 = std::get< 1 >(e1);
  double I2 = std::get< 1 >(e2);
  // First, we look at the sign of information.
  // Then, the probility values
  // and finally the abs value of information.
  if ( (I1 < 0 && I2 < 0) || (I1 >= 0 && I2 >= 0) )
  {
    if (std::max(p1XZ, p1YZ) == std::max(p2XZ, p2YZ))
    {
      return std::abs(I1) > std::abs(I2);
    }
    else
    {
      return std::max(p1XZ, p1YZ) > std::max(p2XZ, p2YZ);
    }
  }
  else
  {
    return I1 < I2;
  }
}

} // namespace OTAGRUM
