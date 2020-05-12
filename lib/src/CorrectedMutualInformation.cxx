#include <cmath>

#include <openturns/EmpiricalBernsteinCopula.hxx>
#include <openturns/NormalCopulaFactory.hxx>

#include "otagrum/CorrectedMutualInformation.hxx"

namespace OTAGRUM
{

CorrectedMutualInformation::CorrectedMutualInformation(const OT::Sample &data)
  : OT::Object()
{
  data_ = (data.rank() + 1) / (data.getSize() + 2); // Switching data to rank space
}

CorrectedMutualInformation::~CorrectedMutualInformation() { }

void CorrectedMutualInformation::setKMode(KModeTypes kmode)
{
  clearCaches();
  kmode_ = kmode;
}

void CorrectedMutualInformation::setCMode(CModeTypes cmode)
{
  clearCaches();
  cmode_ = cmode;
}

void CorrectedMutualInformation::setAlpha(const double alpha)
{
  alpha_ = alpha;
}

double CorrectedMutualInformation::getAlpha() const
{
  return alpha_;
}

OT::UnsignedInteger CorrectedMutualInformation::getSize() const
{
  return data_.getSize();
}

OT::UnsignedInteger CorrectedMutualInformation::getDimension() const
{
  return data_.getDimension();
}

OT::Description CorrectedMutualInformation::getDataDescription() const
{
  return data_.getDescription();
}

void CorrectedMutualInformation::clearCaches() const
{
  clearHCache();
}

void CorrectedMutualInformation::clearHCache() const
{
  HCache_.clear();
}

// Get key associated to an OT::Indices in order to store it in cache
// Question : is K mandatory ?
std::string CorrectedMutualInformation::GetKey(OT::Indices l,
    const OT::UnsignedInteger K) const
{
  if (!l.isIncreasing())              // In order to be independent
  {
    std::sort(l.begin(), l.end());  // of the order of indices
  }
  //return l.__str__();
  return l.__str__() + ":" + std::to_string(K);
}

std::string CorrectedMutualInformation::GetKey(OT::Indices l) const
{
  if (!l.isIncreasing())              // In order to be independent
  {
    std::sort(l.begin(), l.end());  // of the order of indices
  }
  return l.__str__();
}

// Compute the (copula) entropy on variables
double CorrectedMutualInformation::computeCrossEntropy(const OT::Indices &variables,
    const int K)
{
  if (variables.getSize() > 1)    // If there is more than one variable
  {
    //const auto key = GetKey(variables, K);
    std::string key;
    switch (cmode_)
    {
      case CModeTypes::Gaussian:
        key = GetKey(variables);
        break;
      case CModeTypes::Bernstein:
        key = GetKey(variables, K);
        break;
        //default:
    }
    if (!HCache_.exists(key))    // if H(variables) haven't been computed
    {
      auto marginal_data = data_.getMarginal(variables);
      double H = 0.;
      // Is there a way to declare only one copula ?
      // Plus it would cleaner to have no mention of K in the Gaussian case
      auto nc = OT::NormalCopula();
      auto bc = OT::EmpiricalBernsteinCopula();
      switch (cmode_)
      {
        case CModeTypes::Gaussian:
          nc = OT::NormalCopulaFactory().buildAsNormalCopula(marginal_data);
          H = -nc.computeLogPDF(marginal_data).computeMean()[0];
          break;

        case CModeTypes::Bernstein:
          bc = OT::EmpiricalBernsteinCopula(marginal_data, K, false);
          H = -bc.computeLogPDF(marginal_data).computeMean()[0];
          break;

          //default:
          // GUM_ERROR ?

      }
      HCache_.insert(key, H);
      return H;
    }
    else
    {
      return HCache_[key];
    }
  }
  else   // Else entropy is null
  {
    return 0.;
  }
}

OT::UnsignedInteger CorrectedMutualInformation::GetK(const OT::UnsignedInteger size,
    const OT::UnsignedInteger dimension)
{
  return OT::UnsignedInteger(1.0 + std::pow(size, 2.0 / (4.0 + dimension)));
}

double CorrectedMutualInformation::compute2PtInformation(const OT::UnsignedInteger X,
    const OT::UnsignedInteger Y,
    const OT::Indices &U)
{
  OT::UnsignedInteger K = GetK(data_.getSize(), U.getSize() + 2);

  double H_XU = computeCrossEntropy(U + X, K);
  double H_YU = computeCrossEntropy(U + Y, K);
  double H_XYU = computeCrossEntropy(U + X + Y, K);
  double H_U = computeCrossEntropy(U, K);

  double IXY_U = H_XU + H_YU - H_XYU - H_U;

  // TO DO ? add a cache for I

  return IXY_U;

}

double CorrectedMutualInformation::compute2PtPenalty(
  // Commented until a better correction is found
  //const OT::UnsignedInteger X,
  //const OT::UnsignedInteger Y,
  //const OT::Indices &U
)
{
  if (kmode_ == KModeTypes::NoCorr)
  {
    return 0.;
  }
  else if (kmode_ == KModeTypes::Naive)
  {
    return alpha_;
  }
  else
  {
    return 0.;
  }
}

double CorrectedMutualInformation::compute2PtCorrectedInformation(const OT::UnsignedInteger X,
    const OT::UnsignedInteger Y,
    const OT::Indices &U)
{
  // Commented until a better correction is found
  //return compute2PtInformation(X, Y, U) - compute2PtPenalty(X, Y, U);
  return compute2PtInformation(X, Y, U) - compute2PtPenalty();
}

double CorrectedMutualInformation::compute3PtInformation(const OT::UnsignedInteger X,
    const OT::UnsignedInteger Y,
    const OT::UnsignedInteger Z,
    const OT::Indices &U)
{
  OT::UnsignedInteger K = GetK(data_.getSize(), U.getSize() + 3);

  double H_XU = computeCrossEntropy(U + X, K);
  double H_YU = computeCrossEntropy(U + Y, K);
  double H_ZU = computeCrossEntropy(U + Z, K);

  double H_XYU = computeCrossEntropy(U + X + Y, K);
  double H_XZU = computeCrossEntropy(U + X + Z, K);
  double H_YZU = computeCrossEntropy(U + Y + Z, K);

  double H_XYZU = computeCrossEntropy(U + X + Y + Z, K);

  double H_U = computeCrossEntropy(U, K);

  // Maybe it could be better using ICache and the formula
  // I(X;Y;Z|U) = I(X;Y|U) - I(X;Y|U,Z) but what to do about K ?
  //return compute2PtInformation(X, Y, U) - compute2PtInformation(X, Y, U + Z);

  double IXYZ_U = H_XU + H_YU + H_ZU - H_XYU - H_XZU - H_YZU + H_XYZU - H_U;

  return IXYZ_U;
}

double CorrectedMutualInformation::compute3PtPenalty(
  // Commented until a better correction is found
  //const OT::UnsignedInteger X,
  //const OT::UnsignedInteger Y,
  //const OT::UnsignedInteger Z,
  //const OT::Indices &U
)
{
  if (kmode_ == KModeTypes::NoCorr)
  {
    return 0.;
  }
  else if (kmode_ == KModeTypes::Naive)
  {
    return -alpha_;
  }
  else
  {
    return 0.;
  }
}

double CorrectedMutualInformation::compute3PtCorrectedInformation(const OT::UnsignedInteger X,
    const OT::UnsignedInteger Y,
    const OT::UnsignedInteger Z,
    const OT::Indices &U)
{
  // Commented until a better correction is found
  //return compute3PtInformation(X, Y, Z, U) - compute3PtPenalty(X, Y, Z, U);
  return compute3PtInformation(X, Y, Z, U) - compute3PtPenalty();
}

} // namespace OTAGRUM
