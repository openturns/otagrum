%feature("docstring") OTAGRUM::ContinuousBayesianNetwork
"ContinuousBayesianNetwork class represents a continuous bayesian network (CBN),
that is a bayesian network parameterized by marginal distributions and local
copulas for each node from which conditional copulas are
extracted. The scope of each copula is the node and its parents.

Available constructor:
    ContinuousBayesianNetwork(*dag, marginals, copulas*)

Parameters
----------
dag : :class:`~otagrum.NamedDAG`
    The structure of the CBN
marginals : sequence of :py:class:`openturns.Distribution`
    Collection of univariate marginal distributions
copulas : sequence of :py:class:`openturns.Distribution`
    Collection of local conditional copula functions. The copula is
    unused for nodes with no parents.

Examples
--------
Create a CBN from a DAG structure and a collection of distributions

>>> import otagrum
>>> import openturns as ot
>>> import pyAgrum as gum
>>> bn = gum.BayesNet.fastPrototype('A->C->B;')
>>> ndag = otagrum.NamedDAG(bn)
>>> marginals = [ot.Uniform(0.0, 1.0) for i in range(ndag.getSize())]
>>> copulas = []
>>> for i in range(ndag.getSize()):
...     dim = 1 + ndag.getParents(i).getSize()
...     R = ot.CorrelationMatrix(dim)
...     for j in range(dim):
...         for k in range(j):
...             R[j, k] = 0.8
...     copulas.append(ot.Normal([0.0]*dim, [1.0]*dim, R).getCopula())
>>> cbn = otagrum.ContinuousBayesianNetwork(ndag, marginals, copulas)

Draw a sample from the joint distribution encoded by the CBN

>>> sample = cbn.getSample(100)"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousBayesianNetwork::getNamedDAG
"The CBN structure.

Returns
-------
dag : :class:`~otagrum.NamedDAG`
    The underlying NamedDAG"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousBayesianNetwork::setDAGAndMarginalsAndCopulas
"Accessor to the DAG, the marginals and the local copulas collections.

Parameters
----------
dag : :class:`~otagrum.NamedDAG`
    The underlying NamedDAG
marginals : sequence of :py:class:`openturns.Distribution`
    The collection of univariate marginal distributions
copulas : sequence of :py:class:`openturns.Distribution`
    The collection of local conditional copulas"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousBayesianNetwork::getMarginals
"The collection of univariate marginal distributions.

Returns
-------
marginals : sequence of :py:class:`openturns.Distribution`
    Collection of univariate marginal distributions
"
// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::ContinuousBayesianNetwork::getCopulas
"The collection of local conditional copulas.

Returns
-------
copulas : sequence of :py:class:`openturns.Distribution`
    Collection of local conditional copulas
"
