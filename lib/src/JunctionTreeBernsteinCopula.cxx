//                                               -*- C++ -*-
/**
 *  @brief The JunctionTreeBernsteinCopula distribution
 *
 *
 *  Copyright 2010-2019 Airbus-LIP6-Phimeca
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

#include <cmath>
#include "otagrum/JunctionTreeBernsteinCopula.hxx"

#include <openturns/OSS.hxx>
#include <openturns/Exception.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/SpecFunc.hxx>
#include <openturns/DistFunc.hxx>
#include <openturns/Uniform.hxx>
#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace OTAGRUM
{

CLASSNAMEINIT(JunctionTreeBernsteinCopula);

static const Factory<JunctionTreeBernsteinCopula> Factory_JunctionTreeBernsteinCopula;

/* Default constructor */
JunctionTreeBernsteinCopula::JunctionTreeBernsteinCopula()
  : ContinuousDistribution()
  , junctionTree_()
  , cliquesCollection_(0)
  , separatorsCollection_(0)
  , copulaSample_(2, 1)
  , binNumber_(1)
  , logBetaMarginalFactors_(0, 1)
  , logFactors_(0, 1)
{
  setName( "JunctionTreeBernsteinCopula" );
  setCopulaSample(copulaSample_, false);
}

/* Parameters constructor */
JunctionTreeBernsteinCopula::JunctionTreeBernsteinCopula(const NamedJunctionTree & junctionTree,
    const Sample & copulaSample,
    const UnsignedInteger binNumber,
    const Bool isCopulaSample)
  : ContinuousDistribution()
  , junctionTree_(junctionTree)
  , cliquesCollection_(0)
  , separatorsCollection_(junctionTree.getSeparatorsCollection())
  , copulaSample_(copulaSample)
  , binNumber_(binNumber)
  , logBetaMarginalFactors_(0, 1)
  , logFactors_(0, 1)
{
  setName( "JunctionTreeBernsteinCopula" );
  setCopulaSample(copulaSample, isCopulaSample);
  setCliquesOrder(junctionTree.getOrderMaxFirst());
}

/* Parameters constructor */
JunctionTreeBernsteinCopula::JunctionTreeBernsteinCopula(const NamedJunctionTree & junctionTree,
    const Indices & cliquesOrder,
    const Sample & copulaSample,
    const UnsignedInteger binNumber,
    const Bool isCopulaSample)
  : ContinuousDistribution()
  , junctionTree_(junctionTree)
  , cliquesCollection_(0)
  , separatorsCollection_(junctionTree.getSeparatorsCollection())
  , copulaSample_(copulaSample)
  , binNumber_(binNumber)
  , logBetaMarginalFactors_(0, 1)
  , logFactors_(0, 1)
{
  setName( "JunctionTreeBernsteinCopula" );
  setCopulaSample(copulaSample, isCopulaSample);
  setCliquesOrder(cliquesOrder);
}

/* Set the order according to which the cliques are traversed */
void JunctionTreeBernsteinCopula::setCliquesOrder(const Indices & cliquesOrder)
{
  const unsigned int size = cliquesOrder.getSize();
  if (size != junctionTree_.getCliquesCollection().getSize()) throw InvalidArgumentException(HERE) << "Error: expected a cliques order of size=" << junctionTree_.getCliquesCollection().getSize() << ", got size=" << size;
  cliquesCollection_ = IndicesPersistentCollection(size);
  for (unsigned int i = 0; i < size; ++i)
    cliquesCollection_[i] = junctionTree_.getClique(cliquesOrder[i]);
}

