#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include <openturns/ClaytonCopulaFactory.hxx>
#include <openturns/BlockIndependentCopula.hxx>
#include <openturns/FrankCopulaFactory.hxx>
#include <openturns/NormalCopulaFactory.hxx>
#include <openturns/RandomGenerator.hxx>

#include "otagrum/otagrum.hxx"

namespace
{
using IdEdge = std::pair<OT::UnsignedInteger, OT::UnsignedInteger>;
using NameEdge = std::pair<std::string, std::string>;

IdEdge orderedEdge(OT::UnsignedInteger a, OT::UnsignedInteger b)
{
  return (b < a) ? IdEdge(b, a) : IdEdge(a, b);
}

NameEdge orderedEdge(const std::string &a, const std::string &b)
{
  return (b < a) ? NameEdge(b, a) : NameEdge(a, b);
}

void check(bool ok, const std::string &msg)
{
  if (!ok)
    throw std::runtime_error("check failed: " + msg);
}

void checkClose(double value, double expected, double rtol, const std::string &msg)
{
  if (!(std::abs(value - expected) <= rtol * (1.0 + std::abs(expected))))
  {
    std::ostringstream oss;
    oss << "checkClose failed: " << msg << " (got " << value << ", expected " << expected << ")";
    throw std::runtime_error(oss.str());
  }
}

std::set<IdEdge> skeletonEdges(const gum::UndiGraph &skeleton)
{
  std::set<IdEdge> edges;
  for (const auto &edge : skeleton.edges())
    edges.insert(orderedEdge(edge.first(), edge.second()));
  return edges;
}

std::set<NameEdge> namedSkeleton(const OTAGRUM::NamedDAG &ndag)
{
  std::set<NameEdge> edges;
  const OT::Description desc = ndag.getDescription();
  const gum::DAG dag = ndag.getDAG();
  for (const auto &arc : dag.arcs())
    edges.insert(orderedEdge(desc[arc.tail()], desc[arc.head()]));
  return edges;
}
} // namespace

OT::Description DescrFromStringVect(const std::vector<std::string> &v)
{
  OT::Description res;
  for (auto s : v)
  {
    res.add(s);
  }
  return res;
}

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
  OT::RandomGenerator::SetSeed(0);
  OT::Sample sample = getTrucBizarre(5000);
  check(sample.getSize() == 5000, "sample size");
  check(sample.getDimension() == 7, "sample dimension");
  {
    OTAGRUM::ContinuousPC learner(sample, 3, 0.1);
    const std::set<IdEdge> expected = {{0, 1}, {2, 3}, {4, 5}, {4, 6}, {5, 6}};
    check(skeletonEdges(learner.learnSkeleton()) == expected, "skeleton");
    checkClose(learner.getTTest("A1", "A2"), 67.3106, 1e-4, "ttest A1-A2");
    check(learner.getPValue("A1", "A2") == 0.0, "pvalue A1-A2");
    checkClose(learner.getTTest("B1", "B2"), 121.91, 1e-4, "ttest B1-B2");
    check(learner.getPValue("B1", "B2") == 0.0, "pvalue B1-B2");
    checkClose(learner.getTTest("X0", "X1"), 61.0624, 1e-4, "ttest X0-X1");
    checkClose(learner.getPValue("X0", "X1"), 1.02025e-20, 1e-4, "pvalue X0-X1");
    checkClose(learner.getTTest("X0", "X2"), 64.8518, 1e-4, "ttest X0-X2");
    checkClose(learner.getPValue("X0", "X2"), 1.56447e-22, 1e-4, "pvalue X0-X2");
    checkClose(learner.getTTest("X1", "X2"), 58.0097, 1e-4, "ttest X1-X2");
    checkClose(learner.getPValue("X1", "X2"), 5.28577e-16, 1e-4, "pvalue X1-X2");
  }
  {
    OTAGRUM::ContinuousPC learner(sample, 5, 0.8);
    const std::set<IdEdge> expected = {{0, 1}, {2, 3}, {4, 5}, {4, 6}, {5, 6}};
    check(skeletonEdges(learner.learnSkeleton()) == expected, "skeleton");
    const std::set<NameEdge> expectedDag = {{"A1", "A2"}, {"B1", "B2"}, {"X0", "X1"}, {"X0", "X2"}, {"X1", "X2"}};
    check(namedSkeleton(learner.learnDAG()) == expectedDag, "dag");
    check(learner.learnJunctionTree().getSize() == 7, "junction tree size");
  }
}

void testJulien()
{
  const auto data = OT::Sample::ImportFromCSVFile("correlated_sample.csv");
  OTAGRUM::ContinuousPC learner(data, 3, 0.1);
  const gum::MixedGraph pdag = learner.learnPDAG();
  check(pdag.arcs().size() == 0, "pdag arcs");
  std::set<IdEdge> pdagEdges;
  for (const auto &edge : pdag.edges())
    pdagEdges.insert(orderedEdge(edge.first(), edge.second()));
  const std::set<IdEdge> expectedEdges = {{6, 9}, {7, 11}, {5, 9}, {0, 9}, {2, 7}, {8, 10}, {11, 12}};
  check(pdagEdges == expectedEdges, "pdag skeleton");
  const std::set<NameEdge> expectedDag = {{"x1", "y13"}, {"x3", "y11"}, {"x6", "y13"}, {"x7", "y13"}, {"y11", "y22"}, {"y12", "y21"}, {"y22", "y31"}};
  check(namedSkeleton(learner.learnDAG()) == expectedDag, "dag");
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
  OT::UnsignedInteger nb_of_nodes = dag.sizeNodes();
  check(nb_of_nodes == 7, "node number");
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
  check(sample.getSize() == 5000, "sample size");
  check(sample.getDimension() == 7, "sample dimension");

  OTAGRUM::ContinuousPC learner(sample, 5, 0.8);
  const std::set<IdEdge> expectedSkel = {{0, 1}, {0, 2}, {0, 4}, {0, 5}, {1, 2}, {1, 3}, {5, 6}};
  check(skeletonEdges(learner.learnSkeleton()) == expectedSkel, "skeleton");
  const auto ndag = learner.learnDAG();
  check(ndag.getSize() == 7, "dag size");
  const std::set<NameEdge> expectedDag = {{"A", "B"}, {"A", "C"}, {"A", "E"}, {"A", "F"}, {"B", "C"}, {"B", "D"}, {"F", "G"}};
  check(namedSkeleton(ndag) == expectedDag, "dag");
}

int main(void)
{
  try
  {
    tests();
    testJulien();
    testMathis();
  }
  catch (const std::exception &e)
  {
    std::cout << "FAIL: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch (...)
  {
    std::cout << "FAIL: unknown exception" << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "All ContinuousPC checks passed" << std::endl;
  return EXIT_SUCCESS;
}
