#!/usr/bin/env python

import openturns as ot
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

    print("size : " + ("OK" if info.getSize() == 1000 else "fail"))
    print("dimension : " + ("OK" if info.getDimension() == 3 else "fail"))

    print("alpha round-trip : " + ("OK" if info.getAlpha() == 0.01 else "fail"))
    info.setAlpha(0.05)
    print("setAlpha : " + ("OK" if info.getAlpha() == 0.05 else "fail"))

    description = info.getDataDescription()
    idA = description.find("A")
    idB = description.find("B")
    idD = description.find("D")

    miAB = info.compute2PtCorrectedInformation(idA, idB)
    miAD = info.compute2PtCorrectedInformation(idA, idD)

    print(
        "correlated pair has higher mutual information : "
        + ("OK" if miAB > miAD else "fail")
    )


def testEmptySampleRaises():
    try:
        otagrum.CorrectedMutualInformation(ot.Sample())
        print("empty sample raises : fail (no exception)")
    except Exception:
        print("empty sample raises : OK")


testBasic()
testEmptySampleRaises()
