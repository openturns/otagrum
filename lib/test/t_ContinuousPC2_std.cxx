#include <iostream>

#include <openturns/ClaytonCopulaFactory.hxx>
#include <openturns/BlockIndependentCopula.hxx>
#include <openturns/FrankCopulaFactory.hxx>
#include <openturns/NormalCopulaFactory.hxx>
#include <openturns/RandomGenerator.hxx>

#include "otagrum/otagrum.hxx"

OT::Description DescrFromStringVect(const std::vector<std::string> &v)
{
  OT::Description res;
  for (auto s : v)
  {
    res.add(s);
  }
  return res;
}
/*
0--1 : p-value=0
2--3 : p-value=0.750422
1--4 : p-value=0.933081
2--4 : p-value=6.18139e-35
3--4 : p-value=3.61185e-182
*/
OT::Sample getTrucBizarre(OT::UnsignedInteger size)
{
  OT::Collection<OT::Distribution> copulas;

  OT::UnsignedInteger dim = 3;

  OT::CorrelationMatrix R(dim);
  for (OT::UnsignedInteger i = 0; i < dim; i++)
    for (OT::UnsignedInteger j = 0; j < i; j++)
      R(i, j) = 0.6;

  auto copula1 = OT::FrankCopula(4.0);
  copula1.setDescription(DescrFromStringVect({"A1", "A2"}));
  auto copula2 = OT::ClaytonCopula(3.0);
  copula2.setDescription(DescrFromStringVect({"B1", "B2"}));

  copulas.add(copula1);
  copulas.add(copula2);
  copulas.add(OT::NormalCopula(R));

  auto copula = OT::BlockIndependentCopula(copulas);
  return copula.getSample(size);
}

void tests(void)
{
  try
  {
    OT::RandomGenerator::SetSeed(0);
    OT::Sample sample = getTrucBizarre(5000);
    std::cout << "Sample size : " << sample.getSize() << std::endl
              << "Sample dimension :" << sample.getDimension() << std::endl
              << "Sample " << sample << std::endl;
    {
      OTAGRUM::ContinuousPC2 learner(sample, 3, 0.1);
      std::cout << "go" << std::endl;
      auto skel = learner.learnSkeleton();
      std::cout << learner.skeletonToDot(skel) << std::endl;

      auto pdag = learner.learnPDAG();
      std::cout << pdag.toDot() << std::endl;
      for (const auto &e : skel.edges())
      {
        std::cout << e
                  << " : p-value=" << learner.getPValue(e.first(), e.second())
                  << "   ttest=" << learner.getTTest(e.first(), e.second())
                  << std::endl;
      }
      for (const auto &s : learner.getTrace())
        std::cout << s << std::endl;
    }
    {
      std::cout << "\n\n";
      OTAGRUM::ContinuousPC2 learner(sample, 5, 0.8);
      std::cout << "go" << std::endl;
      auto skel = learner.learnSkeleton();
      std::cout << "\n****\n"
                << "skeleton"
                << "\n****\n"
                << skel.toDot() << std::endl;
      auto pdag = learner.learnPDAG();
      std::cout << "\n****\n"
                << "PDAG"
                << "\n****\n"
                << pdag.toDot() << std::endl;

      auto njt = learner.learnJunctionTree();
      std::cout << "\n****\n"
                << "Named Junction tree"
                << "\n****\n"
                << njt.__str__() << std::endl;
      auto ndag = learner.learnDAG();
      std::cout << "\n****\n"
                << "Named DAG"
                << "\n****\n"
                << ndag.__str__() << std::endl;
      std::cout << ndag.toDot() << std::endl;

      for (const auto &s : learner.getTrace())
        std::cout << s << std::endl;
    }
  }
  catch (OT::Exception &e)
  {
    std::cout << e.__repr__() << std::endl;
  }
  catch (gum::Exception &e)
  {
    GUM_SHOWERROR(e);
  }
};

