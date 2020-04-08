#!/usr/bin/env python

from __future__ import print_function

import openturns as ot
import otagrum
import pyAgrum as gum
import sys


def test_basic():
    v = gum.DiscretizedVariable("v", "v")
    for i in [0, 4, 10, 15, 30, 40]:
        v.addTick(i)
    w = gum.DiscretizedVariable("w", "w")
    for i in [-1, 4, 10, 30, 40]:
        w.addTick(i)

    print("\n** From OT::Distribution to gum::Potential:")
    unifDistribution = ot.Uniform(0, 40)
    pv = gum.Potential(otagrum.Utils.Discretize(unifDistribution, v))
    pw = gum.Potential(otagrum.Utils.Discretize(unifDistribution, w))
    print(pv)
    print(pw)

    print("\nCatching InvalidArgumentException for bad support")
    try:
        otagrum.Utils.Discretize(ot.Uniform(1, 100), w)
        print("Fail")
    except:
        print("OK")

    print("\n** Back to OT::Distribution")
    print(otagrum.Utils.FromMarginal(pv))
    print(otagrum.Utils.FromMarginal(pw))


def test_fromMarginal():
    print("\n** FromRangeVariable")
    x = gum.RangeVariable("x", "x", 3, 10)
    px = gum.Potential().add(x).fillWith([1, 2, 3, 4, 5, 6, 7, 8])
    dx = otagrum.Utils.FromMarginal(px)
    print(px.normalize())
    print(dx)

    print("\n** From LabelizedVariable")
    y = gum.LabelizedVariable("y", "y", 0).addLabel(
        "True").addLabel("Maybe").addLabel("False")
    py = gum.Potential().add(y).fillWith([2, 8, 4]).normalize()
    print(py)
    print(otagrum.Utils.FromMarginal(py))

    print("\n** From LabelizedVariable but numerical")
    y = gum.LabelizedVariable("y", "y", 0).addLabel(
        "1").addLabel("1.5").addLabel("3.15")
    py = gum.Potential().add(y).fillWith([2, 8, 4]).normalize()
    print(py)
    print(otagrum.Utils.FromMarginal(py))


def test_fromPotential():
    x = gum.DiscretizedVariable("x", "x", [0, 0.5, 1, 3.5, 10])
    y = gum.LabelizedVariable("y", "y", ["chaud", "tiede", "froid"])
    print(x)
    sys.stdout.flush()
    print(y)
    sys.stdout.flush()

    p = gum.Potential().add(x).add(y)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]).normalize()
    print(p)
    sys.stdout.flush()

    distrib = otagrum.Utils.FromPotential(p)
    print(distrib)

    print(p.margSumIn(["y"]))
    print(distrib.getMarginal(1))


# test_basic()
# test_fromMarginal()
test_fromPotential()
