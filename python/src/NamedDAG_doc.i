%feature("docstring") OTAGRUM::NamedDAG
"NamedDAG represents a directed acyclic graph (DAG) whose nodes have an
associated name.

Available constructors:
    NamedDAG(*bn*)

    NamedDAG(*dag, names*)

Parameters
----------
bn : :py:class:`pyagrum.BayesNet`
    Bayesian network
dag : :py:class:`pyagrum.DAG`
    Directed Acyclic Graph
names : sequence of str
    Node names"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedDAG::getSize
"The number of nodes in the DAG.

Returns
-------
size : int
    The DAG size"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedDAG::getDescription
"The names of the DAG variables.

Returns
-------
names : list
    The list of node names"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedDAG::getParents
"The parents of a node.

Parameters
----------
node : int
    Node id

Returns
-------
parents : :py:class:`openturns.Indices`
    Indices of parent nodes
"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedDAG::getChildren
"The children of a node.

Parameters
----------
node : int
    Node id

Returns
-------
children : :py:class:`openturns.Indices`
    Indices of children nodes
"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedDAG::getNodes
"Nodes accessor.

Returns
-------
nodes : :py:class:`openturns.Indices`
    List of nodes
"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedDAG::getTopologicalOrder
"The list of the nodes IDs in a topological order.

Returns
-------
nodes : :py:class:`openturns.Indices`
    Indices of nodes
"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedDAG::toDot
"Displays the graph in DOT format.

Returns
-------
dot : str
    DOT graph
"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedDAG::getDAG
"Accessor to the DAG.

Returns
-------
dag : :py:class:`pyagrum.DAG`
    The underlying DAG
"
