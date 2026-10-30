#!/usr/bin/env python

import openturns.testing as ott
import otagrum
import pyagrum as gum

bn = gum.BayesNet.fastPrototype("0->1->2->3;1->4->3;4->5;6->5;7->5")
ie = gum.LazyPropagation(bn)
jtagr = ie.junctionTree()
jt = otagrum.NamedJunctionTree(jtagr, bn)

for node in jtagr.nodes():
    assert set(jt.getClique(node)) == jtagr.clique(node)

for u, v in jtagr.edges():
    assert set(jt.getSeparator(u, v)) == jtagr.separator(u, v)
    assert set(jt.getSeparator(v, u)) == jtagr.separator(v, u)

for u in jt.getNodes():
    for v in jt.getNeighbours(u):
        if u < v:
            assert set(jt.getSeparator(u, v)) == jtagr.separator(u, v)

bn = gum.fastBN("0->1->2<-0->3->4<-5->6;4->7;8->9->10<-11")
ie = gum.LazyPropagation(bn)
jt = ie.junctionTree()
njt = otagrum.NamedJunctionTree(jt, bn)

for cliq in njt.getOrderMaxFirst():
    assert set(njt.getClique(cliq)) == jt.clique(cliq)

# cliques/separators cover every variable exactly as the reference junction tree
assert sorted(sum([sorted(c) for c in njt.getCliquesCollection()], [])) == sorted(
    sum([sorted(jt.clique(n)) for n in jt.nodes()], [])
)
assert len(njt.getSeparatorsCollection()) == len(list(jt.edges()))


def testInvalidConstructionsRaise():
    bn = gum.BayesNet.fastPrototype("0->1->2->3;1->4->3->5;6->5;7->5")
    ie = gum.LazyPropagation(bn)
    jtagr = ie.junctionTree()
    short_names = [bn.variable(n).name() for n in bn.nodes() if n != 3]
    with ott.assert_raises(TypeError):
        otagrum.NamedJunctionTree(jtagr, short_names)

    bn2 = gum.BayesNet.fastPrototype("0->1->2->3;1->4->3->5;6->5;7->5")
    bn2.erase("3")
    ie2 = gum.LazyPropagation(bn2)
    hole_names = [bn2.variable(n).name() for n in bn2.nodes()]
    with ott.assert_raises(TypeError):
        otagrum.NamedJunctionTree(ie2.junctionTree(), hole_names)


def testGetMarginal():
    bn = gum.BayesNet.fastPrototype("a->b->c->d->h;b->e->d;e->f->h;e->g->h;e->h")
    ie = gum.LazyPropagation(bn)
    jt = otagrum.NamedJunctionTree(ie.junctionTree(), bn)
    assert jt.getSize() == 8
    marginal = jt.getMarginal([4, 0, 1])
    assert marginal.getSize() == 3
    for cliq in marginal.getOrderMaxFirst():
        assert len(set(marginal.getClique(cliq))) > 0


testInvalidConstructionsRaise()
testGetMarginal()
