# Load OpenTURNS to manipulate distributions
from openturns import *
# Load pyAgrum to define the Network
from pyAgrum import *
# Load the link between OT and aGrUM
from otagrum import *

# Create an empty BN
netName = "Plant Growth"
myBN = BayesNet(netName)

# Create variables
# LabelizedVar(name, comment, modalities number)
# DiscretizedVar(name, comment)
light = LabelizedVar("Light", "quality of light", 0)
moisture = LabelizedVar("Moisture", "quantity of moisture", 0)
height = DiscretizedVar("Height", "plant growth")

# Create labels and ticks
# light has 2 attributes : Dim and Bright
light.addLabel("Dim")
light.addLabel("Bright")
lightSize = len(light)

# moisture has 2 attributes : Dry and Wet
moisture.addLabel("Dry")
moisture.addLabel("Wet")
moistureSize = len(moisture)


# height has a conditional probability table
# We give here its conditional distributions
# We use some OT distributions
# distribution when Dim and Dry
heightWhenDimAndDry = Uniform(0.0, 20.0)
# distribution when Dim and Wet
heightWhenDimAndWet = Triangular(15.0, 30.0, 50.0)
# distribution when Bright and Dry
heightWhenBrightAndDry = Triangular(0.0, 15.0, 30.0)
# distribution when Bright and Wet
heightWhenBrightAndWet = Normal(90.0, 10.0)

# height is a discretized variable
# We give here its discretized range
data = NumericalPoint(range(0, 180, 10))
# We adapt here its range to its conditional distributions
data = BayesNetAgrum.AdaptGrid(DistributionCollection([heightWhenDimAndDry, heightWhenDimAndWet, heightWhenBrightAndDry, heightWhenBrightAndWet]), data)
# We create here the bounds of each local range
#heightSize = data.getSize() - 1
for i in data:
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
# light conditional probability table
myBN.cpt(indexLight)[:]= [0.25, 0.75]

# moisture conditional probability table
# We show the antecedents of moisture with the order in which they were declared
print "moisture Antecedents= ", myBN.cpt(indexMoisture).var_names
myBN.cpt(indexMoisture)[{'Light' : 'Dim'}] = [0.2, 0.8]
myBN.cpt(indexMoisture)[{'Light' : 'Bright'}] = [0.6, 0.4]

# We have to enter some OT distributions whithin aGrUM conditional probability tables
# We show the antecedents of height with the order in which they were declared
print "meight Antecedents= ", myBN.cpt(indexHeight).var_names
# The new class BayesNetAgrum from otagrum is able to marry OT distributions and Agrum conditional probability tables
myBN.cpt(indexHeight)[{'Light': 'Dim', 'Moisture': 'Dry'}]   = BayesNetAgrum.Discretize(heightWhenDimAndDry, data)
myBN.cpt(indexHeight)[{'Light': 'Bright', 'Moisture': 'Dry'}] = BayesNetAgrum.Discretize(heightWhenBrightAndDry, data)
myBN.cpt(indexHeight)[{'Light': 'Dim', 'Moisture': 'Wet'}]    = BayesNetAgrum.Discretize(heightWhenDimAndWet, data)
myBN.cpt(indexHeight)[{'Light': 'Bright', 'Moisture': 'Wet'}] = BayesNetAgrum.Discretize(heightWhenBrightAndWet, data)

# Create a BayesNetAgrum object
otbn = BayesNetAgrum(myBN)

# We want to visualize the bayesian network 
# Export to BIF file
otbn.exportToBIFFile(netName+".bif")
# Visualize the graph
otbn.draw(netName)


# Get the distribution of the variable "Height"
heightDistribution = otbn.getMarginal("Height")

# You are whithin Open TURNS : use the distribution the way you want
# Print its caracteristics
print "heightDistribution =", heightDistribution
# Draw its PDf and CDF
heightDistributionPDF = heightDistribution.drawPDF()
heightDistributionPDF.setTitle("Height - PDF")
heightDistributionPDF.setXTitle("height values")
heightDistributionPDF.setYTitle("density")
heightDistributionPDF_draw = heightDistributionPDF.getDrawable(0)
heightDistributionPDF_draw.setLegendName("")
heightDistributionPDF.setDrawable(heightDistributionPDF_draw,0)
heightDistributionPDF.draw("Height_PDF")
heightDistributionCDF = heightDistribution.drawCDF()
heightDistributionCDF.setTitle("Height - CDF")
heightDistributionCDF.setXTitle("height values")
heightDistributionCDF.setYTitle("density")
heightDistributionCDF_draw = heightDistributionCDF.getDrawable(0)
heightDistributionCDF_draw.setLegendName("")
heightDistributionCDF.setDrawable(heightDistributionCDF_draw,0)
heightDistributionCDF.draw("Height_CDF")


# Perform inference

##############################################################
# Example 1 : it is sunny : "Light" == "Bright"

# First, set evidence
otbn.setEvidence("Light", "Bright")

# Get the distribution of the variable "Height"
heightDistribution_Bright = otbn.getMarginal("Height")

