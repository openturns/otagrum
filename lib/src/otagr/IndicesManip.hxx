//
// Created by phw on 22/01/18.
//

#ifndef OTAGR_INDICESMANIP_H
#define OTAGR_INDICESMANIP_H
#include <openturns/Indices.hxx>

namespace OTAGR {
using Indice = OT::UnsignedInteger;
Indice pos(const OT::Indices &X, Indice y);
bool isIn(const OT::Indices &X, Indice y);

OT::Indices operator+(const OT::Indices &X, Indice y);
OT::Indices operator-(const OT::Indices &X, Indice y);

class IndicesCombinationIterator {
public:
  explicit IndicesCombinationIterator(const OT::Indices &base,
                                      OT::UnsignedInteger n);
  void setFirst();
  bool isLast() const;
  void next();
  const OT::Indices &current() const;
  std::string __str__(const std::string &delim="");

private:
  void set_(Indice pos, int val);
  const OT::Indices base_;
  OT::Indices current_;
  std::vector<OT::UnsignedInteger> coord_;
  bool carry_;
  int size_;
};
} // eo namespace
#endif // OTAGR_INDICESMANIP_H
