//                                               -*- C++ -*-
/**
 *  @brief The JunctionTreeBernsteinCopula distribution
 *
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
#include "otagrum/JunctionTreeBernsteinCopula.hxx"

#include <openturns/OSS.hxx>
#include <openturns/Exception.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/SpecFunc.hxx>
#include <openturns/DistFunc.hxx>
#include <openturns/RegularGrid.hxx>
#include <openturns/UserDefined.hxx>
#include <openturns/PersistentObjectFactory.hxx>


namespace OTAGRUM
{

CLASSNAMEINIT(JunctionTreeBernsteinCopula);

static const OT::Factory<JunctionTreeBernsteinCopula> Factory_JunctionTreeBernsteinCopula;

/* Default constructor */
JunctionTreeBernsteinCopula::JunctionTreeBernsteinCopula()
  : OT::ContinuousDistribution()
  , cliquesCollection_(0)
  , separatorsCollection_(0)
  , copulaSample_(1, 1)
  , binNumber_(1)
  , logBetaMarginalFactors_(0, 1)
  , logFactors_(0, 1)
{
  setName( "JunctionTreeBernsteinCopula" );
  setCopulaSample(copulaSample_, true);
}

/* Parameters constructor */
JunctionTreeBernsteinCopula::JunctionTreeBernsteinCopula(const JunctionTree & junctionTree,
    const OT::Sample & copulaSample,
    const OT::UnsignedInteger binNumber,
    const OT::Bool isCopulaSample)
  : OT::ContinuousDistribution()
  , cliquesCollection_(junctionTree.getCliquesCollection())
  , separatorsCollection_(junctionTree.getSeparatorsCollection())
  , copulaSample_(copulaSample)
  , binNumber_(binNumber)
  , logBetaMarginalFactors_(0, 1)
  , logFactors_(0, 1)
{
  setName( "JunctionTreeBernsteinCopula" );
  setCopulaSample(copulaSample, isCopulaSample);
}

JunctionTreeBernsteinCopula::JunctionTreeBernsteinCopula(const IndicesCollection & cliquesCollection,
    const IndicesCollection & separatorsCollection,
    const OT::Sample & copulaSample,
    const OT::UnsignedInteger binNumber,
    const OT::Bool isCopulaSample)
  : OT::ContinuousDistribution()
  , cliquesCollection_(cliquesCollection)
  , separatorsCollection_(separatorsCollection)
  , copulaSample_(copulaSample)
  , binNumber_(binNumber)
  , logBetaMarginalFactors_(0, 1)
  , logFactors_(0, 1)
{
  setName( "JunctionTreeBernsteinCopula" );
  setCopulaSample(copulaSample, isCopulaSample);
}

/* Copula sample accessor */
void JunctionTreeBernsteinCopula::setCopulaSample(const OT::Sample & copulaSample,
    const OT::Bool isEmpiricalCopulaSample)
{
  // Check the sample
  const OT::UnsignedInteger size = copulaSample.getSize();
  if (size <= 1) throw OT::InvalidArgumentException(HERE) << "Error: expected a sample of size>1.";
  const OT::UnsignedInteger dimension = copulaSample.getDimension();
  if (dimension == 0) throw OT::InvalidArgumentException(HERE) << "Error: expected a sample of dimension>0.";
  const OT::UnsignedInteger remainder = size % binNumber_;
  // If the given sample is an empirical copula sample of a compatible size
  if (isEmpiricalCopulaSample)
  {
    isCopula_ = (remainder == 0);
    copulaSample_ = copulaSample;
  }
  else
  {
    isCopula_ = true;
    if (remainder == 0)
      copulaSample_ = copulaSample.rank();
    else
    {
      LOGINFO(OT::OSS() << "Must drop the last " << remainder << " to build a JunctionTreeBernsteinCopula as the given sample has a size=" << size << " which is not a multiple of the bin number=" << binNumber_);
      copulaSample_ = OT::Sample(copulaSample, 0, size - remainder).rank();
    }
    // Normalize
    copulaSample_ += 1.0;
    copulaSample_ /= 1.0 * (size - remainder);
  } // !(isEmpiricalCopulaSample && remainder == 0)
  setDimension(dimension);
  // Now the sample is correct, compute the by-products
  update();
  computeRange();
}

OT::Sample JunctionTreeBernsteinCopula::getCopulaSample() const
{
  return copulaSample_;
}

/* BinNumber accessors */
void JunctionTreeBernsteinCopula::setBinNumber(const OT::UnsignedInteger binNumber)
{
  binNumber_ = binNumber;
}

OT::UnsignedInteger JunctionTreeBernsteinCopula::getBinNumber() const
{
  return binNumber_;
}

