#!/usr/bin/env python

import os
import openturns as ot
import openturns.testing as ott
import otagrum


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

    skeleton = learner.learnSkeleton()
    assert skeleton.size() == 7
    skeleton_edges = {tuple(sorted(e)) for e in skeleton.edges()}
    for edge in [(0, 1), (2, 3), (2, 4), (5, 6)]:
        assert edge in skeleton_edges
    for u, v in skeleton_edges:
        assert learner.getPValue(u, v) < alpha
        assert learner.getTTest(u, v) > 0.0
    assert len(learner.getTrace()) > 0

    junction_tree = learner.learnJunctionTree()
    assert junction_tree.getSize() == 7

    dag = learner.learnDAG()
    assert dag.getSize() == 7
    assert list(dag.getDescription()) == list("ABCDEFG")
    # blocks [A,B], [C,D,E] and [F,G] should be recovered (up to orientation)
    dagImpl = dag.getDAG()
    for u, v in [(0, 1), (2, 3), (2, 4), (5, 6)]:
        assert dagImpl.existsArc(u, v) or dagImpl.existsArc(v, u)


def testAsiaDirichlet():
    data = ot.Sample.ImportFromTextFile(
        os.path.join(os.path.dirname(__file__), "asia_dirichlet_5000.csv"), ","
    )
    alpha = 0.1
    binNumber = 3
    learner = otagrum.ContinuousPC(data, binNumber, alpha)
    pdag = learner.learnPDAG()
    assert pdag.size() == 8
    dag = learner.learnDAG()
    assert dag.getSize() == 8
    assert sorted(list(dag.getDescription())) == sorted(
        ["asia", "tub", "smoke", "lung", "bronc", "either", "xray", "dysp"]
    )


def testEmptySampleRaises():
    with ott.assert_raises(TypeError):
        otagrum.ContinuousPC(ot.Sample())


testSpecificInstance()
testAsiaDirichlet()
testEmptySampleRaises()
