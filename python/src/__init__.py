"""
    otagrum --- An OpenTURNS module
    ===============================

    Contents
    --------
      'otagrum' is a module for OpenTURNS

"""

from .otagrum import *
import sys
if sys.platform.startswith('win'):
    # this ensures OT dll is loaded
    import openturns
    # idem for libagrum.dll
    import pyAgrum


__version__ = '0.3'