/* Compute the normalization factors */
void JunctionTreeBernsteinCopula::update()
{
  const OT::UnsignedInteger size = copulaSample_.getSize();
  const OT::UnsignedInteger dimension = copulaSample_.getDimension();
  logBetaMarginalFactors_ = OT::SampleImplementation(size, dimension);
  logFactors_ = OT::SampleImplementation(size, dimension);
  for (OT::UnsignedInteger i = 0; i < size; ++i)
  {
    for (OT::UnsignedInteger j = 0; j < dimension; ++j)
    {
      const OT::Scalar xIJ = copulaSample_(i, j);
      const OT::Scalar r = ceil(binNumber_ * xIJ);
      const OT::Scalar s = binNumber_ - r + 1.0;
      const OT::Scalar logBeta = OT::SpecFunc::LogBeta(r, s);
      logBetaMarginalFactors_(i, j) = logBeta;
      logFactors_(i, j) = r;
    } // j
  } // i
}


/* Comparison operator */
OT::Bool JunctionTreeBernsteinCopula::operator ==(const JunctionTreeBernsteinCopula & other) const
{
  if (this == &other) return true;
  return (cliquesCollection_ == other.cliquesCollection_) && (separatorsCollection_ == other.separatorsCollection_) && (copulaSample_ == other.copulaSample_) && (binNumber_ == other.binNumber_);
}

