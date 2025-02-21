//                                               -*- C++ -*-
/**
 *  @brief ContinuousTTest
 *
 *  Copyright 2010-2024 Airbus-LIP6-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
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

#include <agrum/base/core/timer.h>
#include <openturns/DistFunc.hxx>
#include <openturns/EmpiricalBernsteinCopula.hxx>
#include <openturns/Log.hxx>
#include <openturns/NormalCopulaFactory.hxx>
#include <openturns/SpecFunc.hxx>

#include "otagrum/ContinuousTTest.hxx"

#define TRACE_CONTINUOUS_TTEST(x)                                         \
  {                                                                       \
    if (verbose_)                                                         \
        std::cout << x;                                                   \
  }                                                                       \

namespace OTAGRUM
{

ContinuousTTest::ContinuousTTest(const OT::Sample &data, const double alpha)
  : OT::Object(),
    verbose_(false)
{
  setAlpha(alpha);
  data_ = (data.rank() + 0.5) / data.getSize();  // Switching data to rank space
}

OT::UnsignedInteger ContinuousTTest::GetK(const OT::UnsignedInteger size,
    const OT::UnsignedInteger dimension)
{
  return OT::UnsignedInteger(1.0 + std::pow(size, 2.0 / (4.0 + dimension)));
}

std::string ContinuousTTest::GetKey(const OT::Indices &l,
                                    const OT::UnsignedInteger k)
{
  //@todo : sorting l before computing the key ?
  return l.__str__() + ":" + std::to_string(k);
}

OT::Point ContinuousTTest::getLogPDF(const OT::Indices &l,
                                     const OT::UnsignedInteger k) const
{
  if (l.getSize() == 0)
  {
    //@todo Is this correct ?
    return OT::Point(1, -std::log(data_.getSize()));
  }
  if (l.getSize() == 1)
    return OT::Point(1, 0.0);

  const auto key = GetKey(l, k);
  if (cache_.exists(key))
    return cache_.get(key);

  auto dL = data_.getMarginal(l);

  // OT::BernsteinCopulaFactory factory;
  // auto logPDF = factory.build(dL, k).computeLogPDF(dL).asPoint();

  // OT::NormalCopulaFactory factory;
  // auto logPDF = factory.build(dL).computeLogPDF(dL).asPoint();

  LOGINFO(OT::OSS() << "Compute log-PDF for k=" << k << ", l=" << l);
  auto logPDF =
    OT::EmpiricalBernsteinCopula(dL, k, true).computeLogPDF(dL).asPoint();
  LOGINFO(OT::OSS() << "End of compute log-PDF for k=" << k << ", l=" << l);

  cache_.set(l.getSize(), key, logPDF);
  return logPDF;
}

std::tuple<OT::Point, OT::Point, OT::Point, OT::Point, OT::UnsignedInteger>
ContinuousTTest::getLogPDFs(const OT::UnsignedInteger Y,
                            const OT::UnsignedInteger Z,
                            const OT::Indices &X) const
{
  //@todo how to be smart for k ?
  // k =BernsteinCopulaFactory::ComputeLogLikelihoodBinNumber(sample,2);
  OT::UnsignedInteger k = GetK(data_.getSize(), X.getSize() + 2);
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
  return p * (1.0 - p);
}

double ContinuousTTest::getTTest(const OT::UnsignedInteger Y,
                                 const OT::UnsignedInteger Z,
                                 const OT::Indices &X) const
{

  OT::UnsignedInteger k = 0;  // Bandwidth parameter

  OT::Point logFX, logFYX, logFZX, logFYZX;
  std::tie(logFX, logFYX, logFZX, logFYZX, k) = getLogPDFs(Y, Z, X);

  const auto d = X.getSize();     // Conditioning set dimension
  const auto N = data_.getSize(); // Size of data set

  const auto C1 = std::pow(0.5, d + 2.0) * std::pow(M_PI, 0.5 * d + 1.0);
  const auto sigma = M_SQRT2 * std::pow(M_PI / 4.0, 0.5 * d + 1.0);

  double H = 0.0;
  double dH = 0.0;

  double B1 = 0.0;
  double B2 = 0.0;
  double B3 = 0.0;

  const double facteurpi = 1.0 / std::pow(4 * M_PI, 0.5 * d + 0.5);

  const double small = OT::SpecFunc::Precision;
  const double smallLog = std::log(OT::SpecFunc::Precision);

  double logDenominator = 0.0;

  double yI = 0.0;
  double zI = 0.0;


  if (d == 0)    // If the conditioning set is empty
  {
    const double fX0 = 1.0;  // Why isn't it 0.0 ?
    for (unsigned int i = 0; i < N; ++i)
    {
      logDenominator = logFYZX[i];
      yI = data_(i, Y);
      zI = data_(i, Z);
      if ((logDenominator > smallLog) && (yI > small) && (yI < 1.0 - small) &&
          (zI > small) && (zI < 1.0 - small))
      {
        // dH^2 = (1-sqrt(1 * 1 / (fYZX * 1)))^2
        //      = (1-exp(0.5*log(1 / fYZX))^2
        //      = (-expm1(-0.5*log(fYZX)))^2
        //      = (expm1(-0.5*log(fYZX)))^2
        dH = std::expm1(-0.5 * logDenominator);
        H += dH * dH;
        B1 += facteurpi / std::sqrt(pPar1MoinsP(yI));
        B2 += facteurpi / std::sqrt(pPar1MoinsP(zI));
        B3 += fX0;
      } // logDenominator > smallLog
      else
        LOGINFO(OT::OSS() << "Skipped contribution i=" << i
                << ", logDenominator=" << logDenominator);
    } // i
  }   // d == 0

  else if (d == 1)    // If the conditioning set is of size 1
  {
    for (unsigned int i = 0; i < N; ++i)
    {
      logDenominator = logFYZX[i];
      yI = data_(i, Y);
      zI = data_(i, Z);
      if ((logDenominator > smallLog) && (yI > small) && (yI < 1.0 - small) &&
          (zI > small) && (zI < 1.0 - small))
      {
        double xJ = data_(i, X[0]);
        if ((xJ <= small) || (xJ >= 1.0 - small))
        {
          continue;
        }
        double gX = 1.0 / pPar1MoinsP(xJ);
        gX = std::sqrt(gX);
        // dH^2 = (1-sqrt(fYX * fZX / (fYZX * 1)))^2
        //      = (1-exp(0.5*log(fYX * fZX / fYZX))^2
        //      = (-expm1(0.5*(log(fYX) + log(fZX) - log(fYZX))))^2
        //      = (expm1(0.5*(log(fYX) + log(fZX) - log(fYZX))))^2
        dH = std::expm1(0.5 * (logFYX[i] + logFZX[i] - logDenominator));
        H += dH * dH;
        B1 +=
          facteurpi * gX / (std::sqrt(pPar1MoinsP(yI)) * std::exp(logFYX[i]));
        B2 +=
          facteurpi * gX / (std::sqrt(pPar1MoinsP(zI)) * std::exp(logFZX[i]));
        B3 += gX;

        TRACE_CONTINUOUS_TTEST("B1 = " << B1 << std::endl);
        TRACE_CONTINUOUS_TTEST("B2 = " << B2 << std::endl);
        TRACE_CONTINUOUS_TTEST("B3 = " << B3 << std::endl);

      } // logDenominator > smallLog
      else
        LOGDEBUG(OT::OSS() << "Skip contribution i=" << i
                 << ", logDenominator=" << logDenominator);
    } // i
  }   // d == 1

  else
  {
    for (unsigned int i = 0; i < N; ++i)
    {
      logDenominator = logFYZX[i] + logFX[i];
      yI = data_(i, Y);
      zI = data_(i, Z);
      if ((logDenominator > smallLog) && (yI > small) && (yI < 1.0 - small) &&
          (zI > small) && (zI < 1.0 - small))
      {
        double gX = 1.0;
        double xJ = 0.0;
        OT::Bool isSmall = false;
        for (unsigned int j = 0; j < d; ++j)
        {
          xJ = data_(i, X[j]);
          if ((xJ <= small) || (xJ >= 1.0 - small))
          {
            isSmall = true;
            break;
          }
          gX /= pPar1MoinsP(xJ);
          gX = std::sqrt(gX);
        }
        if (isSmall)
          continue;
        // dH^2 = (1-sqrt(fYX * fZX / (fYZX * fX)))^2
        //      = (1-exp(0.5*log(fYX * fZX / (fYZX * fX)))^2
        //      = (-expm1(0.5*(log(fYX) + log(fZX) - log(fYZX) - log(fX))))^2
        //      = (expm1(0.5*(log(fYX) + log(fZX) - log(fYZX) - log(fX))))^2
        dH = std::expm1(0.5 * (logFYX[i] + logFZX[i] - logDenominator));
        H += dH * dH;
        B1 +=
          facteurpi * gX / (std::sqrt(pPar1MoinsP(yI)) * std::exp(logFYX[i]));
        B2 +=
          facteurpi * gX / (std::sqrt(pPar1MoinsP(zI)) * std::exp(logFZX[i]));
        B3 += std::exp(logFX[i]) * gX;

        TRACE_CONTINUOUS_TTEST("B1 = " << B1 << std::endl);
        TRACE_CONTINUOUS_TTEST("B2 = " << B2 << std::endl);
        TRACE_CONTINUOUS_TTEST("B3 = " << B3 << std::endl);

      } // logDenominator > smallLog
      else
        LOGDEBUG(OT::OSS() << "Skip contribution i=" << i
                 << ", logDenominator=" << logDenominator);
    } // i
  }   // d > 0

  // mean
  H /= N;

  const double term12 = -std::pow(0.5, d) * std::pow(M_PI, 0.5 * d + 0.5);
  const double fact3 = std::pow(0.5, d - 1.0) / std::pow(M_PI, 0.5 * d);

  B1 = term12 + B1 / N;
  B2 = term12 + B2 / N;
  B3 = fact3 * B3 / N;

  auto T = std::pow(1.0 / k, 0.5 * d + 1.0) / sigma;
  T *= 4 * H * N - pow(k, 0.5 * d) * (C1 * k + (B1 + B2) * std::sqrt(k) + B3);

  LOGINFO(OT::OSS() << "Y=" << Y << ", Z=" << Z << ", X=" << X << ", T=" << T
          << ", H=" << H << ", B1=" << B1 << ", B2=" << B2
          << ", B3=" << B3);

  return T;
}

double ContinuousTTest::getTTestWithoutCorrections(OT::UnsignedInteger Y,
    OT::UnsignedInteger Z,
    const OT::Indices &X) const
{
  OT::UnsignedInteger k = 0;

  OT::Point logFX, logFYX, logFZX, logFYZX;
  std::tie(logFX, logFYX, logFZX, logFYZX, k) = getLogPDFs(Y, Z, X);

  const auto d = X.getSize();
  const auto N = data_.getSize();

  const auto sigma = M_SQRT2 * std::pow(M_PI / 4, 0.5 * d + 1.0);

  double H = 0.0;

  if (d == 0)
    for (unsigned int i = 0; i < N; ++i)
      H += std::pow(std::expm1(-0.5 * logFYZX[i]), 2.0);
  else if (d == 1)
    for (unsigned int i = 0; i < N; ++i)
      H += std::pow(std::expm1(0.5 * logFYX[i] + logFZX[i] - logFYZX[i]), 2.0);
  else
    for (unsigned int i = 0; i < N; ++i)
      H += std::pow(
             std::expm1(0.5 * logFYX[i] + logFZX[i] - logFYZX[i] - logFX[i]), 2.0);
  const auto T =
    4 * H * N - pow(k, 0.5 * d) / std::pow(k, 0.5 * d + 1.0) / sigma;
  LOGINFO(OT::OSS() << "Y=" << Y << ", Z=" << Z << ", X=" << X << ", T=" << T
          << ", H=" << H);
  return T;
}

std::tuple<double, double, bool>
ContinuousTTest::isIndep(const OT::UnsignedInteger Y,
                         const OT::UnsignedInteger Z,
                         const OT::Indices &X) const
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

OT::Description ContinuousTTest::getDataDescription() const
{
  return data_.getDescription();
}
} // namespace OTAGRUM
