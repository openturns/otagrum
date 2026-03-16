#ifndef OTAGRUM_GREATER_HXX
#define OTAGRUM_GREATER_HXX

#include <utility>    // std::pair
#include <tuple>      // std::tuple
#include <openturns/OT.hxx>  // OT::UnsignedInteger, OT::Indices

namespace OTAGRUM
{
#ifndef SWIG
class GreaterPairOn2nd
{
public:
  bool operator()(
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
    double > & e2) const;
};
#endif

class GreaterTupleOnLast
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
