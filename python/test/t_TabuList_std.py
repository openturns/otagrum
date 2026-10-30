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
    learner = otagrum.TabuList(data)

    ndag = learner.learnDAG()
    idA = ndag.getDescription().find("A")
    idB = ndag.getDescription().find("B")
    idC = ndag.getDescription().find("C")
    idD = ndag.getDescription().find("D")
    idE = ndag.getDescription().find("E")
    idG = ndag.getDescription().find("G")

    dag = ndag.getDAG()
    assert dag.existsArc(idA, idB)
    assert dag.existsArc(idC, idD)
    # depends on map order ?
    assert dag.existsArc(idD, idE) or dag.existsArc(idE, idD)
    assert len(dag.children(idB)) == 0
    assert len(dag.children(idG)) == 0


def testAsiaDirichlet():
    data = ot.Sample.ImportFromTextFile(
        os.path.join(os.path.dirname(__file__), "asia_dirichlet_5000.csv"), ","
    )
    learner = otagrum.TabuList(data)
    dag = learner.learnDAG()
    assert dag.getSize() == 8
    # asia -> tub arc is stable on this dataset
    description = dag.getDescription()
    idAsia = description.find("asia")
    idTub = description.find("tub")
    assert dag.getDAG().existsArc(idAsia, idTub) or dag.getDAG().existsArc(
        idTub, idAsia
    )


def testZeroRestartsRaises():
    data = generateDataForSpecificInstance(100)
    with ott.assert_raises(TypeError):
        otagrum.TabuList(data, restarts=0)


def testBlockData():
    ot.RandomGenerator.SetSeed(0)
    R = ot.CorrelationMatrix(3)
    for i in range(3):
        for j in range(i):
            R[i, j] = 0.64
    block1 = ot.FrankCopula(4.0)
    block1.setDescription(["A1", "A2"])
    block2 = ot.ClaytonCopula(3.0)
    block2.setDescription(["B1", "B2"])
    data = ot.BlockIndependentCopula([block1, block2, ot.NormalCopula(R)]).getSample(
        1000
    )

    learner = otagrum.TabuList(data)
    ndag = learner.learnDAG()
    assert ndag.getSize() == 7
    assert ndag.getDescription().getSize() == 7
    dag = ndag.getDAG()
    description = ndag.getDescription()

    def hasEdge(name1, name2):
        i = description.find(name1)
        j = description.find(name2)
        return dag.existsArc(i, j) or dag.existsArc(j, i)

    # each dependent block must be recovered (either orientation)
    assert hasEdge("A1", "A2")
    assert hasEdge("B1", "B2")
    # NormalCopula block variables default to X0, X1, X2
    assert hasEdge("X0", "X1")
    assert hasEdge("X0", "X2") or hasEdge("X1", "X2")


testSpecificInstance()
testAsiaDirichlet()
testBlockData()
testZeroRestartsRaises()
