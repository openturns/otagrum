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


size=1000
seuil=0.85

data=generateDataForSpecificInstance(size)

t0 = time()
g=otagrum.ContinuousPC(data,alpha=seuil)
t1=time()-t0
print("##")
print("## PC algorithm for size={0}, dimension={1} and seuil={2:4.3f}".format(size,data.getDimension(),seuil))
print("##")
#showGraph(g)

#jtg=gum.JTGenerator()
#jt=jtg.generate(g)
##showGraph(jt)
#learner=otagrum.ContinuousPC()
