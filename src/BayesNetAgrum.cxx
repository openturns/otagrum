//                                               -*- C++ -*-
/**
 *  @file  BayesNetAgrum.cxx
 *  @brief BayesNetAgrum is a UserDefined factory based on Bayesian Networks, manipulated using aGrUM
 *
 *  (C) Copyright 2005-2007 EDF-EADS-Phimeca
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  @author: $LastChangedBy: dutka $
 *  @date:   $LastChangedDate: 2008-10-31 11:52:04 +0100 (Fri, 31 Oct 2008) $
 *  Id:      $Id: BayesNetAgrum.cxx 995 2008-10-31 10:52:04Z dutka $
 */
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "BayesNetAgrum.hxx"
#include "Os.hxx"
#include "Log.hxx"
#include "OSS.hxx"
#include "Path.hxx"
#include "GraphImplementation.hxx"
#include "Exception.hxx"
#include "UserDefined.hxx"
#include "Histogram.hxx"
#include "Description.hxx"
#include "NumericalPoint.hxx"
#include "Interval.hxx"
#include <agrum/multidim/discreteVariable.h>
#include <agrum/multidim/discretizedVariable.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/rangeVariable.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <agrum/core/exceptions.h>

namespace OpenTURNS {

  namespace Uncertainty {

    namespace Distribution {

      CLASSNAMEINIT(BayesNetAgrum);

      typedef gum::Instantiation                        Instantiation;
      typedef gum::DiscreteVariable                     DiscreteVariable;
      typedef gum::DiscretizedVariable<float>           DiscretizedVariable;
      typedef gum::LabelizedVariable                    LabelizedVariable;
      typedef gum::RangeVariable                        RangeVariable;
      typedef gum::NotFound                             NotFound;
      typedef gum::OutOfBounds                          OutOfBounds;
      typedef Base::Type::Description                   Description;
      typedef Base::Type::NumericalPoint                NumericalPoint;
      typedef Base::Type::Interval                      Interval;
      typedef Base::Common::Log                         Log;
      typedef Base::Common::Path                        Path;
      typedef Base::Common::Os                          Os;
      typedef Base::Common::InternalException           InternalException;
      typedef Base::Common::NotYetImplementedException  NotYetImplementedException;
      typedef Base::Common::InvalidArgumentException    InvalidArgumentException;

      /* Default constructor */
      BayesNetAgrum::BayesNetAgrum()
        : PersistentObject(),
          p_bayesNet_(0),
          p_inferenceAlgorithm_(0),
          isInferenceDone_(false),
          evidences_(),
          bifFileName_(),
          pngFileName_()
      {
        // Nothing to do
      }

      /* Parameter constructor from bif file */
      BayesNetAgrum::BayesNetAgrum(const FileName & bifFile)
        : PersistentObject(),
          p_bayesNet_(0),
          p_inferenceAlgorithm_(0),
          isInferenceDone_(false),
          evidences_(),
          bifFileName_(bifFile),
          pngFileName_()
      {
        // Convert the bif file if it contains any discretized variable
        OSS systemCommand;
        // First opening bracket
        systemCommand << "sed -i 's/{\\[/{_/g' " << bifFile << "\n";
        // Other opening brackets
        systemCommand << "sed -i 's/ \\[/ _/g' " << bifFile << "\n";
        // Last closing bracket
        systemCommand << "sed -i 's/\\]}/_}/g' " << bifFile << "\n";
        // Other closing brackets
        systemCommand << "sed -i 's/\\[,/_,/g' " << bifFile << "\n";
        // Protect end of line semi-columns
        systemCommand << "sed -i 's/;$/#/g' " << bifFile << "\n";
        // Convert other semi-columns
        systemCommand << "sed -i 's/;/_/g' " << bifFile << "\n";
        // Unprotect end of line semi-columns
        systemCommand << "sed -i 's/#$/;/g' " << bifFile << "\n";
        system(String(systemCommand).c_str());
        // Load the bif file
        // Create an empty BN
        BayesNetFloat *myBN = new BayesNetFloat();
        // First, read the bif file
        gum::BIFReader<float> reader( myBN, bifFile );
        reader.trace(true);
        // Here we read the file
        Bool isOk(reader.proceed());
        if (!isOk)
          {
            reader.showElegantErrorsAndWarnings();
            throw InvalidArgumentException(HERE) << "Error: unable to parse bif file " << bifFile;
          }
        p_bayesNet_ = myBN;
        p_inferenceAlgorithm_ = new LazyPropagation(*p_bayesNet_);
      }

