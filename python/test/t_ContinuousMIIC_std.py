#!/usr/bin/env python

import os
import openturns as ot
import otagrum
import sys


def generateDataForSpecificInstance(size):
    R = ot.CorrelationMatrix(3)
    R[0, 1] = 0.5
    R[0, 2] = 0.45
    collection = [ot.FrankCopula(3.0), ot.NormalCopula(R), ot.ClaytonCopula(2.0)]
    copula = ot.BlockIndependentCopula(collection)
    copula.setDescription("ABCDEFG")
    return copula.getSample(size)


def testSpecificInstance():
    size = 1000
    data = generateDataForSpecificInstance(size)
    learner = otagrum.ContinuousMIIC(data)

    # skel = learner.learnSkeleton()
    # print(skel.toDot())

    dag = learner.learnDAG()
    print(dag.toDot())
    sys.stdout.flush()


def testAsiaDirichlet():
    data = ot.Sample.ImportFromTextFile(
        os.path.join(os.path.dirname(__file__), "asia_dirichlet_5000.csv"), ","
    )
    learner = otagrum.ContinuousMIIC(data)
    learner.setVerbosity(True)
    pdag = learner.learnPDAG()
    # print(pdag)
    print(pdag.toDot())
    sys.stdout.flush()
    dag = learner.learnDAG()
    print(dag.toDot())
    sys.stdout.flush()


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

    bn = gum.BayesNet()
    for nid in dag.nodes():
        bn.add(gum.LabelizedVariable(f"X{nid}", f"X{nid}", 2))
    for nid in dag.nodes():
        for cid in dag.children(nid):
            bn.addArc(nid, cid)

    learner = otagrum.ContinuousMIIC(sample)
    learner.setAlpha(0.01)
    learner.setVerbosity(False)

    learned_pdag = learner.learnPDAG()
    print("pdag:", learned_pdag)

    learned_named_dag = learner.learnDAG()
    learned_dag = learned_named_dag.getDAG()

    bn2 = gum.BayesNet()
    for nid in learned_dag.nodes():
        bn2.add(gum.LabelizedVariable(f"X{nid}", f"X{nid}", 2))
    for t, h in learned_dag.arcs():
        bn2.addArc(t, h)

    learned_cpdag = gum.EssentialGraph(bn2).pdag()
    print("cpdag:", learned_cpdag)


testSpecificInstance()
testAsiaDirichlet()
testMathis()
