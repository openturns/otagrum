%feature("docstring") OTAGRUM::DistributionBayesianNetwork
"Represents a continuous bayesian network.

Each node is associated to a joint distribution over the node and its
parents, from which the conditional distribution of the node given its
parents is extracted.

Parameters
----------
dag : :class:`~otagrum.NamedDAG`
    The structure of the network
joints : sequence of :py:class:`openturns.Distribution`
    Collection of local joint distributions, one per node.
    The joint at node *i* has dimension ``1 + len(parents(i))``,
    its first components correspond to the parents of the node
    in increasing order and its last component corresponds to the node itself.
    For a root node the joint is the univariate marginal of the node.

Examples
--------
Create a network from a DAG structure and a collection of distributions

>>> import otagrum
>>> import openturns as ot
>>> import pyagrum as gum
>>> bn = gum.BayesNet.fastPrototype('A->C->B;')
>>> ndag = otagrum.NamedDAG(bn)
>>> joints = []
>>> for i in range(ndag.getSize()):
...     dim = 1 + ndag.getParents(i).getSize()
...     R = ot.CorrelationMatrix(dim)
...     for j in range(dim):
...         for k in range(j):
...             R[j, k] = 0.8
...     joints.append(ot.Normal([0.0] * dim, [1.0] * dim, R))
>>> dbn = otagrum.DistributionBayesianNetwork(ndag, joints)

Draw a sample from the joint distribution encoded by the network

>>> sample = dbn.getSample(10)"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::DistributionBayesianNetwork::getDAG
"Accessor to the network structure.

Returns
-------
dag : :py:class:`pyagrum.DAG`
    The underlying DAG"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::DistributionBayesianNetwork::setDAGAndJoints
"Accessor to the DAG and the local joints collection.

Parameters
----------
dag : :class:`~otagrum.NamedDAG`
    The underlying NamedDAG
joints : sequence of :py:class:`openturns.Distribution`
    The collection of local joints from which the conditional
    distributions are extracted."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::DistributionBayesianNetwork::getJoints
"The collection of local joints.

Returns
-------
joints : sequence of :py:class:`openturns.Distribution`
    Collection of local joint distributions from which the conditional
    distributions are extracted.
"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::DistributionBayesianNetwork::getJointAtNode
"Accessor to the local joint distributions.

Parameters
----------
index : int
    Collection index

Returns
-------
joint : :py:class:`openturns.Distribution`
    Local joint distribution
"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::DistributionBayesianNetwork::getParents
"Get node parents.

Parameters
----------
nodeId : int
    Node index

Returns
-------
parents : :py:class:`openturns.Indices`
    Parent nodes
"