# You are whithin Open TURNS : use the distribution the way you want
# Print its caracteristics
print "Height Distribution when Bright =", heightDistribution_Bright
print "Probability (height > 40cm) = ", 1-heightDistribution_Bright.computeCDF(40)
print ""
# Draw its PDF and CDF
heightDistributionPDF_Bright = heightDistribution_Bright.drawPDF()
heightDistributionPDF_Bright.setTitle("Height when Bright - PDF")
heightDistributionPDF_Bright.setXTitle("height values")
heightDistributionPDF_Bright.setYTitle("density")
heightDistributionPDF_Bright_draw = heightDistributionPDF_Bright.getDrawable(0)
heightDistributionPDF_Bright_draw.setLegendName("")
heightDistributionPDF_Bright.setDrawable(heightDistributionPDF_Bright_draw,0)
#heightDistributionPDF_Bright.draw("Height_PDF_WhenBright")

heightDistributionCDF_Bright = heightDistribution_Bright.drawCDF()
heightDistributionCDF_Bright.setTitle("Height when Bright - CDF")
heightDistributionCDF_Bright.setXTitle("height values")
heightDistributionCDF_Bright.setYTitle("cdf")
heightDistributionCDF_Bright_draw = heightDistributionCDF_Bright.getDrawable(0)
heightDistributionCDF_Bright_draw.setLegendName("")
heightDistributionCDF_Bright.setDrawable(heightDistributionCDF_Bright_draw,0)
#heightDistributionCDF_Bright.draw("Height_CDF_WhenBright")

##############################################################
# Example 2 : the atmosphere is very wet : "Moisture" == "Wet"

# First, erase previous evidences!
otbn.eraseEvidences()
# Set evidence
otbn.setEvidence("Moisture", "Wet")

# Get the distribution of the variable "Light"
lightDistribution_Wet = otbn.getMarginal("Light")
print "Light distribution when Wet Air = "
print "Proba(Dim) = ", lightDistribution_Wet.getParametersCollection()[0][0]
print "Proba(Bright) = ", lightDistribution_Wet.getParametersCollection()[0][1]
print ""

# Get the distribution of the variable "Height"
heightDistribution_Wet = otbn.getMarginal("Height")

# You are whithin Open TURNS : use the distribution the way you want
# Print its caracteristics
print "Height Distribution when Wet Air =", heightDistribution_Wet
print "Probability (height > 40cm) = ", 1-heightDistribution_Wet.computeCDF(40)
print ""
# Draw its PDF and CDF
heightDistributionPDF_Wet = heightDistribution_Wet.drawPDF()
heightDistributionPDF_Wet.setTitle("Height when Wet Air - PDF")
heightDistributionPDF_Wet.setXTitle("height values")
heightDistributionPDF_Wet.setYTitle("density")
heightDistributionPDF_Wet_draw = heightDistributionPDF_Wet.getDrawable(0)
heightDistributionPDF_Wet_draw.setLegendName("")
heightDistributionPDF_Wet.setDrawable(heightDistributionPDF_Wet_draw,0)
heightDistributionPDF_Wet.draw("Height_PDF_WhenWet")
heightDistributionCDF_Wet = heightDistribution_Wet.drawCDF()
heightDistributionCDF_Wet.setTitle("Height when Wet Air - CDF")
heightDistributionCDF_Wet.setXTitle("height values")
heightDistributionCDF_Wet.setYTitle("density")
heightDistributionCDF_Wet_draw = heightDistributionCDF_Wet.getDrawable(0)
heightDistributionCDF_Wet_draw.setLegendName("")
heightDistributionCDF_Wet.setDrawable(heightDistributionCDF_Wet_draw,0)
#heightDistributionCDF_Wet.draw("Height_CDF_WhenWet")


##############################################################
# Example 3 : there is no light : "Light" == "Dim"

# First, add one more  evidence
otbn.setEvidence("Light", "Dim")

# Get the distribution of the variable "Height"
heightDistribution_Dim = otbn.getMarginal("Height")

# You are whithin Open TURNS : use the distribution the way you want
# Print its caracteristics
print "Height Distribution when Dim =", heightDistribution_Dim
print "Probability (height > 40cm) = ", 1-heightDistribution_Dim.computeCDF(40)
print ""
# Draw its PDf and CDF
heightDistributionPDF_Dim = heightDistribution_Dim.drawPDF()
heightDistributionPDF_Dim.setTitle("Height when Dim - PDF")
heightDistributionPDF_Dim.setXTitle("height values")
heightDistributionPDF_Dim.setYTitle("density")
heightDistributionPDF_Dim_draw = heightDistributionPDF_Dim.getDrawable(0)
heightDistributionPDF_Dim_draw.setLegendName("")
heightDistributionPDF_Dim.setDrawable(heightDistributionPDF_Dim_draw,0)
#heightDistributionPDF_Dim.draw("Height_PDF_WhenDim")

heightDistributionCDF_Dim = heightDistribution_Dim.drawCDF()
heightDistributionCDF_Dim.setTitle("Height when Dim - CDF")
heightDistributionCDF_Dim.setXTitle("height values")
heightDistributionCDF_Dim.setYTitle("cdf")
heightDistributionCDF_Dim_draw = heightDistributionCDF_Dim.getDrawable(0)
heightDistributionCDF_Dim_draw.setLegendName("")
heightDistributionCDF_Dim.setDrawable(heightDistributionCDF_Dim_draw,0)
#heightDistributionCDF_Dim.draw("Height_CDF_WhenDim")