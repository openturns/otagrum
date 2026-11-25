#!/usr/bin/env python

import os
import re
import openturns as ot
import openturns.testing as ott
import otagrum
import pyagrum as gum


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
    learner = otagrum.ContinuousMIIC(data)

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
    learner = otagrum.ContinuousMIIC(data)
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
    assert parseDotIdSkeleton(pdag.toDot(), names) == expectedSkeleton
    dag = learner.learnDAG()
    assert dag.getSize() == 8
    assert parseDotSkeleton(dag.toDot()) == expectedSkeleton
    # alpha accessor roundtrip
    learner.setAlpha(0.05)
    ott.assert_almost_equal(learner.getAlpha(), 0.05, 1e-12)


def testMathis():
    # extreme case to show that cmiic apply meek rule 3 correctly
    dag = gum.DAG()
    dag.addNodes(4)
    for t, h in [(0, 1), (2, 1), (2, 3)]:
        dag.addArc(t, h)

    var_names = [f"X{i}" for i in range(4)]
    named_dag = otagrum.NamedDAG(dag, var_names)

    marginals = [ot.Uniform(0.0, 1.0) for _ in range(4)]

    rho = 0.8
    local_copulas = []
    for i in range(4):
        n_parents = named_dag.getParents(i).getSize()
        dim = n_parents + 1
        R = ot.CorrelationMatrix(dim)
        for j in range(dim):
            for k in range(j):
                R[j, k] = rho
        local_copulas.append(ot.NormalCopula(R))

    cbn = otagrum.ContinuousBayesianNetwork(named_dag, marginals, local_copulas)

    ot.RandomGenerator.SetSeed(42)
    sample = cbn.getSample(100)

    bn = gum.BayesNet()
    for nid in dag.nodes():
        bn.add(gum.LabelizedVariable(f"X{nid}", f"X{nid}", 2))
    for nid in dag.nodes():
        for cid in dag.children(nid):
            bn.addArc(nid, cid)

    learner = otagrum.ContinuousMIIC(sample)
    learner.setAlpha(0.01)
    learner.setVerbosity(False)

    learner.learnPDAG()  # smoke test

    learned_named_dag = learner.learnDAG()
    assert learned_named_dag.getSize() == 4
    learned = parseDotSkeleton(learned_named_dag.toDot())
    # true skeleton must be recovered (extra edges allowed at n=100)
    trueEdges = {
        frozenset(("X0", "X1")),
        frozenset(("X1", "X2")),
        frozenset(("X2", "X3")),
    }
    assert trueEdges <= learned, learned

    learned_dag = learned_named_dag.getDAG()

    bn2 = gum.BayesNet()
    for nid in learned_dag.nodes():
        bn2.add(gum.LabelizedVariable(f"X{nid}", f"X{nid}", 2))
    for t, h in learned_dag.arcs():
        bn2.addArc(t, h)

    # smoke test: essential graph of the learned dag can be built
    gum.EssentialGraph(bn2).pdag()


def testEmptySampleRaises():
    with ott.assert_raises(TypeError):
        otagrum.ContinuousMIIC(ot.Sample())


testSpecificInstance()
testAsiaDirichlet()
testMathis()
testEmptySampleRaises()
