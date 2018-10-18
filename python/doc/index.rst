OTAgrum documentation
=====================

Introduction
------------

The `aGrUM <https://agrum.gitlab.io>`_ library provides efficient algorithms
to create and manipulate graphical models.
A particular case of such models is the class of Bayesian Networks (BN),
which is of first interest in association with `OpenTURNS <http://openturns.org/>`_.

A *Bayesian network*, *belief network* or *directed acyclic graphical model* is
a probabilistic graphical model that represents a set of random variables and
their conditional dependencies via a directed acyclic graph (DAG).
In this DAG, edges represent conditional dependencies; nodes which are not
connected represent variables which are conditionally independent of each other.
Each node is associated with a probability function that takes as input a
particular set of values for the node's parent variables and gives the
probability of the variable represented by the node.

The manipulation of a Bayesian network is called *inference*.
Efficient algorithms exist that perform inference and learning of Bayesian networks.


User documentation
------------------

.. toctree::
   :maxdepth: 2

   user_manual/user_manual
   examples/examples

Indices and tables
==================

* :ref:`genindex`
* :ref:`search`

