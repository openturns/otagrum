import openturns as ot
import otagrum
import pyAgrum as gum
import sys

bn = gum.BayesNet.fastPrototype("0->1->2->3;1->4->3;4->5;6->5;7->5")
ie = gum.LazyPropagation(bn)
jtagr = ie.junctionTree()
jt = otagrum.NamedJunctionTree(jtagr, bn)

for node in jtagr.nodes():
    print('Clique {} =>'.format(node), end=" ")
    if set(jt.getClique(node)) == jtagr.clique(node):
        print("OK")
    else:
        print("FAILED")
print()

for u, v in jtagr.edges():
    print('separator {},{} =>'.format(u, v), end=" ")
    if set(jt.getSeparator(u, v)) == jtagr.separator(u, v):
        print("OK")
    else:
        print("FAILED")
    u, v = v, u
    print('separator {},{} =>'.format(u, v), end=" ")
    if set(jt.getSeparator(u, v)) == jtagr.separator(u, v):
        print("OK")
    else:
        print("FAILED")
print()

for u in jt.getNodes():
    for v in jt.getNeighbours(u):
        if u < v:
            print('otagrum separator {},{} =>'.format(u, v), end=" ")
            if set(jt.getSeparator(u, v)) == jtagr.separator(u, v):
                print("OK")
            else:
                print("FAILED")
print()

bn = gum.fastBN("0->1->2<-0->3->4<-5->6;4->7;8->9->10<-11")
ie = gum.LazyPropagation(bn)
jt = ie.junctionTree()
njt = otagrum.NamedJunctionTree(jt, bn)

for cliq in njt.getOrderMaxFirst():
    print('Clique {} =>'.format(cliq), end=" ")
    if set(njt.getClique(cliq)) == jt.clique(cliq):
        print("OK")
    else:
        print("FAILED")
print()

print('cliques=', njt.getCliquesCollection())
print('separators=', njt.getSeparatorsCollection())
