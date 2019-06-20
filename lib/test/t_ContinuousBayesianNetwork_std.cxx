#include <openturns/OT.hxx>

#include "otagrum/NamedDAG.hxx"
#include "otagrum/ContinuousBayesianNetwork.hxx"
#include <iostream>

void testConstructor() {
  const auto proto = "A->B->C->D;E->A->C<-E";
  const auto bn = gum::BayesNet<double>::fastPrototype(proto);
  std::cout << "      proto : " << proto << std::endl;
  std::cout << "         BN : " << bn << std::endl << std::endl;
  OTAGRUM::NamedDAG ndag(bn);
  std::cout << "       size : " << ndag.getSize() << std::endl;
  std::cout << "       desc : " << ndag.getDescription() << std::endl
            << std::endl;
  std::cout << "      nodes : " << ndag.getNodes() << std::endl;
  for (const auto &nod : ndag.getNodes())
    {
      std::cout << " parents(" << nod << ") : " << ndag.getParents(nod) << std::endl;
      std::cout << "children(" << nod << ") : " << ndag.getChildren(nod) << std::endl;
    }

  OT::Indices order = ndag.getTopologicalOrder();
  std::cout << "topologicalOrder : " << order << std::endl;
  OT::Collection<OT::Distribution> jointDistributions(order.getSize());
  for (OT::UnsignedInteger i = 0; i < order.getSize(); ++i)
    {
      const OT::UnsignedInteger d = 1 + ndag.getParents(i).getSize();
      std::cout << "i=" << i << ", d=" << d << std::endl;
#ifdef USE_NORMAL_COPULA_AND_GUMBEL_COPULA
      if (d == 2) jointDistributions[i] = OT::GumbelCopula(5.0);
      else*
        {
          OT::CorrelationMatrix R(d);
          for (OT::UnsignedInteger i = 0; i < d; ++i)
            for (OT::UnsignedInteger j = 0; j < i; ++j)
              R(i, j) = 0.5 / d;
          jointDistributions[i] = OT::NormalCopula(R);
        }
#else
      OT::Point theta(d + 1);
      for (OT::UnsignedInteger i = 0; i <= d; ++i)
        theta[i] = std::sqrt((1.0 + i) / d);
      jointDistributions[i] = OT::Dirichlet(theta).getCopula();
#endif
      //jointDistributions[i] = OT::KernelSmoothing().build(OT::NormalCopula(R).getSample(10)).getCopula();
    }
  OTAGRUM::ContinuousBayesianNetwork cbn(ndag, jointDistributions);
  std::cout << "cbn=" << cbn << std::endl;
  std::cout << "cbn pdf=" << cbn.computePDF(OT::Point(cbn.getDimension(), 0.5)) << std::endl;
  std::cout << "cbn realization=" << cbn.getRealization() << std::endl;
  const OT::UnsignedInteger size = 10000;
  const OT::Sample sampleLearn(cbn.getSample(size));
  const OT::Sample sample(cbn.getSample(size));

  sample.exportToCSVFile("samplelearn.csv", ",");
  sample.exportToCSVFile("sample.csv", ",");

  std::cout << "cbn sample=" << sample << std::endl;
  OT::Scalar logL = 0.0;
  for (OT::UnsignedInteger i = 0; i < size; ++i)
    logL += std::log(cbn.computePDF(sample[i]));
  logL /= size;
  std::cout << "log-l=" << logL << std::endl;
  OT::NormalCopula copula(OT::NormalCopulaFactory().buildAsNormalCopula(sampleLearn));
  std::cout << "copula=" << copula << std::endl;
  std::cout << "log-l=" << copula.computeLogPDF(sample).computeMean()[0] << std::endl;
  {
    OT::Graph gr;
    OT::Pairs pairs(sample);
    pairs.setPointStyle("dot");
    gr.add(pairs);
    gr.draw("pairs.png", 800, 820);
  }
  {
    OT::Graph gr;
    OT::Pairs pairs(copula.getSample(size));
    pairs.setPointStyle("dot");
    gr.add(pairs);
    gr.draw("pairsCopula.png", 800, 820);
  }
}

int main(void) {
  testConstructor();

  return 0;
}