/* Copula sample accessor */
void JunctionTreeBernsteinCopula::setCopulaSample(const Sample & copulaSample,
    const Bool isEmpiricalCopulaSample)
{
  // Check the sample
  const UnsignedInteger size = copulaSample.getSize();
  if (size <= 1) throw InvalidArgumentException(HERE) << "Error: expected a sample of size>1.";
  const UnsignedInteger dimension = copulaSample.getDimension();
  if (dimension == 0) throw InvalidArgumentException(HERE) << "Error: expected a sample of dimension>0.";
  const UnsignedInteger remainder = size % binNumber_;
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
      LOGINFO(OSS() << "Must drop the last " << remainder << " to build a JunctionTreeBernsteinCopula as the given sample has a size=" << size << " which is not a multiple of the bin number=" << binNumber_);
      copulaSample_ = Sample(copulaSample, 0, size - remainder).rank();
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

Sample JunctionTreeBernsteinCopula::getCopulaSample() const
{
  return copulaSample_;
}

/* BinNumber accessors */
void JunctionTreeBernsteinCopula::setBinNumber(const UnsignedInteger binNumber)
{
  binNumber_ = binNumber;
}

UnsignedInteger JunctionTreeBernsteinCopula::getBinNumber() const
{
  return binNumber_;
}

/* Compute the normalization factors */
void JunctionTreeBernsteinCopula::update()
{
  const UnsignedInteger size = copulaSample_.getSize();
  const UnsignedInteger dimension = copulaSample_.getDimension();
  logBetaMarginalFactors_ = SampleImplementation(size, dimension);
  logFactors_ = SampleImplementation(size, dimension);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    for (UnsignedInteger j = 0; j < dimension; ++j)
    {
      const Scalar xIJ = copulaSample_(i, j);
      const Scalar r = ceil(binNumber_ * xIJ);
      const Scalar s = binNumber_ - r + 1.0;
      const Scalar logBeta = SpecFunc::LogBeta(r, s);
      logBetaMarginalFactors_(i, j) = logBeta;
      logFactors_(i, j) = r;
    } // j
  } // i
}


/* Comparison operator */
Bool JunctionTreeBernsteinCopula::operator ==(const JunctionTreeBernsteinCopula & other) const
{
  if (this == &other) return true;
  return (cliquesCollection_ == other.cliquesCollection_) && (separatorsCollection_ == other.separatorsCollection_) && (copulaSample_ == other.copulaSample_) && (binNumber_ == other.binNumber_);
}

Bool JunctionTreeBernsteinCopula::equals(const DistributionImplementation & other) const
{
  const JunctionTreeBernsteinCopula* p_other = dynamic_cast<const JunctionTreeBernsteinCopula*>(&other);
  return p_other && (*this == *p_other);
}

/* String converter */
String JunctionTreeBernsteinCopula::__repr__() const
{
  OSS oss(true);
  oss << "class=" << JunctionTreeBernsteinCopula::GetClassName()
      << " name=" << getName()
      << " dimension=" << getDimension()
      << " junctionTree=" << junctionTree_
      << " cliquesCollection=" << cliquesCollection_
      << " separatorsCollection=" << separatorsCollection_
      << " sample=" << copulaSample_
      << " k=" << binNumber_;
  return oss;
}

String JunctionTreeBernsteinCopula::__str__(const String & offset) const
{
  OSS oss(false);
  oss << offset << getClassName() << "(junction tree=" << junctionTree_ << ", sample=\n" << copulaSample_ << ", k=" << binNumber_ << ")";
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
  setRange(Interval(copulaSample_.getDimension()));
}

