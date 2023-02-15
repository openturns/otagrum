%feature("docstring") OTAGRUM::TabuList
"TabuList learner.

Parameters
----------
data : 2-d sequence of float
    The data
initial_dag: :class:`~otagrum.NamedDAG`
    The Initial DAG for the algorithm
max_parents : int
    Maximum parents that a node can have
restarts : int
    Number of random restarts
tabu_list_size : int
    Size of the TABU list

Notes
-----
The TABU list algorithm is used with the BIC score and Gaussian copulas to learn the 
structure of a CBN from data. Moreover, it uses the decomposition of BIC score
to avoid computing score over the entire graph."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::TabuList::learnDAG
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

%feature("docstring") OTAGRUM::TabuList::setVerbosity
"Change the value of verbosity flag. 

Parameters
----------
verbose : bool
    New verbosity flag value. If True, a lot of details about the learning procedure
    are printed."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::TabuList::getVerbosity
"Returns the verbosity flag value.

Returns
-------
verbose : bool
    Verbosity flag value"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::TabuList::getMaxParents
"Returns the value of the parameter max_parents.

Returns
-------
max_parents : int
    Maximum number of parents"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::TabuList::getRestarts
"Returns the value of the parameter restarts.

Returns
-------
restarts : int
    Restarts value"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::TabuList::getBestScore
"Returns the final score value.

Returns
-------
score : double
    Final score value"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::TabuList::setCMode
"Changes the copula model (Bernstein or Gaussian) used for computing the BIC score.

Returns
-------
verbose : CorrectedMutualInformation.CModeTypes
    Copula model (Gaussian or Bernstein)"
