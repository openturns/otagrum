#!/usr/bin/env python

import openturns.testing as ott
import otagrum
import pyagrum as gum


def testConstructorFromBayesNet():
    proto = "A->C->B->D;E->A->B<-E"
    bn = gum.BayesNet.fastPrototype(proto)
    ndag = otagrum.NamedDAG(bn)

    assert ndag.getSize() == 5

    description = ndag.getDescription()
    idA = ndag.idFromName("A")
    idB = ndag.idFromName("B")
    idC = ndag.idFromName("C")
    idD = ndag.idFromName("D")
    idE = ndag.idFromName("E")

    assert description[idA] == "A"

    assert ndag.getParents(idE).getSize() == 0
    assert idE in list(ndag.getParents(idA))
    assert sorted(list(ndag.getParents(idB))) == sorted([idA, idC, idE])
    assert ndag.getChildren(idD).getSize() == 0

    order = list(ndag.getTopologicalOrder())
    assert sorted(order) == list(range(5))
    # A parent must appear before its children in the topological order.
    assert order.index(idE) < order.index(idA)


def testConstructorWithNonContiguousIdsRaises():
    bn = gum.BayesNet.fastPrototype("A->B->C<-D->E<-A")
    bn.erase("C")
    with ott.assert_raises(TypeError):
        otagrum.NamedDAG(bn)


def testToDot():
    bn = gum.BayesNet.fastPrototype("A->C->B->D;E->A->B<-E")
    ndag = otagrum.NamedDAG(bn)
    dot = ndag.toDot()
    assert isinstance(dot, str)
    for name in ["A", "B", "C", "D", "E"]:
        assert name in dot


def testSerialization():
    import openturns as ot
    import os
    from tempfile import mkdtemp
    import shutil

    work_dir = mkdtemp()
    try:
        bn = gum.BayesNet.fastPrototype("A->C->B->D;E->A->B<-E")
        ndag = otagrum.NamedDAG(bn)
        ndag.setName("ndagFromTest")
        path = os.path.join(work_dir, "ndag.xml")
        study = ot.Study(path)
        study.add("ndag", ndag)
        study.save()

        study2 = ot.Study(path)
        study2.load()
        check = otagrum.NamedDAG()
        study2.fillObject("ndag", check)

        assert check.getName() == "ndagFromTest"
        assert check.getSize() == ndag.getSize()
        assert list(check.getDescription()) == list(ndag.getDescription())
        assert list(check.getTopologicalOrder()) == list(ndag.getTopologicalOrder())
        for nod in range(ndag.getSize()):
            assert sorted(list(check.getParents(nod))) == sorted(list(ndag.getParents(nod)))
            assert sorted(list(check.getChildren(nod))) == sorted(list(ndag.getChildren(nod)))
    finally:
        shutil.rmtree(work_dir)


testConstructorFromBayesNet()
testConstructorWithNonContiguousIdsRaises()
testToDot()
testSerialization()
