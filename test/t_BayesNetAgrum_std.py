#! /usr/bin/env python

from openturns import *
from pyAgrum import *
from otagrum import *

try :
    # Create an empty BN
    myBN = BayesNet()
    # Create light variable
    light = LabelizedVar("Light", "Light", 0)
    light.addLabel("Dim")
    light.addLabel("Bright")
    lightSize = len(light)
    # Create moisture vriable
    moisture = LabelizedVar("Moisture", "Moisture", 0)
    moisture.addLabel("Dry")
    moisture.addLabel("Wet")
    moistureSize = len(moisture)
    # Create height variable
    height = DiscretizedVar("Height", "Height")
    data = NumericalPoint(range(10, 100, 10))
    heightWhenDimAndDry = Uniform(0.0, 40.0)
    heightWhenDimAndWet = Uniform(10.0, 60.0)
    heightWhenBrightAndDry = Uniform(0.0, 20.0)
    heightWhenBrightAndWet = Uniform(40.0, 100.0)
    data = BayesNetAgrum.AdaptGrid(DistributionCollection([heightWhenDimAndDry, heightWhenDimAndWet, heightWhenBrightAndDry, heightWhenBrightAndWet]), data)
    heightSize = data.getSize() - 1
    for i in list(data):
        height.addTick(i)
    # Add variables to the net
    indexLight    = myBN.add(light)
    indexMoisture = myBN.add(moisture)
    indexHeight   = myBN.add(height)
    # Create arcs
    myBN.insertArc(indexLight, indexMoisture)
    myBN.insertArc(indexLight, indexHeight)
    myBN.insertArc(indexMoisture, indexHeight)
    # Create conditional probability tables
    myBN.cpt(indexLight)[:]= [0.5, 0.5]
    myBN.cpt(indexMoisture)[{'Light' : 'Dim'}] = [0.2, 0.8]
    myBN.cpt(indexMoisture)[{'Light' : 'Bright'}] = [0.6, 0.4]
    myBN.cpt(indexHeight)[{'Light': 'Dim', 'Moisture': 'Dry'}]   = BayesNetAgrum().Discretize(heightWhenDimAndDry, data)
    myBN.cpt(indexHeight)[{'Light': 'Bright', 'Moisture': 'Dry'}] = BayesNetAgrum().Discretize(heightWhenBrightAndDry, data)
    myBN.cpt(indexHeight)[{'Light': 'Dim', 'Moisture': 'Wet'}]    = BayesNetAgrum().Discretize(heightWhenDimAndWet, data)
    myBN.cpt(indexHeight)[{'Light': 'Bright', 'Moisture': 'Wet'}] = BayesNetAgrum().Discretize(heightWhenBrightAndWet, data)
    # Create a BayesNetAgrum object
    otbn = BayesNetAgrum(myBN)
    # Export to BIF file
    otbn.exportToBIFFile("test.bif")
    # Perform inference
    for evidenceValue in ["Bright", "Dim"]:
        # First, set evidence
        otbn.setEvidence("Light", evidenceValue)
        # Second, get a marginal distribution of interrest
        heightDistribution = otbn.getMarginal("Height")
        heightDistribution.setName("Height_Light_" + evidenceValue)
        # Use the distribution the way you want
        print "heightDistribution=", heightDistribution
        heightDistribution.drawPDF(-10.0, 110.0, 512).draw("PDF_" + heightDistribution.getName())
        heightDistribution.drawCDF(-10.0, 110.0, 512).draw("CDF_" + heightDistribution.getName())
    
except :
    import sys
    print "t_BayesNetAgrum_std.py", sys.exc_type, sys.exc_value

