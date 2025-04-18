%feature("docstring") OTAGRUM::ContinuousMIIC
"ContinuousMIIC learner.

Parameters
----------
data : 2-d sequence of float
    The data

Notes
-----
MIIC algorithm is a hybrid method between contrained and score based methods.
It allows one to learn the structure of a bayesian network and consists in three steps :
skeleton learning, V-structure searching and constraint propagation. These steps
rely on an information theoretic score."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::PDAGtoDot
"Displays the PDAG in DOT format.

Parameters
----------
pdag : :py:class:`pyagrum.MixedGraph`
    dag

Returns
-------
dot : str
    Dot format representation"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::skeletonToDot
"Displays the skeleton graph to dot

Parameters
----------
pdag : :py:class:`pyagrum.UndiGraph`
    dag

Returns
-------
dot : str
    Dot format representation"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::learnSkeleton
"Learn the skeleton from data.

Returns
-------
gum::UndiGraph : :py:class:`pyagrum.UndiGraph`
    The learned skeleton

Notes
-----
The skeleton is obtained starting from the complete undirected graph on the set of variables
and removing edges which (corrected) conditional mutual information is negative."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::learnPDAG
"Learn the PDAG from data.

Returns
-------
gum::MixedGraph : :py:class:`pyagrum.MixedGraph`
    the learned PDAG

Notes
-----
Starting with the learned skeleton, each triplet of variables
X, Y, Z such that X-Z-Y and such that there is no edge between X and Y is oriented
as X->Z<-Y if its (corrected) three-point conditional information is negative and 
as X->Z->Y (or X<-Z<-Y) if not."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::learnDAG
"Learn the DAG from data.

Returns
-------
dag : :class:`~otagrum.NamedDAG`
    the learned DAG

Notes
-----
This step starts with the learned PDAG and orient the remaining undirected edges
by avoiding to add new V-structures unless it implies to create an oriented circle."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::setVerbosity
"Change the value of verbosity flag. 

Parameters
----------
verbose : bool
    New verbosity flag value. If True, a lot of details about the learning procedure
    are printed."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::getVerbosity
"Returns the verbosity flag value.

Returns
-------
verbose : bool
    Verbosity flag value"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::setCMode
"Change the copula model used for computing corrected mutual information.

Parameters
----------
cmode : CorrectedMutualInformation.CModeTypes
    Copula model (Gaussian or Bernstein)"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::setKMode
"Change the correction mode for mutual information.

Parameters
----------
kmode : CorrectedMutualInformation.CModeTypes
    Correction mode (NoCorr or Naive)"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::setAlpha
"Set the alpha correction value of mutual information when using
the naive KMode.

Parameters
----------
alpha : float
    Correction value"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::getAlpha
"Returns the alpha correction value of mutual information when using
the naive KMode.

Returns
-------
alpha : float
    Correction value"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::addForbiddenArc
"The arc will not be added in the learned DAG.

Parameters
----------
arc : :py:class:`pyagrum.Arc`
head : int
    Head node id
tail : int
    Tail node id
head : str
    Name of the head node
tail : str
    Name of the tail node"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::addMandatoryArc
"The arc will be added in the learned DAG. This allows one to add prior structural
knowledge.

Parameters
----------
arc : :py:class:`pyagrum.Arc`
head : int
    Head node id
tail : int
    Tail node id
head : str
    Name of the head node
tail : str
    Name of the tail node"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousMIIC::getLatentVariables
"Latent variables accessor.

Returns
-------
vars : sequence of :py:class:`pyagrum.Arc`"
