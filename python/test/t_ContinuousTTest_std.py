#!/usr/bin/env python

import openturns as ot
import openturns.testing as ott
import otagrum


def generateData(size):
    R = ot.CorrelationMatrix(2)
    R[0, 1] = 0.8
    collection = [ot.NormalCopula(R), ot.IndependentCopula(1)]
    copula = ot.BlockIndependentCopula(collection)
    copula.setDescription(["A", "B", "D"])
    return copula.getSample(size)


def testBasic():
    data = generateData(1000)
    tester = otagrum.ContinuousTTest(data, 0.1)

    assert tester.getDimension() == 3
    assert tester.getAlpha() == 0.1
    tester.setAlpha(0.05)
    assert tester.getAlpha() == 0.05

    description = tester.getDataDescription()
    idA = description.find("A")
    idB = description.find("B")
    idD = description.find("D")

    # A and B are strongly correlated: independence should be rejected.
    _, _, indepAB = tester.isIndep(idA, idB, ot.Indices())
    # D is independent from A: independence should not be rejected.
    _, _, indepAD = tester.isIndep(idA, idD, ot.Indices())

    assert not indepAB
    assert indepAD


def testEmptySampleRaises():
    with ott.assert_raises(TypeError):
        otagrum.ContinuousTTest(ot.Sample())


def testConditionalIndependence():
    ot.RandomGenerator.SetSeed(0)
    R = ot.CorrelationMatrix(3)
    R[0, 1] = 0.9
    R[0, 2] = 0.25
    block1 = ot.NormalCopula(R)
    block1.setDescription(["A1", "A2", "A3"])
    block2 = ot.FrankCopula(3.0)
    block2.setDescription(["B1", "B2"])
    block3 = ot.ClaytonCopula(2.0)
    block3.setDescription(["C1", "C2"])
    data = ot.BlockIndependentCopula([block1, block2, block3]).getSample(6000)

    tester = otagrum.ContinuousTTest(data)
    tester.setAlpha(0.1)

    _, _, indep01 = tester.isIndep(0, 1, ot.Indices())
    assert not indep01

    _, _, indep04 = tester.isIndep(0, 4, ot.Indices())
    assert indep04

    _, _, indep04cond = tester.isIndep(0, 4, ot.Indices([2, 3]))
    assert indep04cond


testBasic()
testConditionalIndependence()
testEmptySampleRaises()
