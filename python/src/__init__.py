"""
    otagrum --- An OpenTURNS module
    ===============================

    Contents
    --------
      'otagrum' is a module for OpenTURNS

"""

# flake8: noqa

# ensures swig type tables order & dll load
import openturns as _ot
import pyAgrum as _gum

from .otagrum import *

__version__ = '0.7'
