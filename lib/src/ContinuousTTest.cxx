//                                               -*- C++ -*-
/**
 *  @brief ContinuousTTest
 *
 *  Copyright 2010-2018 Airbus-LIP6-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cmath>
#include <tuple>

#include <openturns/EmpiricalBernsteinCopula.hxx>
#include <openturns/NormalCopulaFactory.hxx>
#include <openturns/SpecFunc.hxx>
#include <openturns/DistFunc.hxx>
#include <openturns/Log.hxx>

#include "otagrum/ContinuousTTest.hxx"



namespace OTAGRUM
{

ContinuousTTest::ContinuousTTest(const OT::Sample & data,
                                 const double alpha)
  : OT::Object()
{
  setAlpha(alpha);
  data_ = (data.rank() + 1.0) / data.getSize();
}


OT::UnsignedInteger ContinuousTTest::GetK(const OT::Sample & sample)
{
  return OT::UnsignedInteger(1.0 + std::pow(sample.getSize(), 2.0 / (4.0 + sample.getDimension())));
}

std::string ContinuousTTest::GetKey(const OT::Indices & l,
                                    const OT::UnsignedInteger k)
{
  //@todo : sorting l before computing the key ?
  return l.__str__() + ":" + std::to_string(k);
}

OT::Point ContinuousTTest::getLogPDF(const OT::Indices & l,
                                     OT::UnsignedInteger k) const
{
  if (l.getSize() == 0)
  {
    //@todo Is this correct ?
    return OT::Point(1, -std::log(data_.getSize()));
  }
  // default value for k only depends on the size of the sample
  if (k == 0)
  {
    // k =BernsteinCopulaFactory::ComputeLogLikelihoodBinNumber(sample,2);
    // k =BernsteinCopulaFactory::ComputeAMISEBinNumber(sample,2);
    k = GetK(data_);
  }

  const auto key = GetKey(l, k);
  if (cache_.exists(key)) return cache_.get(key);
  auto dL = data_.getMarginal(l);

  // OT::BernsteinCopulaFactory factory;
  // auto logPDF = factory.build(dL, k).computeLogPDF(dL).asPoint();

  // OT::NormalCopulaFactory factory;
  // auto logPDF = factory.build(dL).computeLogPDF(dL).asPoint();

  LOGINFO(OT::OSS() << "Compute log-PDF for k=" << k << ", l=" << l);
  auto logPDF = OT::EmpiricalBernsteinCopula(dL, k, true).computeLogPDF(dL).asPoint();

  cache_.set(l.getSize(), key, logPDF);
  return logPDF;
}

std::tuple<OT::Point, OT::Point, OT::Point, OT::Point, OT::UnsignedInteger>
ContinuousTTest::getLogPDFs(const OT::UnsignedInteger Y,
                            const OT::UnsignedInteger Z,
                            const OT::Indices & X) const
{
  //@todo how to be smart for k ?
  OT::UnsignedInteger k = GetK(data_);
  OT::Point logPDF1(getLogPDF(X, k));
  OT::Point logPDF2(getLogPDF(X + Y, k));
  OT::Point logPDF3(getLogPDF(X + Z, k));
  OT::Point logPDF4(getLogPDF(X + Y + Z, k));
  return std::make_tuple(logPDF1, logPDF2, logPDF3, logPDF4, k);
}

void ContinuousTTest::setAlpha(const double alpha)
{
  alpha_ = alpha;
}

double ContinuousTTest::getAlpha() const
{
  return alpha_;
}

inline double pPar1MoinsP(const double p)
{
  return p * (1 - p);
}

double ContinuousTTest::getTTest(const OT::UnsignedInteger Y,
                                 const OT::UnsignedInteger Z,
                                 const OT::Indices & X) const
{
  OT::UnsignedInteger k = 0;

  const auto dY = data_.getMarginal(Y);
  const auto dZ = data_.getMarginal(Z);
  const auto dX = data_.getMarginal(X);

  OT::Point logFX, logFYX, logFZX, logFYZX;
  std::tie(logFX, logFYX, logFZX, logFYZX, k) = getLogPDFs(Y, Z, X);
  const auto d = X.getSize();
  const auto N = data_.getSize();

  const auto C1 = std::pow(0.5, d + 2.0) * std::pow(M_PI, 0.5 * d + 1.0);
  const auto sigma = M_SQRT2 * std::pow(M_PI / 4.0, 0.5 * d + 1.0);

  double H = 0.0;
  double B1 = 0.0;
  double B2 = 0.0;
  double B3 = 0.0;
  const double facteurpi = 1.0 / std::pow(4 * M_PI, 0.5 * d + 0.5);
  const double smallLog = std::log(OT::SpecFunc::ScalarEpsilon);
  double logDenominator = 0.0;
  if (d == 0)
  {
    const double fX0 = std::exp(logFX[0]);
    for (unsigned int i = 0; i < N; ++i)
    {
      logDenominator = logFYZX[i];
      if (logDenominator > smallLog)
      {
        H += std::pow(-std::expm1(0.5 * (logFYX[i] + logFZX[i] - logDenominator)), 2.0);
        B1 += facteurpi / (std::sqrt(pPar1MoinsP(dY(i, 0))) * std::exp(logFYX[i]));
        B2 += facteurpi / (std::sqrt(pPar1MoinsP(dZ(i, 0))) * std::exp(logFZX[i]));
        B3 += fX0;
      } // logDenominator > smallLog
    } // i
  } // d == 0
  else
  {
    for (unsigned int i = 0; i < N; ++i)
    {
      logDenominator = logFYZX[i] + logFX[i];
      if (logDenominator > smallLog)
      {
        H += std::pow(-std::expm1(0.5 * (logFYX[i] + logFZX[i] - logDenominator)), 2.0);
        double gX = 1.0;
        for (unsigned int j = 0; j < d; ++j)
          gX /= pPar1MoinsP(dX(i, j));
        gX = std::sqrt(gX);
        B1 += facteurpi * gX / (std::sqrt(pPar1MoinsP(dY(i, 0))) * std::exp(logFYX[i]));
        B2 += facteurpi * gX / (std::sqrt(pPar1MoinsP(dZ(i, 0))) * std::exp(logFZX[i]));
        B3 += std::exp(logFX[i]) * gX;
      } // logDenominator > smallLog
    } // i
  } // d > 0
  // mean
  H /= N;

  const double term12 = -std::pow(0.5, d) * std::pow(M_PI, 0.5 * d + 0.5);
  const double fact3 = std::pow(0.5, d + 1.0) / std::pow(M_PI, 0.5 * d);
  B1 = term12 + B1 / N;
  B2 = term12 + B2 / N;
  B3 = fact3 * B3 / N;

  auto T = std::pow(1.0 / k, 0.5 * d + 1.0) / sigma;
  T *= 4 * H * N - pow(k, 0.5 * d) * (C1 * k + (B1 + B2) * std::sqrt(k) + B3);
  return T;
}

double ContinuousTTest::getTTestWithoutCorrections(OT::UnsignedInteger Y, OT::UnsignedInteger Z,
    const OT::Indices &X) const
{
  OT::UnsignedInteger k = 0;

  const auto dY = data_.getMarginal(Y);
  const auto dZ = data_.getMarginal(Z);
  const auto dX = data_.getMarginal(X);

  OT::Point logFX, logFYX, logFZX, logFYZX;
  std::tie(logFX, logFYX, logFZX, logFYZX, k) = getLogPDFs(Y, Z, X);

  const auto d = X.getSize();
  const auto N = data_.getSize();

  const auto sigma = M_SQRT2 * std::pow(M_PI / 4, 0.5 * d + 1.0);

  double H = 0.0;

  if (d == 0)
    for (unsigned int i = 0; i < N; ++i)
      H += std::pow(-std::expm1(0.5 * logFYX[i] + logFZX[i] - logFYZX[i]), 2.0);
  else
    for (unsigned int i = 0; i < N; ++i)
      H += std::pow(-std::expm1(0.5 * logFYX[i] + logFZX[i] - logFYZX[i] - logFX[i]), 2.0);
  const auto T = 4 * H * N - pow(k, 0.5 * d) / std::pow(k, 0.5 * d + 1.0) / sigma;
  return T;
}

std::tuple<double, double, bool>
ContinuousTTest::isIndep(const OT::UnsignedInteger Y, const OT::UnsignedInteger Z, const OT::Indices & X) const
{
  return ContinuousTTest::isIndepFromTest(getTTest(Y, Z, X), alpha_);
}

std::tuple<double, double, bool>
ContinuousTTest::isIndepFromTest(const double t, const double alpha)
{
  const double p = 2.0 * OT::DistFunc::pNormal(-std::abs(t));
  return std::make_tuple(t, p, p >= alpha);
}

std::string ContinuousTTest::__str__(const std::string &offset) const
{
  std::stringstream ss;
  ss << offset << "Data dimension : " << data_.getDimension() << std::endl;
  ss << offset << "Data size : " << data_.getSize() << std::endl;
  ss << offset << "Cache : " << std::endl
     << cache_.__str__(offset + "      |") << std::endl;
  ss << offset << "alpha :" << getAlpha() << std::endl;
  return ss.str();
}

void ContinuousTTest::clearCache() const
{
  cache_.clear();
}

void ContinuousTTest::clearCacheLevel(const OT::UnsignedInteger level) const
{
  cache_.clearLevel(level);
}

OT::UnsignedInteger ContinuousTTest::getDimension() const
{
  return data_.getDimension();
}
} // namespace OTAGRUM