      /* Parameter constructor from agrum bayesian network */
      BayesNetAgrum::BayesNetAgrum(const BayesNetFloat & bayesNet)
        : PersistentObject(),
          p_bayesNet_(new BayesNetFloat(bayesNet)),
          p_inferenceAlgorithm_(new LazyPropagation(*p_bayesNet_)),
          isInferenceDone_(false),
          evidences_(),
          bifFileName_(),
          pngFileName_()
      {
        // Nothing to do
      }

      /* Virtual constructor method */
      BayesNetAgrum * BayesNetAgrum::clone() const
      {
        return new BayesNetAgrum(*this);
      }

      /* Destructor */
      BayesNetAgrum::~BayesNetAgrum()
      {
        // Check if a BayesNet has been instanciated
        if (p_bayesNet_ != 0) delete p_bayesNet_;
        // Check if an inference algorithm has been instanciated
        if (p_inferenceAlgorithm_ != 0) delete p_inferenceAlgorithm_;
        // Check if there is any evidence to clear
        for (UnsignedInteger i = 0; i < evidences_.size(); ++i)
          delete evidences_[i];
      }

      // /* String converter */
      String BayesNetAgrum::__repr__() const
      {
        OSS oss;
        oss << "class=" << BayesNetAgrum::GetClassName()
            << " bayesian network=" << p_bayesNet_->toString()
            << " bif file=" << bifFileName_;
        return oss;
      }

      /* Draw a bitmap of the network */
      void BayesNetAgrum::draw(const FileName & fileName)
      {
        // First, build the dot file
        FileName temporaryFileName(Path::BuildTemporaryFileName("tmp_graph.R.XXXXXX") + ".dot");
        std::ofstream dotFile(temporaryFileName.c_str(), std::ios::out);
        dotFile << p_bayesNet_->toDot();
        dotFile.close();
        // Execute dot
        OSS systemCommand;
        systemCommand << "dot -Tpng \"" << temporaryFileName << "\" -o \"" << fileName << ".png\"" << Os::GetDeleteCommandOutput();
        if (system(String(systemCommand).c_str())) throw InternalException(HERE) << "BayesNetAgrum: error trying to execute dot command=" << String(systemCommand);
        if (remove(temporaryFileName.c_str()) == -1) Log::Warn(OSS() << "BayesNetAgrum: error trying to remove file " << temporaryFileName);
        pngFileName_= fileName + ".png";
      }

      /* Get the bitmap of the network */
      FileName BayesNetAgrum::getBitmap() const
      {
        return pngFileName_;
      }

      /* Evidence manipulators */
      /* For labelized variables */
      void BayesNetAgrum::setEvidence(const String & name,
                                      const String & value)
      {
        // Find the variable for which we have an evidence
        gum::NodeId nodeId;
        try
          {
            nodeId = p_bayesNet_->idFromName(name);
          }
        catch (NotFound & ex)
          {
            throw InvalidArgumentException(HERE) << "Error: the given name does not identify a node in the bayes net.";
          }
        // Build an evidence the GUM way
        // First, get the variable
        Potential * potential = new Potential();
        LabelizedVariable * p_variable(0);
	p_variable = new LabelizedVariable((static_cast<const LabelizedVariable &>(p_bayesNet_->variable(nodeId))));
        // Second, get the label associated with the value
        UnsignedInteger labelIndex;
        try
          {
            labelIndex = p_variable->operator[](value);
          }
        catch (OutOfBounds & ex)
          {
            throw InvalidArgumentException(HERE) << "Error: cannot find the value as a label of the variable. " << ex.getContent();
          }
        // Here, we can delete the variable
        if (p_variable != 0) delete p_variable;
        potential->add(p_bayesNet_->variable(nodeId));
        potential->fill(0);
        Instantiation instantiation( *potential );
        // Set the state of the variable to the one given by the evidence
        instantiation.chgVal(p_bayesNet_->variable(nodeId), labelIndex);
        // We set the probability to 1.0 for this state
        potential->set(instantiation, 1.0);
        // Insert the evidence into the inference algorithm
        gum::List< const Potential* > evidenceList;
        evidenceList.insert(potential);
        p_inferenceAlgorithm_->insertEvidence(evidenceList);
      }

