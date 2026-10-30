#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott
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

    assert copula.getDimension() == dim

    import math

    ot.RandomGenerator.SetSeed(33)
    entropy = -copula.computeLogPDF(copula.getSample(500)).computeMean()[0]
    assert math.isfinite(entropy)

    generic = factory.build(sample)
    assert generic.getDimension() == dim


def testEmptySampleRaises():
    factory = otagrum.JunctionTreeBernsteinCopulaFactory()
    with ott.assert_raises(Exception):
        factory.buildAsJunctionTreeBernsteinCopula(ot.Sample())


def testDefaultBuild():
    factory = otagrum.JunctionTreeBernsteinCopulaFactory()
    empty = factory.buildAsJunctionTreeBernsteinCopula()
    assert empty.getDimension() == 1


testBuildFromSample()
testEmptySampleRaises()
testDefaultBuild()
