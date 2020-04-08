#!/usr/bin/env python

from __future__ import print_function

import openturns as ot
import otagrum
import pyAgrum as gum
import sys

# create data
proto = "A->B->C->D;E->A->C<-E"
bn = gum.BayesNet.fastPrototype(proto)
print("      proto : ", proto)
print("         BN : ", bn)

ndag = otagrum.NamedDAG(bn)
print("       size : ", ndag.getSize())
print("       desc : ", ndag.getDescription())
print("      nodes : ", ndag.getTopologicalOrder())
for nod in ndag.getTopologicalOrder():
    print(" parents(", nod, ") : ", ndag.getParents(nod))
    print("children(", nod, ") : ", ndag.getChildren(nod))

order = ndag.getTopologicalOrder()
jointDistributions = list()
for i in range(order.getSize()):
    d = 1 + ndag.getParents(i).getSize()
    print("i=", i, ", d=", d)
    R = ot.CorrelationMatrix(d)
    for i in range(d):
        for j in range(i):
            R[i, j] = 0.5 / d
    jointDistributions.append(ot.Student(5.0, [0.0]*d, [1.0]*d, R).getCopula())

cbn = otagrum.ContinuousBayesianNetwork(ndag, jointDistributions)
size = 300
sample = cbn.getSample(size)

# ContinuousBayesianNetworkFactory
factories = [ot.NormalCopulaFactory(), ot.BernsteinCopulaFactory()]
threshold = 0.1
maxParents = 5
factory = otagrum.ContinuousBayesianNetworkFactory(
    factories, ndag, threshold, maxParents)
cbn = factory.build(sample)
print('cbn=', cbn)
