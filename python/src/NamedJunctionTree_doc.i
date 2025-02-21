%feature("docstring") OTAGRUM::NamedJunctionTree
"NamedJunctionTree interface.

Available constructors:
    NamedJunctionTree(*jt, names*)

    NamedJunctionTree(*jt, bn*)

Parameters
----------
jt : :py:class:`pyagrum.CliqueGraph`
    A junction tree.
bn : :py:class:`pyagrum.BayesNet`
    A bayesian network.
names : list
    A list of node names.

Examples
--------
>>> import otagrum
>>> import pyagrum as gum
>>> bn = gum.BayesNet.fastPrototype('0->1->2->3;1->4->3;4->5;6->5;7->5')
>>> ie = gum.LazyPropagation(bn)
>>> jtagr = ie.junctionTree()
>>> njt = otagrum.NamedJunctionTree(jtagr, bn)"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedJunctionTree::getSize
"Return size of the named junction tree.

Returns
-------
size : int
    The number of nodes in the junction tree."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedJunctionTree::getDescription
"Return the description of the named junction tree.

Returns
-------
names : list
    The list of variable names."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedJunctionTree::getClique
"Return id of the variables in the clique.

Parameters
----------
node : int
    Id of the desired node clique.

Returns
-------
clique : :py:class:`~openturns.Indices`
    Indices of the variables in the desired clique node."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedJunctionTree::getSeparator
"Return the separator associated to an edge of the junction tree.

Parameters
----------
node1 : int
    Id of one end of the desired edge.
node2 : int
    Id of the other end of the desired edge.

Returns
-------
separator : :py:class:`~openturns.Indices`
    Separator associated to the desired edge."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedJunctionTree::getNeighbours
"Return the neighbours of a clique node.

Parameters
----------
node : int
    Id of a clique node  in the junction tree.

Returns
-------
neighbours : :py:class:`~openturns.Indices`
    Neighbours of the desired clique node."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedJunctionTree::getNodes
"Return ids of the clique nodes in the junction tree.

Returns
-------
nodes : :py:class:`~openturns.Indices`
    Ids of the clique nodes"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedJunctionTree::getCliquesCollection
"Cliques accessor

Returns
-------
cliques : sequence of :py:class:`openturns.Indices`
    List of cliques"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedJunctionTree::getSeparatorsCollection
"Separators accessor

Returns
-------
separators : sequence of :py:class:`openturns.Indices`
    List of separators"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedJunctionTree::getMarginal
"Return the marginal junction tree on desired variables.

Parameters
----------
nodes : :py:class:`~openturns.Indices`
    Ids of target variables.

Returns
-------
marginal : :class:`~otagrum.NamedJunctionTree`
    Marginal junction tree on target variables."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::NamedJunctionTree::getOrderMaxFirst
"Return an order on the junction tree beginning with the largest clique.

Returns
-------
order : :class:`~openturns.Indices`
    The order.
"
