from __future__ import print_function
import os
from time import time

import openturns as ot
import pyAgrum as gum
import otagrum
import sys


def generateDataForSpecificInstance(size):
    R = ot.CorrelationMatrix(3)
    R[0, 1] = 0.5
    R[0, 2] = 0.45
    collection = [ot.FrankCopula(3.0), ot.NormalCopula(
        R), ot.ClaytonCopula(2.0)]
    copula = ot.ComposedCopula(collection)
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
        os.path.dirname(__file__)+"/asia_dirichlet_5000.csv", ",")
    learner = otagrum.ContinuousMIIC(data)
    learner.setVerbosity(True)
    pdag = learner.learnPDAG()
    # print(pdag)
    print(pdag.toDot())
    dag = learner.learnDAG()
    print(dag.toDot())


testSpecificInstance()
testAsiaDirichlet()
