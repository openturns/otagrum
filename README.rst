.. image:: https://travis-ci.org/openturns/otagrum.svg?branch=master
    :target: https://travis-ci.org/openturns/otagrum

otagr Module
=================

otagr is a module for mixing OpenTURNS and aGrUM

More information can found at http://www.openturns.org and http//ww.agrum.org


Installation
============
install openturns
install aGrUM and pyAgrum
(check your PYTHONPATH for pyAgrum and openturns)

mkdir build
cd build
cmake ..  -DCMAKE_INSTALL_PREFIX=install
make check
make installcheck

-- The OpenTURNS/pyAgrum team
