%feature("docstring") OTAGRUM::ContinuousPC
"ContinuousPC learner.

Parameters
----------
data : 2-d sequence of float
    The data
maxConditioningSetSize : int, optional
    The maximum conditioning set for independecy test
alpha : float
    Hypothesis test threshold

Notes
-----
PC algorithm is a constrained method allowing to learn the structure of a bayesian
network. It consists in three steps : skeleton learning, V-structure searching
and constraint propagation."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::PDAGtoDot
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

%feature("docstring") OTAGRUM::ContinuousPC::skeletonToDot
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

%feature("docstring") OTAGRUM::ContinuousPC::learnSkeleton
"Learn the skeleton.

Returns
-------
gum::UndiGraph : :py:class:`pyagrum.UndiGraph`
    The learned skeleton

Notes
-----
This step consists in taking the complete undirected graph on the set of variables
and to remove an edge if the linked variables are independent conditionally to
a subset of their neighbors."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::learnPDAG
"Learn the PDAG.

Returns
-------
gum::MixedGraph : :py:class:`pyagrum.MixedGraph`
    the learned PDAG

Notes
-----
This step starts with the learned skeleton and looks at each triplet of variables
X, Y, Z such that X-Z-Y and there is no edge between X and Y. If Z is in the sepset
of X and Y, that is if it was in the conditional set that allowed to remove the edge
between X and Y then we orient the triplet as X->Z<-Y."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::learnJunctionTree
"Learn the junction tree.

Returns
-------
njt : :class:`~otagrum.NamedJunctionTree`
    the learned junction tree

Notes
-----
In order to learn the junction tree, the PDAG is moralized and then triangulated
giving an undirected graph. Each clique in this graph is then associated to a
node in the junction tree and there is an edge between nodes sharing common
variables. Finally to each edge is associated a separator containing the intersection
of variables in the two linked nodes."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::learnDAG
"Learn the DAG.

Returns
-------
dag : :class:`~otagrum.NamedDAG`
    the learned DAG

Notes
-----
This step starts with the learned PDAG and orient the remaining undirected edges
by avoiding to add new V-structures unless it implies to create an oriented circle."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::getPValue
"P-value accessor.

Parameters
----------
x : str
    A variable's name
y : str
    A variable's name
x : int
    A variable's id
y : int
    A variable's id

Returns
-------
p_value : float
    P-value of edge x - y"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::getSepsetNames
"Separator set names accessor.

Parameters
----------
x : str
    A variable's name
y : str
    A variables's name

Returns
-------
sepset_names : :py:class:`openturns.Description`
    The names of variables in the separator set of variables x and y"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::getSepset
"Separator set indices accessor.

Parameters
----------
x : int
    Node id
y : int
    Node id

Returns
-------
sep_set : :py:class:`openturns.Indices`
    Indices of variables in the separator set of variables x and y"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::getTTest
"T-Test statistics accessor.

Parameters
----------
x : str
    A variable's name
y : str
    A variable's name
x : int
    A variable's id
y : int
    A variable's id

Returns
-------
ttest : float
    T-Test statistics value of edge x - y"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::setVerbosity
"Change the value of verbosity flag. 

Parameters
----------
verbose : bool
    New verbosity flag value. If True, a lot of details about the learning procedure
    are printed."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::getVerbosity
"Returns the verbosity flag value.

Returns
-------
verbose : bool
    Verbosity flag value"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::getTrace
"Returns a summary of the learning algorithm containing which conditional indepences
have been found and the associated T-test and p-values.

Returns
-------
trace : :py:class:`openturns.Description`
    Trace of the learning algorithm"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::idFromName
"Id accessor.

Parameters
----------
n : str
    Node name

Returns
-------
id : int
    Id of the node"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousPC::isRemoved
"Test if an arc is removed.

Parameters
----------
x : str
    A variable's name
y : str
    A variable's name
x : int
    A variable's id
y : int
    A variable's id

Returns
-------
removed : bool
    Whether the arc x-y has been removed, this is equivalent to x and y
    being independent"
