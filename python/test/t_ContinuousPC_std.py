from __future__ import print_function

from time import time

import openturns as ot
import pyAgrum as gum

import otagrum

def generateDataForSpecificInstance(size):
  R = ot.CorrelationMatrix(3)
  R[0, 1] = 0.5
  R[0, 2] = 0.45
  collection = [ot.FrankCopula(3.0), ot.NormalCopula(R), ot.ClaytonCopula(2.0)]
  copula = ot.ComposedCopula(collection)
  return copula.getSample(size)


size = 1000
data = generateDataForSpecificInstance(size)

alpha = 0.9
binNumber = 3

t0 = time()
learner = otagrum.ContinuousPC(data, binNumber, alpha)
t1 = time() - t0

#skel = learner.inferSkeleton()