      /* For range variables */
      void BayesNetAgrum::setEvidence(const String & name,
                                      const UnsignedInteger value)
      {
        // Find the variable for which we have an evidence
        gum::NodeId nodeId;
        try
          {
            nodeId = p_bayesNet_->idFromName(name);
          }
        catch (NotFound & ex)
          {
            throw InvalidArgumentException(HERE) << "Error: the given name does not identify a node in the bayes net.";
          }
        // Build an evidence the GUM way
        // First, get the variable
        Potential * potential = new Potential();
        RangeVariable * p_variable(0);
	p_variable = new RangeVariable((dynamic_cast<const RangeVariable &>(p_bayesNet_->variable(nodeId))));
        // Second, get the label associated with the value
        UnsignedInteger labelIndex;
        try
          {
            labelIndex = p_variable->operator[](String(OSS() << value));
          }
        catch (OutOfBounds & ex)
          {
            throw InvalidArgumentException(HERE) << "Error: cannot find the value as a label of the variable. " << ex.getContent();
          }
        // Here, we can delete the variable
        if (p_variable != 0) delete p_variable;
        potential->add(p_bayesNet_->variable(nodeId));
        potential->fill(0);
        Instantiation instantiation( *potential );
        // Set the state of the variable to the one given by the evidence
        instantiation.chgVal(p_bayesNet_->variable(nodeId), labelIndex);
        // We set the probability to 1.0 for this state
        potential->set(instantiation, 1.0);
        // Insert the evidence into the inference algorithm
        gum::List< const Potential* > evidenceList;
        evidenceList.insert(potential);
        p_inferenceAlgorithm_->insertEvidence(evidenceList);
      }

      /* For discretized variables */
      void BayesNetAgrum::setEvidence(const String & name,
                                      const NumericalScalar value)
      {
        // Find the variable for which we have an evidence
        gum::NodeId nodeId;
        try
          {
            nodeId = p_bayesNet_->idFromName(name);
          }
        catch (NotFound & ex)
          {
            throw InvalidArgumentException(HERE) << "Error: the given name does not identify a node in the bayes net.";
          }
        // Build an evidence the GUM way
        // First, get the variable
        Potential * potential = new Potential();
        DiscretizedVariable * p_variable(0);
	p_variable = new DiscretizedVariable((dynamic_cast<const DiscretizedVariable &>(p_bayesNet_->variable(nodeId))));
        // Second, get the label associated with the value
        UnsignedInteger labelIndex;
        try
          {
            labelIndex = p_variable->operator[](value);
          }
        catch (OutOfBounds & ex)
          {
            throw InvalidArgumentException(HERE) << "Error: cannot find the value as a label of the variable. " << ex.getContent();
          }
        // Here, we can delete the variable
        if (p_variable != 0) delete p_variable;
        potential->add(p_bayesNet_->variable(nodeId));
        potential->fill(0);
        Instantiation instantiation( *potential );
        // Set the state of the variable to the one given by the evidence
        instantiation.chgVal(p_bayesNet_->variable(nodeId), labelIndex);
        // We set the probability to 1.0 for this state
        potential->set(instantiation, 1.0);
        // Insert the evidence into the inference algorithm
        gum::List< const Potential* > evidenceList;
        evidenceList.insert(potential);
        p_inferenceAlgorithm_->insertEvidence(evidenceList);
      }

      /* Erase all the evidences */
      void BayesNetAgrum::eraseEvidences()
      {
        p_inferenceAlgorithm_->eraseAllEvidence();
      }

      /* Erase one evidence */
      void BayesNetAgrum::eraseEvidence(const String & name)
      {
	throw NotYetImplementedException(HERE);
      }

