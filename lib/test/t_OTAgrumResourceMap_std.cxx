#include <iostream>

#include "otagrum/Utils.hxx"

void check(const std::string &key) {
  std::cout <<"<"<< key << ">  =  <" << OT::ResourceMap::Get(key)<<">\n";
}
int main(int argc, char **argv) {
  check("OTAgrum.ContinuousPC.defaultNbBins");
  check("OTAgrum.ContinuousPC.defaultAlpha");
  check("OTAgrum.ContinuousPC.defaultMaximumConditioningSetSize");
  return 0;
}
