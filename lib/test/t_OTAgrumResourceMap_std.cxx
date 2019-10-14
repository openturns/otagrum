#include <iostream>

#include "otagrum/OTAgrumResourceMap.hxx"

void check(const std::string &key)
{
  if (OT::ResourceMap::HasKey(key))
    std::cout << "<" << key << ">  =  <" << OT::ResourceMap::Get(key) << ">\n";
  else
    std::cout << "!HasKey " << key << std::endl;
}

int main(int argc, char **argv)
{
  try
  {
    check("JunctionTreeBernsteinCopulaFactory-DefaultBinNumber");
    check("JunctionTreeBernsteinCopulaFactory-DefaultAlpha");
    check("JunctionTreeBernsteinCopulaFactory-DefaultMaximumConditioningSetSize");
  }
  catch (OT::Exception &e)
  {
    std::cout << e.__repr__() << std::endl;
    return 1;
  }
  return 0;
}
