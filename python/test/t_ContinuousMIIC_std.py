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
    learner = otagrum.ContinuousMIIC(data)

    skeleton = learner.learnSkeleton()
    assert skeleton.size() == 7
    skeleton_edges = {tuple(sorted(e)) for e in skeleton.edges()}
    for edge in [(0, 1), (2, 3), (2, 4), (5, 6)]:
        assert edge in skeleton_edges

    dag = learner.learnDAG()
    assert dag.getSize() == 7
    assert list(dag.getDescription()) == list("ABCDEFG")


def testAsiaDirichlet():
    data = ot.Sample.ImportFromTextFile(
        os.path.join(os.path.dirname(__file__), "asia_dirichlet_5000.csv"), ","
    )
    learner = otagrum.ContinuousMIIC(data)
    pdag = learner.learnPDAG()
    assert pdag.size() == 8
    dag = learner.learnDAG()
    assert dag.getSize() == 8
    assert sorted(list(dag.getDescription())) == sorted(
        ["asia", "tub", "smoke", "lung", "bronc", "either", "xray", "dysp"]
    )


def testMathis():
    # extreme case to show that cmiic apply meek rule 3 correctly
    import pyagrum as gum

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

    learner = otagrum.ContinuousMIIC(sample)
    learner.setAlpha(0.01)

    learned_pdag = learner.learnPDAG()
    assert learned_pdag.size() == 4

    learned_named_dag = learner.learnDAG()
    assert learned_named_dag.getSize() == 4
    learned_dag = learned_named_dag.getDAG()
    assert learned_dag.sizeArcs() >= 2


def testEmptySampleRaises():
    with ott.assert_raises(TypeError):
        otagrum.ContinuousMIIC(ot.Sample())


testSpecificInstance()
testAsiaDirichlet()
testMathis()
testEmptySampleRaises()
