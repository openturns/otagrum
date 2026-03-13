#!/usr/bin/env python

import os
import openturns as ot
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
    learner.setVerbosity(True)
    dag = learner.learnDAG()
    print(dag.toDot())


testSpecificInstance()
testAsiaDirichlet()
