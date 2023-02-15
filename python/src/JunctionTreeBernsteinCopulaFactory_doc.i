%feature("docstring") OTAGRUM::JunctionTreeBernsteinCopulaFactory
"Estimate a Junction-Tree Bernstein Copula.

Parameters
----------
nbBins : int
    Number of bins.
alpha : float
    Threshold.
maximumConditioningSetSize : int
    Maximum conditional set size.

Examples
--------
>>> import otagrum
>>> factory = otagrum.JunctionTreeBernsteinCopulaFactory(5, 0.1, 5)"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::JunctionTreeBernsteinCopulaFactory::buildAsJunctionTreeBernsteinCopula
"Build a JunctionTreeBernsteinCopula distribution estimated from a sample.

Parameters
----------
sample : :py:class:`openturns.Sample`
    Input sample.

Returns
-------
distribution : :class:`~otagrum.JunctionTreeBernsteinCopula`
    The estimated JunctionTreeBernsteinCopula distribution."
