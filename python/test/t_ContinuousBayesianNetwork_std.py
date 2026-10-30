#!/usr/bin/env python


import openturns as ot
import openturns.testing as ott
import otagrum
import pyagrum as gum

proto = "A->B->C->D;E->A->C<-E"
bn = gum.BayesNet.fastPrototype(proto)

ndag = otagrum.NamedDAG(bn)
assert ndag.getSize() == 5
assert list(ndag.getDescription()) == ["A", "B", "C", "D", "E"]
assert list(ndag.getTopologicalOrder()) == [4, 0, 1, 2, 3]
for nod, parents, children in [
    (4, [], [0, 2]),
    (0, [4], [1, 2]),
    (1, [0], [2]),
    (2, [1, 4, 0], [3]),
    (3, [2], []),
]:
    assert sorted(list(ndag.getParents(nod))) == sorted(parents)
    assert sorted(list(ndag.getChildren(nod))) == sorted(children)


def testConstruction():
    order = ndag.getTopologicalOrder()
    marginals = [ot.Uniform(0.0, 1.0) for i in range(order.getSize())]
    copulas = []
    for i in range(order.getSize()):
        d = 1 + ndag.getParents(i).getSize()
        if d == 1:
            copulas.append(ot.IndependentCopula(1))
        else:
            R = ot.CorrelationMatrix(d)
            for a in range(d):
                for b in range(a):
                    R[a, b] = 0.5 / d
            copulas.append(ot.Student(5.0, [0.0] * d, [1.0] * d, R).getCopula())

    cbn = otagrum.ContinuousBayesianNetwork(ndag, marginals, copulas)
    assert cbn.getDimension() == 5
    assert len(cbn.getMarginals()) == 5
    assert len(cbn.getCopulas()) == 5

    ott.assert_almost_equal(cbn.computePDF([0.5] * 5), 1.949391972042584)

    realization = cbn.getRealization()
    assert len(realization) == 5
    for x in realization:
        assert 0.0 <= x <= 1.0

    sample = cbn.getSample(10)
    assert sample.getSize() == 10
    assert sample.getDimension() == 5


testConstruction()
