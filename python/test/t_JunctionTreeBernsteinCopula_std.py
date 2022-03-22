#!/usr/bin/env python


import openturns as ot
import otagrum
import pyAgrum as gum
import sys

bn = gum.BayesNet.fastPrototype('0->1->2->3;1->4->3;4->5;6->5;7->5')
ie = gum.LazyPropagation(bn)
jtagr = ie.junctionTree()
jt = otagrum.NamedJunctionTree(jtagr, bn)
copulaSample = ot.Normal(jt.getSize()).getSample(1000)
copula = otagrum.JunctionTreeBernsteinCopula(jt, copulaSample, 5, False)
print('copula=', copula)
sample = copula.getSample(10)
print('sample=', sample)
pdf = copula.computePDF(sample)
print('pdf=', pdf)
