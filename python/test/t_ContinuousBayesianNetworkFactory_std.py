#!/usr/bin/env python


import openturns as ot
import otagrum
import pyagrum as gum

# create data
proto = "A->B->C->D;E->A->C<-E"
bn = gum.BayesNet.fastPrototype(proto)

ndag = otagrum.NamedDAG(bn)
assert ndag.getSize() == 5
assert list(ndag.getDescription()) == ["A", "B", "C", "D", "E"]

order = ndag.getTopologicalOrder()
marginals = [ot.Uniform(0.0, 1.0) for i in range(order.getSize())]
copulas = list()
for i in range(order.getSize()):
    d = 1 + ndag.getParents(i).getSize()
    assert d >= 1
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
sample = cbn.getSample(300)
assert sample.getSize() == 300
assert sample.getDimension() == 5
# ContinuousBayesianNetworkFactory
marginalsFactory = ot.UniformFactory()
copulasFactory = ot.BernsteinCopulaFactory()
threshold = 0.1
maxParents = 5
factory = otagrum.ContinuousBayesianNetworkFactory(
    marginalsFactory, copulasFactory, ndag, threshold, maxParents
)
learnt = factory.build(sample)
assert learnt.getDimension() == 5
assert learnt.getSample(10).getSize() == 10