      /*
       * UserDefined distribution from evidence and marginal selection
       * In the 1D case, one can get either a UserDefined distribution
       * if the corresponding variable in the bayesian network is either
       * a labelized or a range variable, or an Histogram if the
       * underlying variable is a discretized variable.
       */
      BayesNetAgrum::DistributionImplementation * BayesNetAgrum::getMarginal(const String & marginal)
      {
        if (p_bayesNet_ == 0) throw InvalidArgumentException(HERE) << "Error: cannot make inference on null BayesNEt.";
        // Check the marginal
        // Get the marginal node and check if it is compatible with the Bayes Net
        gum::NodeId marginalId;
        try
          {
            marginalId = p_bayesNet_->idFromName(marginal);
          }
        catch (NotFound & ex)
          {
            throw InvalidArgumentException(HERE) << "Error: cannot found the marginal : " << ex.getContent();
          }
        // The inference algorithm, which compute a posteriori distributions from given bayesian network and evidence values
        // Make inference
        if (!isInferenceDone_)
          {
            p_inferenceAlgorithm_->makeInference();
            isInferenceDone_ = true;
          }
        // Extract the marginal distribution
        Instantiation instantiation(p_inferenceAlgorithm_->marginal(marginalId));
        // Downcast the marginal discrete variable. As the dynamic_cast refuses to work, use an interpretation of the labels for it:
        // If the labels start by [, it is a discretized variable: parse the labels to find the bounds and output an Histogram
        // If the labels can be casted into integers, it is a range variable and output a UserDefined variable
        // Else it is a labelized variable. Each label is replaced by its index and output a UserDefined variable
        UserDefined::PairCollection collectionUserDefined(0);
        Histogram::PairCollection collectionHistogram(0);
        NumericalScalar x0;
        UnsignedInteger modalityIndex(0);
        NumericalScalar sum(0.0);
        Bool isDiscretized(true);
        // Loop over the marginal labels
        for (instantiation.setFirst(); ! instantiation.end(); ++instantiation)
          {
            std::istringstream iss(p_bayesNet_->variable(marginalId).label(modalityIndex));
            String label(iss.str());
            // If the labels start by [, it is a discretized variable: parse the labels to find the bounds and output an Histogram
            // If the labels can be casted into integers, it is a range variable and output a UserDefined variable
            // Else it is a labelized variable. Each label is replaced by its index and output a UserDefined variable
            isDiscretized = isDiscretized && (label[0] == '[');
            // Discretized variables
            if (isDiscretized)
              {
                // Skip the first character
                char tmp;
                iss >> tmp;
                // Get the lower bound
                NumericalScalar xMin;
                iss >> xMin;
                // Skip the separator
                iss >> tmp;
                // Get the upper bound
                NumericalScalar xMax;
                iss >> xMax;
                // Store the base abscissa
                if (modalityIndex == 0) x0 = xMin;
                const NumericalScalar length(xMax - xMin);
                if (length <= 0.0) throw InternalException(HERE) << "Error: one of the classes of the marginal distribution is empty.";
                const NumericalScalar probability(p_inferenceAlgorithm_->marginal(marginalId).get(instantiation));
		if ((probability < 0.0) || (probability > 1.0)) throw InternalException(HERE) << "Error: the probability of " << label << " is not in [0, 1].";
                collectionHistogram.add(Histogram::Pair(probability / length, length));
              }
            // Labelized or range variables
            else
              {
                NumericalScalar value(0.0);
                if (!(iss >> value)) value = modalityIndex;
                const NumericalScalar probability(p_inferenceAlgorithm_->marginal(marginalId).get(instantiation));
                sum += probability;
                collectionUserDefined.add(UserDefined::Pair(NumericalPoint(1, value), probability));
              }
            ++modalityIndex;
          }
        DistributionImplementation * p_marginalDistribution;
        if (isDiscretized)
          {
            p_marginalDistribution = Histogram(x0, collectionHistogram).clone();
          }
        else
          {
            // Normalize the probabilities of non discretized variables as aGrUM works on float precision and OT on double precision
            for (UnsignedInteger i = 0; i < collectionUserDefined.getSize(); ++i)
              collectionUserDefined[i].setP(collectionUserDefined[i].getP() / sum);
            p_marginalDistribution = UserDefined(collectionUserDefined).clone();
          }
        return p_marginalDistribution;
      }

      /* Method save() stores the object through the StorageManager */
      void BayesNetAgrum::exportToBIFFile(const FileName & fileName) const
      {
	gum::BIFWriter<float> writer;
	writer.write(fileName, *p_bayesNet_);
      }

