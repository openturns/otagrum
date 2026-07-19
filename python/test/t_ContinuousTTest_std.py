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
    tester = otagrum.ContinuousTTest(data, 0.1)

    print("dimension : " + ("OK" if tester.getDimension() == 3 else "fail"))
    print("alpha round-trip : " + ("OK" if tester.getAlpha() == 0.1 else "fail"))
    tester.setAlpha(0.05)
    print("setAlpha : " + ("OK" if tester.getAlpha() == 0.05 else "fail"))

    description = tester.getDataDescription()
    idA = description.find("A")
    idB = description.find("B")
    idD = description.find("D")

    # A and B are strongly correlated: independence should be rejected.
    _, _, indepAB = tester.isIndep(idA, idB, ot.Indices())
    # D is independent from A: independence should not be rejected.
    _, _, indepAD = tester.isIndep(idA, idD, ot.Indices())

    print("correlated pair detected as dependent : " + ("OK" if not indepAB else "fail"))
    print("independent pair detected as independent : " + ("OK" if indepAD else "fail"))


def testEmptySampleRaises():
    try:
        otagrum.ContinuousTTest(ot.Sample())
        print("empty sample raises : fail (no exception)")
    except Exception:
        print("empty sample raises : OK")


testBasic()
testEmptySampleRaises()
