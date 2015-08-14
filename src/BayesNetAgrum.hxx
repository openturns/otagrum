//                                               -*- C++ -*-
/**
 *  @file  BayesNetAgrum.hxx
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
 *  Id:      $Id: BayesNetAgrumImplementation.hxx 995 2008-10-31 10:52:04Z dutka $
 */
#ifndef BAYESNETAGRUM_BAYESNETAGRUM_HXX
#define BAYESNETAGRUM_BAYESNETAGRUM_HXX

#include <vector>

#include "OTprivate.hxx"
#include "PersistentObject.hxx"
#include "Collection.hxx"
#include "NumericalPoint.hxx"
#include "Distribution.hxx"
#include "DistributionImplementation.hxx"
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>

namespace OpenTURNS {

  namespace Uncertainty {

    namespace Distribution {

      /**
       * @class BayesNetAgrum
       *
       * BayesNetAgrum is a UserDefined factory based on Bayesian Networks, manipulated using aGrUM
       * It can be used to perform elementary operations on Bayesian Networks, such as drawing the network, or to perform more elaborate operations: set evidences, perform inference, extract marginal distributions or multivariate discrete joint distributions
       */
      class BayesNetAgrum: public Base::Common::PersistentObject
      {
        CLASSNAME;

      public:
        typedef gum::BayesNet<float>                      BayesNetFloat;
        typedef gum::LazyPropagation<float>               LazyPropagation;
        typedef gum::Potential<float>                     Potential;
        typedef gum::List< const Potential* >             EvidenceList;
        typedef Base::Common::PersistentObject            PersistentObject;
        typedef Base::Common::StorageManager              StorageManager;
        typedef Base::Type::NumericalPoint                NumericalPoint;
        typedef Model::Distribution                       Distribution;
        typedef Model::DistributionImplementation         DistributionImplementation;
        typedef Base::Type::Collection<Distribution>      DistributionCollection;

        /** Default constructor */
        BayesNetAgrum();

        /** Parameter constructor from bif file */
        BayesNetAgrum(const FileName & bifFile);

        /** Parameter constructor from agrum bayesian network */
        BayesNetAgrum(const BayesNetFloat & bayesNet);

        /** Virtual constructor method */
        BayesNetAgrum * clone() const;

        /** Destructor */
        ~BayesNetAgrum();

        /** Evidence manipulators */
        /** For labelized variables */
        void setEvidence(const String & name,
                         const String & value);

        /** For range variables */
        void setEvidence(const String & name,
                         const UnsignedInteger value);

        /** For discretized variables */
        void setEvidence(const String & name,
                         const NumericalScalar value);

        /** Erase all the evidences */
        void eraseEvidences();

        /** Erase one evidence */
        void eraseEvidence(const String & name);

        /** UserDefined distribution from evidence and marginal selection */
        DistributionImplementation * getMarginal(const String & marginal);

        /** String converter */
        String __repr__() const;

        /** Draw a bitmap of the network */
        void draw(const FileName & fileName);

        /** Get the bitmap of the network */
        FileName getBitmap() const;

        /** Export to BIF format */
        void exportToBIFFile(const FileName & fileName) const;

        /** Helper function to discretize a continuous distribution */
        static NumericalPoint AdaptGrid(const DistributionCollection & distributionCollection,
                                        const NumericalPoint & grid);

        /** Helper function to discretize a continuous distribution */
        static std::vector<float> Discretize(const Distribution & distribution,
                                             const NumericalPoint & grid,
                                             const Bool isSorted = false);

        /** Helper function to discretize a continuous distribution */
        static std::vector<float> Discretize(const DistributionImplementation & distribution,
                                             const NumericalPoint & grid,
                                             const Bool isSorted = false);

        /** Helper function to discretize a continuous distribution */
        static std::vector<float> Discretize(const DistributionCollection & distributionCollection,
                                             const NumericalPoint & grid,
                                             const Bool isSorted = false);

      private:
        /** The underlying Bayesian Network */
        BayesNetFloat * p_bayesNet_;

        /** The algorithm used to perform inference */
        LazyPropagation * p_inferenceAlgorithm_;

        /** Flag to tell if the inference has already been done */
        mutable Bool isInferenceDone_;

        /** Agrum notion of evidence */
        EvidenceList evidences_;

        /** The bif file where to save the network */
        FileName bifFileName_;

        /** The png file associated with the drawing of the network */
        FileName pngFileName_;

      }; /* class BayesNetAgrum */

    } /* namespace Distribution */
  } /* namespace Uncertainty */
} /* namespace OpenTURNS */


#endif /* BAYESNETAGRUM_BAYESNETAGRUM_HXX */
