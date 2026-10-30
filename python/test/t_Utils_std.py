#!/usr/bin/env python


import openturns as ot
import openturns.testing as ott
import otagrum
import pyagrum as gum


def test_basic():
    v = gum.DiscretizedVariable("v", "v")
    for i in [0, 4, 10, 15, 30, 40]:
        v.addTick(i)
    w = gum.DiscretizedVariable("w", "w")
    for i in [-1, 4, 10, 30, 40]:
        w.addTick(i)

    unifDistribution = ot.Uniform(0, 40)
    pv = gum.Tensor(otagrum.Utils.Discretize(unifDistribution, v))
    pw = gum.Tensor(otagrum.Utils.Discretize(unifDistribution, w))
    ott.assert_almost_equal([float(pv[i]) for i in range(5)], [0.1, 0.15, 0.125, 0.375, 0.25])
    ott.assert_almost_equal([float(pw[i]) for i in range(4)], [0.1, 0.15, 0.5, 0.25])

    # out-of-support raises unless truncated
    with ott.assert_raises(Exception):
        otagrum.Utils.Discretize(ot.Uniform(1, 100), w, False)

    backV = otagrum.Utils.FromMarginal(pv)
    backW = otagrum.Utils.FromMarginal(pw)
    assert backV.getImplementation().getClassName() == "Histogram"
    assert backW.getImplementation().getClassName() == "Histogram"
    ott.assert_almost_equal(backV.getMean()[0], 20.0, 1e-4, 1e-4)
    ott.assert_almost_equal(backW.getMean()[0], 19.95, 1e-4, 1e-4)


def test_fromMarginal():
    x = gum.RangeVariable("x", "x", 3, 10)
    px = gum.Tensor().add(x).fillWith([1, 2, 3, 4, 5, 6, 7, 8])
    dx = otagrum.Utils.FromMarginal(px.normalize())
    assert dx.getDimension() == 1
    for i, p in enumerate([1, 2, 3, 4, 5, 6, 7, 8]):
        ott.assert_almost_equal(dx.computePDF([3 + i]), p / 36.0)

    y = (
        gum.LabelizedVariable("y", "y", 0)
        .addLabel("True")
        .addLabel("Maybe")
        .addLabel("False")
    )
    py = gum.Tensor().add(y).fillWith([2, 8, 4]).normalize()
    dy = otagrum.Utils.FromMarginal(py)
    ott.assert_almost_equal(dy.computePDF([0.0]), 2 / 14.0)
    ott.assert_almost_equal(dy.computePDF([1.0]), 8 / 14.0)
    ott.assert_almost_equal(dy.computePDF([2.0]), 4 / 14.0)

    y = (
        gum.LabelizedVariable("y", "y", 0)
        .addLabel("1")
        .addLabel("1.5")
        .addLabel("3.15")
    )
    py = gum.Tensor().add(y).fillWith([2, 8, 4]).normalize()
    dy = otagrum.Utils.FromMarginal(py)
    ott.assert_almost_equal(dy.computePDF([1.0]), 2 / 14.0)
    ott.assert_almost_equal(dy.computePDF([1.5]), 8 / 14.0)
    ott.assert_almost_equal(dy.computePDF([3.15]), 4 / 14.0)


def test_fromMarginalPartiallyNumerical():
    y = (
        gum.LabelizedVariable("y", "y", 0)
        .addLabel("1")
        .addLabel("foo")
        .addLabel("3.15")
    )
    py = gum.Tensor().add(y).fillWith([2, 8, 4]).normalize()
    dy = otagrum.Utils.FromMarginal(py)
    assert dy.getDimension() == 1
    ott.assert_almost_equal(dy.computePDF([0.0]), 2 / 14.0)
    ott.assert_almost_equal(dy.computePDF([1.0]), 8 / 14.0)
    ott.assert_almost_equal(dy.computePDF([2.0]), 4 / 14.0)


