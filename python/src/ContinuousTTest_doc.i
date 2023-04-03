%feature("docstring") OTAGRUM::ContinuousTTest
"ContinuousTTest class.

Parameters
----------
data : 2-d sequence of float
    The data from which the t-test statistics are extracted.
alpha : float
    The confidence level. If not specified, its value is set to 0.1."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousTTest::clearCache
"Clear cache containing the logPDFs of conditional empirical Bernstein copula
(X,Y|Z) that are used to compute the test statistics."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousTTest::clearCacheLevel
"Clear cache for a fixed size of conditioning set (cache level).

Parameters
----------
level : int
    The cache level."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousTTest::getDimension
"Return the dimension of the underlying data set.

Returns
-------
dimension : int
    Dimension of the data set."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousTTest::setAlpha
R"RAW(Change the hypothesis test threshold.

Parameters
----------
alpha : float, :math:`\alpha \in [0, 1]`
    New value of the hypothesis test threshold.)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousTTest::getAlpha
"Return the hypothesis test threshold.

Returns
-------
alpha : float
    Hypothesis test threshold"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousTTest::getDataDescription
"Return the description (variable names) of the underlying data set.

Returns
-------
description : :class:`~openturns.Description`
    Data description."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousTTest::getTTest
"Return the value of t-test statistics for the hypothesis Y indep Z | X.

Parameters
----------
Y : int
    Y node id.
Z : int
    Z node id.
X : list
    list of node ids in the set X.

Returns
-------
ttest : float
    Value of the t-test statistics."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousTTest::getTTestWithoutCorrections
"Return the value of t-test statistics without corrections for the hypothesis
Y indep Z | X.

Parameters
----------
Y : int
    Y node id.
Z : int
    Z node id.
X : list
    list of node ids in the set X.

Returns
-------
ttest : float
    Value of the raw t-test statistics"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousTTest::isIndepFromTest
"Test independence from a given t-test statistics value and a given confidence
threshold.

Parameters
----------
t : float
    A value of t-test statistics.
alpha : float
    A confidence threshold.

Returns
-------
result : tuple
    (t-test, p-value, independent?)"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousTTest::isIndep
"Test the hypothesis Y indep Z | X.

Parameters
----------
Y : int
    Y node id.
Z : int
    Z node id.
X : list
    list of node ids in the set X.

Returns
-------
result : tuple
    (t-test, p-value, independent?)"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousTTest::GetK
"Static method to compute the bin number of an empirical Bernstein copula.

Parameters
----------
size : int
    Size of the sample from which the empirical Bernstein copula is extracted.
dimension : int
    Dimension of the sample from which the empirical Bernstein copula is extracted.

Returns
-------
K : int
    The number of cells into which each dimension of the unit hypercube is divided
    to cluster the empirical copula sample."

// ----------------------------------------------------------------------------

