#!/usr/bin/env python

import openturns.testing as ott
import otagrum
import pyagrum as gum


def testConstructorFromBayesNet():
    proto = "A->C->B->D;E->A->B<-E"
    bn = gum.BayesNet.fastPrototype(proto)
    ndag = otagrum.NamedDAG(bn)

    print("size : " + ("OK" if ndag.getSize() == 5 else "fail"))

    description = ndag.getDescription()
    idA = ndag.idFromName("A")
    idB = ndag.idFromName("B")
    idC = ndag.idFromName("C")
    idD = ndag.idFromName("D")
    idE = ndag.idFromName("E")

    print(
        "idFromName round-trips through description : "
        + ("OK" if description[idA] == "A" else "fail")
    )

    print("E has no parent : " + ("OK" if ndag.getParents(idE).getSize() == 0 else "fail"))
    print(
        "A has parent E : "
        + ("OK" if idE in list(ndag.getParents(idA)) else "fail")
    )
    print(
        "B has parents A, C and E : "
        + ("OK" if sorted(list(ndag.getParents(idB))) == sorted([idA, idC, idE]) else "fail")
    )
    print("D has no children : " + ("OK" if ndag.getChildren(idD).getSize() == 0 else "fail"))

    order = list(ndag.getTopologicalOrder())
    print("topological order has every node once : " + ("OK" if sorted(order) == list(range(5)) else "fail"))
    # A parent must appear before its children in the topological order.
    print(
        "topological order respects E before A : "
        + ("OK" if order.index(idE) < order.index(idA) else "fail")
    )


def testConstructorWithNonContiguousIdsRaises():
    bn = gum.BayesNet.fastPrototype("A->B->C<-D->E<-A")
    bn.erase("C")
    with ott.assert_raises(TypeError):
        otagrum.NamedDAG(bn)


testConstructorFromBayesNet()
testConstructorWithNonContiguousIdsRaises()
