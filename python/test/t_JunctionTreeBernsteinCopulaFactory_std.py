#!/usr/bin/env python

import openturns as ot
import otagrum


def generateData(size):
    dim = 4
    R = ot.CorrelationMatrix(dim)
    for i in range(dim):
        for j in range(i):
            R[i, j] = 0.3
    ot.RandomGenerator.SetSeed(0)
    return ot.Normal([0.0] * dim, R).getSample(size)


def testBuildFromSample():
    dim = 4
    sample = generateData(500)

    factory = otagrum.JunctionTreeBernsteinCopulaFactory(5, 0.1, 3)
    copula = factory.buildAsJunctionTreeBernsteinCopula(sample)

    print("learnt copula dimension : " + ("OK" if copula.getDimension() == dim else "fail"))

    generic = factory.build(sample)
    print(
        "build() dimension matches : "
        + ("OK" if generic.getDimension() == dim else "fail")
    )


def testEmptySampleRaises():
    factory = otagrum.JunctionTreeBernsteinCopulaFactory()
    try:
        factory.buildAsJunctionTreeBernsteinCopula(ot.Sample())
        print("empty sample raises : fail (no exception)")
    except Exception:
        print("empty sample raises : OK")


def testDefaultBuild():
    factory = otagrum.JunctionTreeBernsteinCopulaFactory()
    empty = factory.buildAsJunctionTreeBernsteinCopula()
    print("default build dimension : " + ("OK" if empty.getDimension() == 1 else "fail"))


testBuildFromSample()
testEmptySampleRaises()
testDefaultBuild()