/* Get one realization of the distribution */
Point JunctionTreeBernsteinCopula::getRealization() const
{
  const UnsignedInteger dimension = getDimension();
  if ((dimension == 1) && isCopula_) return Point(1, RandomGenerator::Generate());
  const UnsignedInteger size = copulaSample_.getSize();
  Collection<UnsignedInteger> generatedFlags(dimension, 0);
  Point realization(dimension);
  for (UnsignedInteger cliqueIndex = 0; cliqueIndex < cliquesCollection_.getSize(); ++cliqueIndex)
  {
    const Indices clique(cliquesCollection_[cliqueIndex]);
    // Get the components of the clique that have already been simulated
    Point knownValues(0);
    Indices knownPositions(0);
    Indices unknownPositions(0);
    const UnsignedInteger cliqueSize = clique.getSize();
    for (UnsignedInteger cliqueComponent = 0; cliqueComponent < cliqueSize; ++cliqueComponent)
    {
      const UnsignedInteger index(clique[cliqueComponent]);
      if (generatedFlags[index] == 1)
      {
        knownValues.add(realization[index]);
        knownPositions.add(index);
      }
      else unknownPositions.add(index);
    } // cliqueComponent
    // If no known positions, no need to buid the conditional copula
    UnsignedInteger atomIndex = size;
    if (knownPositions.getSize() == 0) atomIndex = RandomGenerator::IntegerGenerate(size);
    else
    {
      // Create the elements of the conditional Bernstein copula
      // Weights
      Point weights(size, 1.0);
      const UnsignedInteger knownSize = knownPositions.getSize();
      Point logX(knownSize);
      Point log1mX(knownSize);
      for (UnsignedInteger knownComponent = 0; knownComponent < knownSize; ++knownComponent)
      {
        logX[knownComponent] = std::log(knownValues[knownComponent]);
        log1mX[knownComponent] = log1p(-knownValues[knownComponent]);
      } // knownComponent
      Scalar totalWeight = 0.0;
      for (UnsignedInteger i = 0; i < size; ++i)
      {
        Scalar currentLogFactor = 0.0;
        for (UnsignedInteger knownComponent = 0; knownComponent < knownSize; ++knownComponent)
        {
          const UnsignedInteger knownIndex = knownPositions[knownComponent];
          currentLogFactor += logBetaMarginalFactors_(i, knownIndex) + (logFactors_(i, knownIndex) - 1.0) * logX[knownComponent] + (binNumber_ - logFactors_(i, knownIndex)) * log1mX[knownComponent];
        } // knownComponent
        weights[i] *= std::exp(currentLogFactor);
        totalWeight += weights[i];
      } // i
      weights *= (size / totalWeight);
      atomIndex = DistFunc::rDiscrete(weights);
    } // knownPositions.getSize() > 0
    // Simulate the conditional Bernstein copula
    for (UnsignedInteger unknownComponent = 0; unknownComponent < unknownPositions.getSize(); ++unknownComponent)
    {
      const UnsignedInteger unknownIndex = unknownPositions[unknownComponent];
      realization[unknownIndex] = DistFunc::rBeta(logFactors_(atomIndex, unknownIndex), binNumber_ - logFactors_(atomIndex, unknownIndex) + 1.0);
      generatedFlags[unknownIndex] = 1;
    } // unknownComponent
  } // cliqueIndex
  return realization;
}