      /* Helper function to adapt a grid to a collection of distributions */
      NumericalPoint BayesNetAgrum::AdaptGrid(const DistributionCollection & distributionCollection,
					      const NumericalPoint & grid)
      {
	const UnsignedInteger collectionSize(distributionCollection.getSize());
	if (collectionSize == 0) throw InvalidArgumentException(HERE) << "Error: the collection of distributions is empty.";
	for (UnsignedInteger i = 0; i < collectionSize; ++i)
	  if (distributionCollection[i].getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: the distributions must be 1D, which is not the case for distribution number " << i;
	// Get the min and max values of the grid
	const NumericalScalar minGrid(*std::min_element(grid.begin(), grid.end()));
	const NumericalScalar maxGrid(*std::max_element(grid.begin(), grid.end()));
	// Get the minimal lower bound and maximal upper bounds of the distributions
	NumericalScalar lowerBound(distributionCollection[0].getRange().getLowerBound()[0]);
	NumericalScalar upperBound(distributionCollection[0].getRange().getUpperBound()[0]);
	for (UnsignedInteger i = 1; i < collectionSize; ++i)
	  {
	    const NumericalScalar currentLowerBound(distributionCollection[i].getRange().getLowerBound()[0]);
	    const NumericalScalar currentUpperBound(distributionCollection[i].getRange().getUpperBound()[0]);
	    if (currentLowerBound < lowerBound) lowerBound = currentLowerBound;
	    if (currentUpperBound > upperBound) upperBound = currentUpperBound;
	  }
	// Check if these bounds have to be included in the grid
	NumericalPoint result(grid);
	if (lowerBound < minGrid) result.add(lowerBound);
	if (upperBound > maxGrid) result.add(upperBound);
	// Sort the grid
	std::sort(result.begin(), result.end());
	// That's all, folks!
	return result;
      }

      /* Helper function to discretize a continuous distribution */
      std::vector<float> BayesNetAgrum::Discretize(const DistributionImplementation & distribution,
						   const NumericalPoint & grid,
						   const Bool isSorted)
      {
	return Discretize(Distribution(distribution), grid, isSorted);
      }

      /* Helper function to discretize a continuous distribution */
      std::vector<float> BayesNetAgrum::Discretize(const Distribution & distribution,
						   const NumericalPoint & grid,
						   const Bool isSorted)
      {
	if (distribution.getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot discretize a distribution with dimension > 1.";
	if (grid.getDimension() < 1) throw InvalidArgumentException(HERE) << "Error: cannot discretize a distribution with less than 2 ticks.";
	const UnsignedInteger gridSize(grid.getDimension());
	NumericalPoint sortedGrid(grid);
	if (!isSorted) std::sort(sortedGrid.begin(), sortedGrid.end());
	std::vector<float> result(gridSize - 1);
	NumericalScalar sum(0.0);
	for (UnsignedInteger i = 0; i < gridSize - 1; ++i)
	  {
	    result[i] = distribution.computeProbability(Interval(sortedGrid[i], sortedGrid[i + 1]));
	    sum += result[i];
	  }
	if (sum < 1.0 - sqrt(DistributionImplementation::DefaultCDFEpsilon)) throw InternalException(HERE) << "Error: the discretization is not adapted to the distribution. There is a mass leakage of " << 1.0 - sum;
	// Normalize the discretization to take into account roundoff errors
	for (UnsignedInteger i = 0; i < gridSize - 1; ++i)
	  result[i] /= sum;
	return result;
      }

      /* Helper function to discretize a collection of continuous distributions */
      std::vector<float> BayesNetAgrum::Discretize(const DistributionCollection & distributionCollection,
						   const NumericalPoint & grid,
						   const Bool isSorted)
      {
	const UnsignedInteger collectionSize(distributionCollection.getSize());
	if (collectionSize == 0) throw InvalidArgumentException(HERE) << "Error: the collection of distributions is empty.";
	for (UnsignedInteger i = 0; i < collectionSize; ++i)
	  if (distributionCollection[i].getDimension() != 1) throw InvalidArgumentException(HERE) << "Error: cannot discretize a distribution with dimension > 1.";
	if (grid.getDimension() < 1) throw InvalidArgumentException(HERE) << "Error: cannot discretize a distribution with less than 2 ticks.";
	const UnsignedInteger gridSize(grid.getDimension());
	NumericalPoint sortedGrid(grid);
	if (!isSorted) std::sort(sortedGrid.begin(), sortedGrid.end());
	std::vector<float> result((gridSize - 1) * collectionSize);
	UnsignedInteger index(0);
	for (UnsignedInteger i = 0; i < collectionSize; ++i)
	  {
	    const std::vector<float> currentResult(Discretize(distributionCollection[i], sortedGrid, true));
	    for (UnsignedInteger j = 0; j < gridSize - 1; ++j)
	      {
		result[index] = currentResult[j];
		++index;
	      }
	  }
	return result;
      }

    } /* namespace Distribution */
  } /* namespace Uncertainty */
} /* namespace OpenTURNS */
