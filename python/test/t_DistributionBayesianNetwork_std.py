#!/usr/bin/env python

import math
import openturns as ot
import openturns.testing as ott
import otagrum
import pyagrum as gum

ot.RandomGenerator.SetSeed(0)

bn = gum.BayesNet.fastPrototype("A->C->B;")
ndag = otagrum.NamedDAG(bn)
joints = []
for i in range(ndag.getSize()):
    dim = 1 + ndag.getParents(i).getSize()
    R = ot.CorrelationMatrix(dim)
    for j in range(dim):
        for k in range(j):
            R[j, k] = 0.8
    joints.append(ot.Normal([0.0] * dim, [1.0] * dim, R))
dbn = otagrum.DistributionBayesianNetwork(ndag, joints)
assert dbn.getDimension() == 3

pdf = dbn.computePDF([0.5] * dbn.getDimension())
assert math.isfinite(pdf)
assert pdf > 0.0

realization = dbn.getRealization()
assert len(realization) == dbn.getDimension()

assert dbn.getParameterDimension() > 0

ot.Log.Show(ot.Log.TRACE)
validator = ott.DistributionValidation(dbn)
validator.skipDDF()  # slow
validator.skipGradient()  # generic
validator.skipComplementaryCDF()  # slow
validator.skipConditional()  # slow
validator.skipTransformation()  # slow
validator.skipEntropy()  # slow
validator.skipCharacteristicFunction()  # slow
validator.skipGeneratingFunction()  # slow
validator.skipMinimumVolumeInterval()  # slow
validator.skipMinimumVolumeLevelSet()  # slow
validator.skipConfidenceInterval()  # slow
validator.skipCorrelation()  # slow
validator.skipDependenceMeasures()  # slow
validator.run()
assert dbn.getDimension() == 3
