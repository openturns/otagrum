#include <iostream>

#include <openturns/OT.hxx>

#include "otagrum/otagrum.hxx"

void testConstructor()
{
  OT::ResourceMap::SetAsBool("Distribution-Parallel", false);
  const auto proto = "A->B->C->D;E->A->C<-E";
  const auto bn = gum::BayesNet<double>::fastPrototype(proto);
  std::cout << "      proto : " << proto << std::endl;
  std::cout << "         BN : " << bn << std::endl << std::endl;
  OTAGRUM::NamedDAG ndag(bn);
  std::cout << "       size : " << ndag.getSize() << std::endl;
  std::cout << "       desc : " << ndag.getDescription() << std::endl
            << std::endl;
  for (const auto &nod : ndag.getTopologicalOrder())
  {
    std::cout << " parents(" << nod << ") : " << ndag.getParents(nod)
              << std::endl;
    std::cout << "children(" << nod << ") : " << ndag.getChildren(nod)
              << std::endl;
  }

  OT::Indices order = ndag.getTopologicalOrder();
  std::cout << "topologicalOrder : " << order << std::endl;
  OT::Collection<OT::Distribution> marginals(order.getSize(), OT::Uniform(0.0, 1.0));
  OT::Collection<OT::Distribution> copulas(order.getSize());

  for (OT::UnsignedInteger i2 = 0; i2 < order.getSize(); ++i2)
  {
    const OT::UnsignedInteger d = 1 + ndag.getParents(i2).getSize();
    std::cout << "i=" << i2 << ", d=" << d << std::endl;
    if (d == 1) copulas[i2] = OT::IndependentCopula(1);
    else
      {
        OT::CorrelationMatrix R(d);
        for (OT::UnsignedInteger i = 0; i < d; ++i)
          for (OT::UnsignedInteger j = 0; j < i; ++j)
            R(i, j) = 0.5 / d;
        copulas[i2] =
          OT::Student(5.0, OT::Point(d), OT::Point(d, 1.0), R).getCopula();
      }
  }

  OTAGRUM::ContinuousBayesianNetwork cbn(ndag, marginals, copulas);
  std::cout << "cbn=" << cbn << std::endl;
  std::cout << "cbn pdf=" << cbn.computePDF(OT::Point(cbn.getDimension(), 0.5))
            << std::endl;
  std::cout << "cbn realization=" << cbn.getRealization() << std::endl;
}
void genereData()
{
  const auto proto = "A->B->C->D;E->A->C<-E";
  const auto bn = gum::BayesNet<double>::fastPrototype(proto);
  OTAGRUM::NamedDAG ndag(bn);
  OT::Indices order = ndag.getTopologicalOrder();
  OT::Collection<OT::Distribution> marginals(order.getSize(), OT::Uniform(0.0, 1.0));
  OT::Collection<OT::Distribution> copulas(order.getSize());

  for (OT::UnsignedInteger i2 = 0; i2 < order.getSize(); ++i2)
  {
    const OT::UnsignedInteger d = 1 + ndag.getParents(i2).getSize();
    std::cout << "i=" << i2 << ", d=" << d << std::endl;
    if (d == 1) copulas[i2] = OT::IndependentCopula(1);
    else
      {
        OT::CorrelationMatrix R(d);
        for (OT::UnsignedInteger i = 0; i < d; ++i)
          for (OT::UnsignedInteger j = 0; j < i; ++j)
            R(i, j) = 0.5 / d;
        copulas[i2] =
          OT::Student(5.0, OT::Point(d), OT::Point(d, 1.0), R).getCopula();
      }
  }

  OTAGRUM::ContinuousBayesianNetwork cbn(ndag, marginals, copulas);
  std::cout << "cbn=" << cbn << std::endl;
  std::cout << "cbn pdf=" << cbn.computePDF(OT::Point(cbn.getDimension(), 0.5))
            << std::endl;
  std::cout << "cbn realization=" << cbn.getRealization() << std::endl;
  const OT::UnsignedInteger size = 1000;
  const OT::Sample sampleLearn(cbn.getSample(size));
  const OT::Sample sample(cbn.getSample(size));

  sampleLearn.exportToCSVFile("samplelearn.csv", ",");
  sample.exportToCSVFile("sample.csv", ",");

  std::cout << "cbn sample=" << sample << std::endl;
  OT::Scalar logL = 0.0;
  OT::Sample pdfSample(cbn.computePDF(sample));
  pdfSample.exportToCSVFile("pdfSample.csv", ",");
  for (OT::UnsignedInteger i = 0; i < size; ++i)
    logL += std::log(pdfSample(i, 0));
  logL /= size;
  std::cout << "log-l=" << logL << std::endl;
  OT::NormalCopula copula(
    OT::NormalCopulaFactory().buildAsNormalCopula(sampleLearn));
  std::cout << "copula=" << copula << std::endl;
  std::cout << "log-l=" << copula.computeLogPDF(sample).computeMean()[0]
            << std::endl;
  /*
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
  }*/
}

int main(void)
{
  testConstructor();

  return 0;
}