/* Get the PDF of the distribution */
Scalar JunctionTreeBernsteinCopula::computePDF(const Point & point) const
{
  const UnsignedInteger dimension = getDimension();
  if (point.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the given point must have dimension=" << dimension << ", here dimension=" << point.getDimension();
  if ((dimension == 1) && isCopula_) return (point[0] <= 0.0 ? 0.0 : (point[0] > 1.0 ? 0.0 : 1.0));
  for (UnsignedInteger i = 0; i < dimension; ++i)
    // Here we have to exclude both bounds in order to avoid NaNs
    if ((point[i] <= 0.0) || (point[i] >= 1.0)) return 0.0;
  Scalar pdfValue = 1.0;
  Point logX(dimension);
  Point log1mX(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i)
  {
    logX[i] = std::log(point[i]);
    log1mX[i] = log1p(-point[i]);
  }
  // Precompute some multiplications
  const UnsignedInteger size = copulaSample_.getSize();
  Sample logPDFAtoms(logBetaMarginalFactors_);
  for (UnsignedInteger i = 0; i < size; ++i)
    for (UnsignedInteger j = 0; j < dimension; ++j)
      logPDFAtoms(i, j) += (logFactors_(i, j) - 1.0) * logX[j] + (binNumber_ - logFactors_(i, j)) * log1mX[j];
  // For each clique, multiply the PDF value
  for (UnsignedInteger n = 0; n < cliquesCollection_.getSize(); ++n)
  {
    const Indices clique(cliquesCollection_[n]);
    const UnsignedInteger cliqueSize = clique.getSize();
    Scalar cliquePDFValue = 0.0;
    // For the current clique, compute the PDF of the associated beta mixture
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      Scalar logPDFAtom = 0.0;
      // For each component of the clique, accumulate the log-PDF
      for (UnsignedInteger k = 0; k < cliqueSize; ++k)
        logPDFAtom += logPDFAtoms(i, clique[k]);
      cliquePDFValue += std::exp(logPDFAtom);
    } // i
    cliquePDFValue /= size;
    pdfValue *= cliquePDFValue;
  } // n
  // For each separator, divide the PDF value
  for (UnsignedInteger n = 0; n < separatorsCollection_.getSize(); ++n)
  {
    const Indices separator(separatorsCollection_[n]);
    const UnsignedInteger separatorSize = separator.getSize();
    Scalar separatorPDFValue = 0.0;
    // For the current separator, compute the PDF of the associated beta mixture
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      Scalar logPDFAtom = 0.0;
      // For each component of the separator, accumulate the log-PDF
      for (UnsignedInteger k = 0; k < separatorSize; ++k)
        logPDFAtom += logPDFAtoms(i, separator[k]);
      separatorPDFValue += std::exp(logPDFAtom);
    } // i
    separatorPDFValue /= size;
    pdfValue /= separatorPDFValue;
  } // n
  return pdfValue;
}


/* Get the i-th marginal distribution */
Distribution JunctionTreeBernsteinCopula::getMarginal(const UnsignedInteger i) const
{
  if (i >= getDimension()) throw InvalidArgumentException(HERE) << "The index of a marginal distribution must be in the range [0, dim-1]";
  if (isCopula_) return Uniform(0.0, 1.0);
  return getMarginal(Indices(1, i));
}

/* Get the distribution of the marginal distribution corresponding to indices dimensions */
Distribution JunctionTreeBernsteinCopula::getMarginal(const Indices & indices) const
{
  if (isCopula_ && (indices.getSize() == 1)) return Uniform(0.0, 1.0);
  return JunctionTreeBernsteinCopula(junctionTree_.getMarginal(indices), copulaSample_.getMarginal(indices), binNumber_, true);
}


/* Method save() stores the object through the StorageManager */
void JunctionTreeBernsteinCopula::save(Advocate & adv) const
{
  ContinuousDistribution::save(adv);
  adv.saveAttribute( "cliquesCollection_", cliquesCollection_ );
  adv.saveAttribute( "separatorsCollection_", separatorsCollection_ );
  adv.saveAttribute( "copulaSample_", copulaSample_ );
  adv.saveAttribute( "binNumber_", binNumber_ );
  adv.saveAttribute( "logBetaMarginalFactors_", logBetaMarginalFactors_ );
  adv.saveAttribute( "logFactors_", logFactors_ );
}

/* Method load() reloads the object from the StorageManager */
void JunctionTreeBernsteinCopula::load(Advocate & adv)
{
  ContinuousDistribution::load(adv);
  adv.loadAttribute( "cliquesCollection_", cliquesCollection_ );
  adv.loadAttribute( "separatorsCollection_", separatorsCollection_ );
  adv.loadAttribute( "copulaSample_", copulaSample_ );
  adv.loadAttribute( "binNumber_", binNumber_ );
  adv.loadAttribute( "logBetaMarginalFactors_", logBetaMarginalFactors_ );
  adv.loadAttribute( "logFactors_", logFactors_ );
  computeRange();
}

} /* namespace OTAGR */