void testJulien()
{
  const auto data = OT::Sample::ImportFromCSVFile("correlated_sample.csv");
  //const auto n_nodes = data.getDimension();
  const auto desc = data.getDescription();
  std::cout << desc << std::endl;

  try
  {
    auto learner = OTAGRUM::ContinuousPC2(data, 3, 0.1);
    auto pdag = learner.learnPDAG();
    std::cout << "pdag:" << pdag << std::endl;
    auto dag = learner.learnDAG();
    std::cout << "dag:" << dag << std::endl;
  }
  catch (gum::Exception &e)
  {
    GUM_SHOWERROR(e);
  }
}

void testMathis()
{
  OT::RandomGenerator::SetSeed(0);

  // Create the structure
  gum::DAG dag;
  std::map<std::string, gum::NodeId> nodes;
  std::vector<std::string> names = {"A", "B", "C", "D", "E", "F", "G"};
  for (const auto& name : names)
  {
    nodes[name] = dag.addNode();
  }
  dag.addArc(nodes["B"], nodes["D"]);
  dag.addArc(nodes["B"], nodes["A"]);
  dag.addArc(nodes["B"], nodes["C"]);
  dag.addArc(nodes["A"], nodes["C"]);
  dag.addArc(nodes["A"], nodes["E"]);
  dag.addArc(nodes["F"], nodes["A"]);
  dag.addArc(nodes["F"], nodes["G"]);
  OTAGRUM::NamedDAG structure(dag, names);
  
  // Create the marginal distributions
  OT::Collection<OT::Distribution> m_list;
  int nb_of_nodes = dag.sizeNodes();
  std::cout << "Number of nodes: " << nb_of_nodes << std::endl;
  for (OT::UnsignedInteger i = 0; i < nb_of_nodes; i++)
  {
    m_list.add(OT::Uniform(0.0, 1.0));
  }
  
  // Create the local conditional copulas
  OT::Collection<OT::Distribution> lcc_list;
  // #PA=2, #PB=0, #PC=2, #PD=1, #PE=1, #PF=0, #PG=1
  std::vector<OT::UnsignedInteger> parent_counts = {2, 0, 2, 1, 1, 0, 1};
  for (OT::UnsignedInteger i = 0; i < nb_of_nodes; i++)
  {
    OT::UnsignedInteger dim_lcc = parent_counts[i] + 1;
    OT::CorrelationMatrix R(dim_lcc);
    for (OT::UnsignedInteger j = 0; j < dim_lcc; j++)
      for (OT::UnsignedInteger k = 0; k < j; k++)
        R(j, k) = 0.6;
    
    lcc_list.add(OT::Normal(OT::Point(dim_lcc, 0.0), OT::Point(dim_lcc, 1.0), R).getCopula());
  }
  
  // Create the CBN
  OTAGRUM::ContinuousBayesianNetwork cbn(structure, m_list, lcc_list);
  
  // Sample from the CBN
  OT::Sample sample = cbn.getSample(5000);
  
  std::cout << "Sample size : " << sample.getSize() << std::endl
            << "Sample dimension : " << sample.getDimension() << std::endl
            << "Description : " << sample.getDescription() << std::endl;
  
  try
  {
      std::cout << "\n\n";
      OTAGRUM::ContinuousPC2 learner(sample, 5, 0.8);
      std::cout << "go" << std::endl;

      auto skel = learner.learnSkeleton();
      std::cout << "\n****\n"
                << "skeleton"
                << "\n****\n"
                << skel.toDot() << std::endl;

      auto pdag = learner.learnPDAG();
      std::cout << "\n****\n"
                << "PDAG"
                << "\n****\n"
                << pdag.toDot() << std::endl;

      auto cpdag = learner.learnCPDAG();
      std::cout << "\n****\n"
                << "CPDAG"
                << "\n****\n"
                << cpdag.toDot() << std::endl;

      auto ndag = learner.learnDAG();
      std::cout << "\n****\n"
                << "Named DAG"
                << "\n****\n"
                << ndag.__str__() << std::endl;
      std::cout << ndag.toDot() << std::endl;
  }
  catch (gum::Exception &e)
  {
    GUM_SHOWERROR(e);
  }
}

int main(void)
{
//   OT::Log::Show(OT::Log::ALL);
  tests();

  testJulien();

  testMathis();

  return EXIT_SUCCESS;
}
