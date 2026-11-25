#!/usr/bin/env python

import os
import re
import openturns as ot
import openturns.testing as ott
import otagrum


def parseDotSkeleton(dot):
    """Undirected edge set parsed from a dot graph.

    String comparison of dot outputs is unstable (edge order and
    Markov-equivalent orientations vary), only the skeleton is asserted.
    """
    edges = set()
    for head, tail in re.findall(r'"([^"]+)"\s*->\s*"([^"]+)"', dot):
        edges.add(frozenset((head, tail)))
    return edges


def parseDotIdSkeleton(dot, names):
    """Undirected named edge set parsed from an id-based dot graph."""
    edges = set()
    for head, tail in re.findall(r"(\d+)\s*->\s*(\d+)", dot):
        edges.add(frozenset((names[int(head)], names[int(tail)])))
    return edges


def generateDataForSpecificInstance(size):
    R = ot.CorrelationMatrix(3)
    R[0, 1] = 0.5
    R[0, 2] = 0.45
    collection = [ot.FrankCopula(3.0), ot.NormalCopula(R), ot.ClaytonCopula(2.0)]
    copula = ot.BlockIndependentCopula(collection)
    copula.setDescription("ABCDEFG")
    return copula.getSample(size)


def testSpecificInstance():
    ot.RandomGenerator.SetSeed(0)
    size = 1000
    data = generateDataForSpecificInstance(size)
    alpha = 0.1
    binNumber = 3
    learner = otagrum.ContinuousPC(data, binNumber, alpha)

    dag = learner.learnDAG()
    assert dag.getSize() == 7
    # block structure of the generator: {A, B}, {C, D, E}, {F, G}
    trueEdges = {
        frozenset(("A", "B")),
        frozenset(("C", "D")),
        frozenset(("C", "E")),
        frozenset(("D", "E")),
        frozenset(("F", "G")),
    }
    learned = parseDotSkeleton(dag.toDot())
    # most true edges are recovered whatever the realization
    assert len(learned & trueEdges) >= 3, learned


def testAsiaDirichlet():
    data = ot.Sample.ImportFromTextFile(
        os.path.join(os.path.dirname(__file__), "asia_dirichlet_5000.csv"), ","
    )
    names = [str(name) for name in data.getDescription()]
    alpha = 0.1
    binNumber = 3
    learner = otagrum.ContinuousPC(data, binNumber, alpha)
    learner.setVerbosity(True)
    pdag = learner.learnPDAG()
    expectedSkeleton = {
        frozenset(("dysp", "either")),
        frozenset(("dysp", "bronc")),
        frozenset(("either", "tub")),
        frozenset(("either", "lung")),
        frozenset(("either", "xray")),
        frozenset(("lung", "smoke")),
        frozenset(("smoke", "bronc")),
        frozenset(("tub", "asia")),
    }
    # node ids of the pdag follow the column order of the data
    assert parseDotIdSkeleton(learner.PDAGtoDot(pdag), names) == expectedSkeleton
    dag = learner.learnDAG()
    assert dag.getSize() == 8
    assert parseDotSkeleton(dag.toDot()) == expectedSkeleton
    # test statistics of pairs tested a single time
    ott.assert_almost_equal(learner.getTTest("tub", "either"), 9.24, 1e-2)
    assert learner.getPValue("tub", "either") < 1e-15
    ott.assert_almost_equal(learner.getTTest("asia", "tub"), 9.76, 1e-2)
    assert learner.getPValue("asia", "tub") < 1e-15


def testEmptySampleRaises():
    with ott.assert_raises(TypeError):
        otagrum.ContinuousPC(ot.Sample())


testSpecificInstance()
testAsiaDirichlet()
testEmptySampleRaises()
