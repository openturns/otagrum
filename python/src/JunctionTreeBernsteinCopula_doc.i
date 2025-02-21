%feature("docstring") OTAGRUM::JunctionTreeBernsteinCopula
"JunctionTreeBernsteinCopula distribution

Available constructors:
    JunctionTreeBernsteinCopula(*junctionTree, copulaSample, binNumber, isCopulaSample*)

    JunctionTreeBernsteinCopula(*cliquesCollection, separatorsCollection, binNumber, isCopulaSample*)

Parameters
----------
junctionTree : a :class:`~otagrum.NamedJunctionTree`
    The junction tree describing the dependence structure of the copula.
copulaSample : a 2-d sequence of float.
    The sample defining the copula
binNumber : int
    The common number of bins for all the Bernstein copulas involved in modelling
    cliques and separators copulas.
isCopulaSample : bool
    Flag to tell if the given sample is already in the normalized rank space.

Notes
-----
This class allows one to build a nonparametric copula from a given sample, a bin
count and a junction tree describing the conditional independencies in the data.
It is based on a Bernstein estimation of each sub-copulas of the model.

Examples
--------
>>> import otagrum
>>> import openturns as ot
>>> import pyagrum as gum
>>> bn = gum.BayesNet.fastPrototype('0->1->2->3;1->4->3;4->5;6->5;7->5')
>>> ie = gum.LazyPropagation(bn)
>>> jtagr = ie.junctionTree()
>>> jt = otagrum.NamedJunctionTree(jtagr, list(bn.names()))
>>> sample = ot.Normal(5).getSample(1000)
>>> binNumber = 5
>>> copula = otagrum.JunctionTreeBernsteinCopula(jt, sample, binNumber, False)"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::JunctionTreeBernsteinCopula::getCopulaSample
"Return the sample defining the (Bernstein) copula.

Returns
-------
copulaSample : :py:class:`openturns.Sample`
    Sample in the normalized rank space."

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::JunctionTreeBernsteinCopula::setCopulaSample
"Set the sample defining the (Bernstein) copula.

Parameters
----------
sample : 2-d sequence of float
    The sample defining the copula.
isCopulaSample : bool
    Flag to tell if the given sample is already in the normalized rank space.
"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::JunctionTreeBernsteinCopula::setCliquesOrder
"Set the order according to which the cliques are traversed.

Parameters
----------
order : :class:`~openturns.Indices`
  The order.
"

// ----------------------------------------------------------------------------

%feature("docstring") OTAGRUM::JunctionTreeBernsteinCopula::getBinNumber
"Return the number of bin which is common to all the local Bernstein copula.
It corresponds to the number of cells into which each dimension of the unit hypercube
is divided to cluster the empirical copula sample.

Returns
-------
binNumber : int
    The number of bins. 
"
