#!/usr/bin/env python

from __future__ import print_function

import openturns as ot
import otagrum
import pyAgrum as gum
import sys

# Create variables
light = gum.LabelizedVariable("Light", "quality of light", 0)
moisture = gum.LabelizedVariable("Moisture", "quantity of moisture", 0)
height = gum.DiscretizedVariable("Height", "plant growth")

## Create labels and ticks

# light has 2 attributes : Dim and Bright
light.addLabel("Dim")
light.addLabel("Bright")

# moisture has 2 attributes : Dry and Wet
moisture.addLabel("Dry")
moisture.addLabel("Wet")

# height is a discretized variable
[height.addTick(i) for i in range(0, 150, 10)]
height.domainSize()


# height has a conditional probability table
# We give here its conditional distributions
# We use some OT distributions

# distribution when Dim and Dry
heightWhenDimAndDry = ot.Uniform(0.0, 20.0)
# distribution when Dim and Wet
heightWhenDimAndWet = ot.Triangular(15.0, 30.0, 50.0)
# distribution when Bright and Dry
heightWhenBrightAndDry = ot.Triangular(0.0, 15.0, 30.0)
# distribution when Bright and Wet
heightWhenBrightAndWet = ot.Normal(90.0, 10.0)


## Create the net
bn = gum.BayesNet("Plant Growth")

# Add variables
indexLight    = bn.add(light)
indexMoisture = bn.add(moisture)
indexHeight   = bn.add(height)

# Add arcs
bn.addArc(indexLight, indexMoisture)
bn.addArc(indexLight, indexHeight)
bn.addArc(indexMoisture, indexHeight)


# Create conditional probability tables
# light conditional probability table
bn.cpt(indexLight)[:]= [0.25, 0.75]


# moisture conditional probability table
# We show the antecedents of moisture with the order in which they were declared
print("moisture Antecedents= ", bn.cpt(indexMoisture).var_names)
bn.cpt(indexMoisture)[{'Light' : 'Dim'}] = [0.2, 0.8]
bn.cpt(indexMoisture)[{'Light' : 'Bright'}] = [0.6, 0.4]

# We have to enter some OT distributions whithin aGrUM conditional probability tables
# We show the antecedents of height with the order in which they were declared
# The new class Utils from otagrum is able to marry OT distributions and Agrum conditional probability tables
bn.cpt(indexHeight)[{'Light': 'Dim', 'Moisture': 'Dry'}]   = otagrum.Utils.Discretize(heightWhenDimAndDry, height)[:]
bn.cpt(indexHeight)[{'Light': 'Bright', 'Moisture': 'Dry'}] = otagrum.Utils.Discretize(heightWhenBrightAndDry, height)[:]
bn.cpt(indexHeight)[{'Light': 'Dim', 'Moisture': 'Wet'}]    = otagrum.Utils.Discretize(heightWhenDimAndWet, height)[:]
bn.cpt(indexHeight)[{'Light': 'Bright', 'Moisture': 'Wet'}] = otagrum.Utils.Discretize(heightWhenBrightAndWet, height)[:]

# Variability of the plant growth on my balcony
ie = gum.LazyPropagation(bn)
h_dist = otagrum.Utils.FromMarginal(ie.posterior("Height"))
print("Probability (height > 40cm) = {:.6g}".format(1.0 - h_dist.computeCDF(40.0)))
h_dist.drawPDF()


# Variability of the plant growth in my cellar
ie = gum.LazyPropagation(bn)
ie.setEvidence({'Light':'Dim'})
h_dist_dim = otagrum.Utils.FromMarginal(ie.posterior("Height"))
h_dist_dim.setDescription(['Height|Light=Dim'])
print("Probability (height > 40cm)|Light=Dim = {:.6g}".format(1.0 - h_dist_dim.computeCDF(40.0)))
h_dist_dim.drawPDF()


# Variability of the plant growth when the atmosphere is very wet
ie = gum.LazyPropagation(bn)
ie.setEvidence({'Moisture':'Wet'})
h_dist_wet = otagrum.Utils.FromMarginal(ie.posterior("Height"))
h_dist_wet.setDescription(['Height|Moisture=Wet'])
print("Probability (height > 40cm)|Moisture=Wet = {:.6g}".format(1.0 - h_dist_wet.computeCDF(40.0)))
h_dist_wet.drawPDF()


# Get the distribution of the variable "Light"
l_dist_wet = otagrum.Utils.FromPotential(ie.posterior("Light"))
print(l_dist_wet)
l_dist_wet.drawPDF()



# Get the joint distribution [H, M]
ie = gum.LazyPropagation(bn)
ie.addJointTarget(["Height","Moisture"])
ie.makeInference()
h_m_dist = otagrum.Utils.FromPotential(ie.jointPosterior(["Height","Moisture"]))
print(h_m_dist.getDescription())
print(h_m_dist.getMarginal(0))


