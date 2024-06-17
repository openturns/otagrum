%feature("docstring") OTAGRUM::DistributionBayesianNetwork
"DistributionBayesianNetwork class represents a continuous bayesian network (CBN),
that is a bayesian network parameterized by marginal distributions and local
copulas for each node from which conditional copulas are
extracted. The scope of each copula is the node and its parents.

Available constructor:
    DistributionBayesianNetwork(*dag, marginals, copulas*)

Parameters
----------
dag : :class:`~otagrum.NamedDAG`
    The structure of the CBN
joints : sequence of :py:class:`openturns.Distribution`
    Collection of local joint distribution from which the conditional
    distributions are extracted.

Examples
--------
Create a CBN from a DAG structure and a collection of distributions

>>> import otagrum
>>> import openturns as ot
>>> import pyAgrum as gum
>>> bn = gum.BayesNet.fastPrototype('A->C->B;')
>>> ndag = otagrum.NamedDAG(bn)
>>> joints = []
>>> for i in range(ndag.getSize()):
...     dim = 1 + ndag.getParents(i).getSize()
...     R = ot.CorrelationMatrix(dim)
...     for j in range(dim):
...         for k in range(j):
...             R[j, k] = 0.8
...     joints.append(ot.Normal([0.0]*dim, [1.0]*dim, R))
>>> cbn = otagrum.DistributionBayesianNetwork(ndag, joints)

Draw a sample from the joint distribution encoded by the CBN

>>> sample = cbn.getSample(100)"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::DistributionBayesianNetwork::getDAG
"Accessor to the CBN structure.

Returns
-------
dag : :py:class:`pyAgrum.DAG`
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
"The collection of local conditional joints.

Returns
-------
joints : sequence of :py:class:`openturns.Distribution`
    Collection of local joint distributions from which the marginal
    distributions are extracted.
"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::DistributionBayesianNetwork::getJointAtNode
"Accessor to the local conditional distributions.

Parameters
----------
index : int
    Collection index

Returns
-------
copula : `py:class:`openturns.Distribution`
    Local conditional distribution
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
