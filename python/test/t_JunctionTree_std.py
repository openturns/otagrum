#!/usr/bin/env python

from __future__ import print_function

import openturns as ot
import otagrum
import pyAgrum as gum
import sys

bn = gum.BayesNet.fastPrototype("0->1->2->3;1->4->3;4->5;6->5;7->5")
ie = gum.LazyPropagation(bn)
jtagr = ie.junctionTree()
jt  = otagrum.JunctionTree(jtagr, bn.names())
print(jt)
for node in jtagr.nodes():
    print('clique', node, '=', list(jtagr.clique(node)))
for edge in jtagr.edges():
    u,v = edge
    #print('separator', edge, '=', jtagr.separator(u,v))