OT::Bool JunctionTreeBernsteinCopula::equals(const DistributionImplementation & other) const
{
  const JunctionTreeBernsteinCopula* p_other = dynamic_cast<const JunctionTreeBernsteinCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
OT::String JunctionTreeBernsteinCopula::__repr__() const
{
  OT::OSS oss(true);
  oss << "class=" << JunctionTreeBernsteinCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " cliquesCollection=" << cliquesCollection_
      << " separatorsCollection=" << separatorsCollection_
      << " sample=" << copulaSample_
      << " k=" << binNumber_;
  return oss;
}

OT::String JunctionTreeBernsteinCopula::__str__(const OT::String & offset) const
{
  OT::OSS oss(false);
  oss << offset << getClassName() << "(cliques=" << cliquesCollection_ << ", separators=" << separatorsCollection_ << ", sample=\n" << copulaSample_ << ", k=" << binNumber_ << ")";
  return oss;
}

/* Virtual constructor */
JunctionTreeBernsteinCopula * JunctionTreeBernsteinCopula::clone() const
{
  return new JunctionTreeBernsteinCopula(*this);
}

/* Compute the numerical range of the distribution given the parameters values */
void JunctionTreeBernsteinCopula::computeRange()
{
  setRange(OT::Interval(copulaSample_.getDimension()));
}

/* Get one realization of the distribution */
OT::Point JunctionTreeBernsteinCopula::getRealization() const
{
  const OT::UnsignedInteger size = copulaSample_.getSize();
  const OT::UnsignedInteger dimension = getDimension();
  OT::Collection<OT::UnsignedInteger> generatedFlags(dimension, 0);
  OT::Point realization(dimension);
  for (OT::UnsignedInteger cliqueIndex = 0; cliqueIndex < cliquesCollection_.getSize(); ++cliqueIndex)
  {
    const OT::Indices clique(cliquesCollection_[cliqueIndex]);
    // Get the components of the clique that have already been simulated
    OT::Point knownValues(0);
    OT::Indices knownPositions(0);
    OT::Indices unknownPositions(0);
    const OT::UnsignedInteger cliqueSize = clique.getSize();
    for (OT::UnsignedInteger cliqueComponent = 0; cliqueComponent < cliqueSize; ++cliqueComponent)
    {
      const OT::UnsignedInteger index(clique[cliqueComponent]);
      if (generatedFlags[index] == 1)
      {
        knownValues.add(realization[index]);
        knownPositions.add(index);
      }
      else unknownPositions.add(index);
    } // cliqueComponent
    // If no known positions, no need to buid the conditional copula
    OT::UnsignedInteger atomIndex = size;
    if (knownPositions.getSize() == 0) atomIndex = OT::RandomGenerator::IntegerGenerate(size);
    else
    {
      // Create the elements of the conditional Bernstein copula
      // Weights
      OT::Point weights(size, 1.0);
      const OT::UnsignedInteger knownSize = knownPositions.getSize();
      OT::Point logX(knownSize);
      OT::Point log1mX(knownSize);
      for (OT::UnsignedInteger knownComponent = 0; knownComponent < knownSize; ++knownComponent)
      {
        logX[knownComponent] = std::log(knownValues[knownComponent]);
        log1mX[knownComponent] = log1p(-knownValues[knownComponent]);
      } // knownComponent
      OT::Scalar totalWeight = 0.0;
      for (OT::UnsignedInteger i = 0; i < size; ++i)
      {
        OT::Scalar currentLogFactor = 0.0;
        for (OT::UnsignedInteger knownComponent = 0; knownComponent < knownSize; ++knownComponent)
        {
          const OT::UnsignedInteger knownIndex = knownPositions[knownComponent];
          currentLogFactor += logBetaMarginalFactors_(i, knownIndex) + (logFactors_(i, knownIndex) - 1.0) * logX[knownComponent] + (binNumber_ - logFactors_(i, knownIndex)) * log1mX[knownComponent];
        } // knownComponent
        weights[i] *= std::exp(currentLogFactor);
        totalWeight += weights[i];
      } // i
      weights *= (size / totalWeight);
      atomIndex = OT::DistFunc::rDiscrete(weights);
    } // knownPositions.getSize() > 0
    // Simulate the conditional Bernstein copula
    for (OT::UnsignedInteger unknownComponent = 0; unknownComponent < unknownPositions.getSize(); ++unknownComponent)
    {
      const OT::UnsignedInteger unknownIndex = unknownPositions[unknownComponent];
      realization[unknownIndex] = OT::DistFunc::rBeta(logFactors_(atomIndex, unknownIndex), binNumber_ - logFactors_(atomIndex, unknownIndex) + 1.0);
      generatedFlags[unknownIndex] = 1;
    } // unknownComponent
  } // cliqueIndex
  return realization;
}

/* Get the PDF of the distribution */
OT::Scalar JunctionTreeBernsteinCopula::computePDF(const OT::Point & point) const
{
  const OT::UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw OT::InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  for (OT::UnsignedInteger i = 0; i < dimension; ++i)
    if ((point[i] <= 0.0) || (point[i] >= 1.0)) return 0.0;
  OT::Scalar pdfValue = 1.0;
  OT::Point logX(dimension);
  OT::Point log1mX(dimension);
  for (OT::UnsignedInteger i = 0; i < dimension; ++i)
  {
    logX[i] = std::log(point[i]);
    log1mX[i] = log1p(-point[i]);
  }
  // Precompute some multiplications
  const OT::UnsignedInteger size = copulaSample_.getSize();
  OT::Sample logPDFAtoms(logBetaMarginalFactors_);
  for (OT::UnsignedInteger i = 0; i < size; ++i)
    for (OT::UnsignedInteger j = 0; j < dimension; ++j)
      logPDFAtoms(i, j) += (logFactors_(i, j) - 1.0) * logX[j] + (binNumber_ - logFactors_(i, j)) * log1mX[j];
  // For each clique, multiply the PDF value
  for (OT::UnsignedInteger n = 0; n < cliquesCollection_.getSize(); ++n)
  {
    const OT::Indices clique(cliquesCollection_[n]);
    const OT::UnsignedInteger cliqueSize = clique.getSize();
    OT::Scalar cliquePDFValue = 0.0;
    // For the current clique, compute the PDF of the associated beta mixture
    for (OT::UnsignedInteger i = 0; i < size; ++i)
    {
      OT::Scalar logPDFAtom = 0.0;
      // For each component of the clique, accumulate the log-PDF
      for (OT::UnsignedInteger k = 0; k < cliqueSize; ++k)
        logPDFAtom += logPDFAtoms(i, clique[k]);
      cliquePDFValue += std::exp(logPDFAtom);
    } // i
    cliquePDFValue /= size;
    pdfValue *= cliquePDFValue;
  } // n
  // For each separator, divide the PDF value
  for (OT::UnsignedInteger n = 0; n < separatorsCollection_.getSize(); ++n)
  {
    const OT::Indices separator(separatorsCollection_[n]);
    const OT::UnsignedInteger separatorSize = separator.getSize();
    OT::Scalar separatorPDFValue = 0.0;
    // For the current separator, compute the PDF of the associated beta mixture
    for (OT::UnsignedInteger i = 0; i < size; ++i)
    {
      OT::Scalar logPDFAtom = 0.0;
      // For each component of the separator, accumulate the log-PDF
      for (OT::UnsignedInteger k = 0; k < separatorSize; ++k)
        logPDFAtom += logPDFAtoms(i, separator[k]);
      separatorPDFValue += std::exp(logPDFAtom);
    } // i
    separatorPDFValue /= size;
    pdfValue /= separatorPDFValue;
  } // n
  return pdfValue;
}


/* Method save() stores the object through the StorageManager */
void JunctionTreeBernsteinCopula::save(OT::Advocate & adv) const
{
  OT::ContinuousDistribution::save(adv);
  adv.saveAttribute( "cliquesCollection_", cliquesCollection_ );
  adv.saveAttribute( "separatorsCollection_", separatorsCollection_ );
  adv.saveAttribute( "copulaSample_", copulaSample_ );
  adv.saveAttribute( "binNumber_", binNumber_ );
  adv.saveAttribute( "logBetaMarginalFactors_", logBetaMarginalFactors_ );
  adv.saveAttribute( "logFactors_", logFactors_ );
}

/* Method load() reloads the object from the StorageManager */
void JunctionTreeBernsteinCopula::load(OT::Advocate & adv)
{
  OT::ContinuousDistribution::load(adv);
  adv.loadAttribute( "cliquesCollection_", cliquesCollection_ );
  adv.loadAttribute( "separatorsCollection_", separatorsCollection_ );
  adv.loadAttribute( "copulaSample_", copulaSample_ );
  adv.loadAttribute( "binNumber_", binNumber_ );
  adv.loadAttribute( "logBetaMarginalFactors_", logBetaMarginalFactors_ );
  adv.loadAttribute( "logFactors_", logFactors_ );
  computeRange();
}

} /* namespace OTAGR */
