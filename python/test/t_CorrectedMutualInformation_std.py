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
    info = otagrum.CorrectedMutualInformation(data)

    assert info.getSize() == 1000
    assert info.getDimension() == 3

    assert info.getAlpha() == 0.01
    info.setAlpha(0.05)
    assert info.getAlpha() == 0.05

    description = info.getDataDescription()
    idA = description.find("A")
    idB = description.find("B")
    idD = description.find("D")

    miAB = info.compute2PtCorrectedInformation(idA, idB)
    miAD = info.compute2PtCorrectedInformation(idA, idD)

    assert miAB > miAD


def testEmptySampleRaises():
    with ott.assert_raises(TypeError):
        otagrum.CorrectedMutualInformation(ot.Sample())


def testThreePoint():
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

    info = otagrum.CorrectedMutualInformation(data)
    info.setAlpha(0.01)

    assert info.compute2PtCorrectedInformation(0, 1) > 0.0
    assert info.compute2PtCorrectedInformation(0, 4) < 0.0
    # variables 0, 1, 2 belong to the same correlated NormalCopula block
    ott.assert_almost_equal(
        info.compute3PtCorrectedInformation(0, 1, 2), -0.0592439, 1e-4, 1e-4
    )


testBasic()
testThreePoint()
testEmptySampleRaises()
