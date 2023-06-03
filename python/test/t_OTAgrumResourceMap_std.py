#!/usr/bin/env python

import openturns as ot
import otagrum

print(otagrum.__version__)
key = "JunctionTreeBernsteinCopulaFactory-DefaultBinNumber"
assert ot.ResourceMap.HasKey(key), "missing RM key"
