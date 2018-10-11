"""
    otagr --- An OpenTURNS module
    ==================================

    Contents
    --------
      'otagr' is a module for OpenTURNS

"""

import sys
if sys.platform.startswith('win'):
    # this ensures OT dll is loaded
    import openturns

if sys.platform.startswith('win'):
  import pyAgrum

from .otagrum import *

__version__ = '0.2.0'

