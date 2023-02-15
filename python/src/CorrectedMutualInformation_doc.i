%feature("docstring") OTAGRUM::CorrectedMutualInformation
"CorrectedMutualInformation class.

Parameters
----------
data : 2-d sequence of float
    The data set."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::CorrectedMutualInformation::setAlpha
"Displays the skeleton graph to dot

Parameters
----------
pdag : :py:class:`pyAgrum.UndiGraph`
    dag

Returns
-------
dot : str
    Dot format representation"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::CorrectedMutualInformation::getAlpha
"Returns the alpha correction value of mutual information when using the Naive KMode.

Returns
-------
alpha : float
    Correction value."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::CorrectedMutualInformation::getSize
"Returns the size of the underlying data set.

Returns
-------
size : int
    Size of the data set."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::CorrectedMutualInformation::getDimension
"Returns the dimension of the underlying data set.

Returns
-------
dimension : int
    Dimension of the data set."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::CorrectedMutualInformation::getDataDescription
"Return the description (variable names) of the underlying data set.

Returns
-------
description : :class:`~openturns.Description`
    Data description."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::CorrectedMutualInformation::compute2PtCorrectedInformation
"Return the corrected mutual information between variables X and Y conditionally
to the variable set U (possibly empty).

Parameters
----------
X : int
    X node id.
Y : int
    Y node id.
U : list
    List of node ids in the (possibly empty) set U

Returns
-------
info : float
    The corrected mutual information I'(X;Y|U).

Notes
-----
The mutual information is computed from the cross-entropy H of the copula using the relation
I(X;Y|U) = H(X,U) + H(Y,U) - H(X,Y,U) - H(U)."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::CorrectedMutualInformation::compute3PtCorrectedInformation
"Return the corrected three-point information between variables X, Y and Z conditionally
to the variable set U (possibly empty).

Parameters
----------
X : int
    X node id.
Y : int
    Y node id.
Z : int
    Z node id.
U : list
    List of node ids in the (possibly empty) set U

Returns
-------
info : float
    The corrected three-point information I'(X;Y;Z|U).

Notes
-----
The three-point information is computed from mutual information using the relation
I(X;Y;Z|U) = I(X;Y|U) - I(X;Y|U,Z)."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::CorrectedMutualInformation::clearCaches
"Clear cache containing the cross-entropies that are used to compute mutual information."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::CorrectedMutualInformation::setKMode
"Change the correction mode for mutual information.

Parameters
----------
kmode : CorrectedMutualInformation.KModeTypes
    Correction mode (NoCorr or Naive)"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::CorrectedMutualInformation::setCMode
"Change the copula model used for computing corrected mutual information.

Parameters
----------
cmode : CorrectedMutualInformation.CModeTypes
    Copula model (Gaussian or Bernstein)."
