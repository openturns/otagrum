#!/usr/bin/env python

from __future__ import print_function

import openturns as ot
import otagrum
import pyAgrum as gum
import sys

def testOK():
    bn = gum.BayesNet.fastPrototype("0->1->2->3;1->4->3;4->5;6->5;7->5")
    ie = gum.LazyPropagation(bn)
    jtagr = ie.junctionTree()
    names = [bn.variable(elt).name() for elt in bn.nodes()]
    jt  = otagrum.JunctionTree(jtagr, names)
    print(jt)
    #nodes = jt.getNeighbours(3)
    #print(nodes.__class__.__name__)

testOK()
