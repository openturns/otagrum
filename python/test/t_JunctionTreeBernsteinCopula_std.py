#!/usr/bin/env python


import openturns as ot
import openturns.testing as ott
import otagrum
import pyagrum as gum

ot.RandomGenerator.SetSeed(0)

bn = gum.BayesNet.fastPrototype("0->1->2->3;1->4->3;4->5;6->5;7->5")
ie = gum.LazyPropagation(bn)
jtagr = ie.junctionTree()
jt = otagrum.NamedJunctionTree(jtagr, bn)
assert jt.getSize() == 8
copulaSample = ot.Normal(jt.getSize()).getSample(1000)
copula = otagrum.JunctionTreeBernsteinCopula(jt, copulaSample, 5, False)
assert copula.getDimension() == 8
sample = copula.getSample(10)
assert sample.getSize() == 10
assert sample.getDimension() == 8
pdf = copula.computePDF(sample)
assert pdf.getSize() == 10
for i in range(10):
    assert pdf[i, 0] >= 0.0
    assert pdf[i, 0] < 1e100


def testCliquesOrderGivesSameEntropy():
    dim = jt.getSize()
    cov = ot.CovarianceMatrix(dim)
    for i in range(dim):
        for j in range(i):
            cov[i, j] = 1.0
        cov[i, i] = 2.0
    ot.RandomGenerator.SetSeed(0)
    copulaSample = ot.Normal([0.0] * dim, cov).getSample(1000)

    copula1 = otagrum.JunctionTreeBernsteinCopula(jt, copulaSample, 5, False)
    order = jt.getOrderMaxFirst()
    reversedOrder = ot.Indices([order[i] for i in reversed(range(order.getSize()))])
    assert list(reversedOrder) != list(order)
    copula2 = otagrum.JunctionTreeBernsteinCopula(jt, copulaSample, 5, False)
    copula2.setCliquesOrder(reversedOrder)

    ot.RandomGenerator.SetSeed(33)
    sample = copula1.getSample(1000)
    entropy1 = -copula1.computeLogPDF(sample).computeMean()[0]
    entropy2 = -copula2.computeLogPDF(sample).computeMean()[0]

    ott.assert_almost_equal(entropy1, entropy2, 1e-5, 1e-5)


testCliquesOrderGivesSameEntropy()