def test_fromMarginalDiscretized():
    y = gum.DiscretizedVariable("y", "y", [0, 0.5, 1.5, 2, 2.3, 10])
    py = gum.Tensor().add(y).fillWith([1, 2, 3, 4, 5]).normalize()
    dy = otagrum.Utils.FromMarginal(py)
    assert dy.getDimension() == 1
    # Histogram heights are densities: proba / width
    widths = [0.5, 1.0, 0.5, 0.3, 7.7]
    probas = [1 / 15.0, 2 / 15.0, 3 / 15.0, 4 / 15.0, 5 / 15.0]
    for x, p, w in zip([0.25, 1.0, 1.75, 2.15, 5.0], probas, widths):
        ott.assert_almost_equal(dy.computePDF([x]), p / w)


def test_fromTensor():
    x = gum.DiscretizedVariable("x", "x", [0, 0.5, 1, 3.5, 10])
    y = gum.LabelizedVariable("y", "y", ["chaud", "tiede", "froid"])

    p = gum.Tensor().add(x).add(y)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]).normalize()

    distrib = otagrum.Utils.FromTensor(p)
    assert distrib.getDimension() == 2
    ott.assert_almost_equal(distrib.computePDF([0.25, 0.0]), 1 / 39.0, 1e-12, 1e-12)

    assert distrib.getMarginal(1).getDimension() == 1


def test_fromTensorMarginalConsistency():
    y = gum.LabelizedVariable("y", "y", 0)
    y.addLabel("True").addLabel("Maybe").addLabel("False")

    z = gum.DiscretizedVariable("z", "z", [0, 0.5, 1.5])

    p = gum.Tensor()
    p.add(y)
    p.add(z)
    p.fillWith([1, 2, 3, 4, 5, 6]).normalize()

    distribution = otagrum.Utils.FromTensor(p)
    assert distribution.getDimension() == 2

    import math

    for m in [0, 1]:
        marginal = distribution.getMarginal(m)
        assert marginal.getDimension() == 1

    tensor_marginal_y = p.sumIn(["y"])
    check_y = otagrum.Utils.FromMarginal(tensor_marginal_y)
    total_y = sum(check_y.computePDF([k]) for k in [0.0, 1.0, 2.0])
    ott.assert_almost_equal(total_y, 1.0)

    tensor_marginal_z = p.sumIn(["z"])
    check_z = otagrum.Utils.FromMarginal(tensor_marginal_z)
    total_z = check_z.computeCDF(1.5)
    ott.assert_almost_equal(total_z, 1.0)
    assert math.isfinite(check_z.computePDF([0.25]))


def test_fromInference():
    bn = gum.BayesNet()
    bn.add(gum.DiscretizedVariable("A", "A", [1, 1.5, 2, 2.5, 3, 4]))
    bn.add(gum.LabelizedVariable("B", "B", ["chaud", "tiede", "froid"]))
    bn.add(gum.RangeVariable("C", "C", 1, 4))
    bn.addArc("A", "C")
    bn.addArc("C", "B")
    bn.cpt("A").fillWith([1, 2, 3, 4, 5]).normalize()
    bn.cpt("C").fillWith(list(range(1, 21))).normalizeAsCPT()
    bn.cpt("B").fillWith(list(range(1, 13))).normalizeAsCPT()

    ie = gum.LazyPropagation(bn)
    ie.addJointTarget({0, 2})
    ie.addTarget(1)
    ie.makeInference()

    joint = otagrum.Utils.FromTensor(ie.jointPosterior({0, 2}))
    assert joint.getDimension() == 2
    assert sorted(joint.getDescription()) == ["A", "C"]

    for name in ["A", "B", "C"]:
        posterior = otagrum.Utils.FromTensor(ie.posterior(name))
        assert posterior.getDimension() == 1
        assert posterior.getDescription() == [name]


test_basic()
test_fromMarginal()
test_fromMarginalPartiallyNumerical()
test_fromMarginalDiscretized()
test_fromTensor()
test_fromTensorMarginalConsistency()
test_fromInference()
