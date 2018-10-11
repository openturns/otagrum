"""
    otagrum --- An OpenTURNS module
    ===============================

    Contents
    --------
      'otagrum' is a module for OpenTURNS

"""

import sys
if sys.platform.startswith('win'):
    # this ensures OT dll is loaded
    import openturns

from .otagrum import *

__version__ = '0.2.0'

