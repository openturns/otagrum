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


What is otagrum ?
-----------------

The otagrum module is the link between Bayesian networks built with aGrUM and distributions defined with OpenTURNS.

It offers the ability to:

  - define discretized aGrUM distributions from OpenTURNS distributions
  - extract marginal distributions of aGrUM Bayesian networks as OpenTURNS distributions
  - define and estimate bayesian networks parameterized by local conditional copula functions for each node (CBN)

How to install it ?
-------------------

Conda
^^^^^

Using miniforge, you need to run the following command:

.. code-block:: console

   $ conda install otagrum

Manually (Linux)
^^^^^^^^^^^^^^^^

Create a new directory where you will clone all the necessary repositories:

.. code-block:: console

   $ mkdir git-repos
   $ cd git-repos
   $ git clone https://gitlab.com/agrumery/aGrUM.git
   $ git clone https://github.com/openturns/openturns.git
   $ git clone https://github.com/openturns/otagrum.git

First, you need to install aGrUM and pyAgrum:

.. code-block:: console

   $ cd aGrUM
   $ ./act install release aGrUM
   $ ./act install release pyAgrum

For more details, you can check the `aGrUM installation page <https://agrum.gitlab.io/pages/pyagrum-installation.html>`_.

Next, you need to install OpenTURNS:

.. code-block:: console

   $ cd ../openturns
   $ cmake -B build .
   $ cmake --build build

For more details, you can check this `page <http://openturns.github.io/openturns/latest/install.html>`_.

Finally, you can install otagrum:

.. code-block:: console

    $ cd ../otagrum
    $ cmake -B build .
    $ cmake --build build


User documentation
------------------

.. toctree::
   :maxdepth: 2

   user_manual/user_manual
   auto_examples/index

Indices and tables
==================

* :ref:`genindex`
* :ref:`search`

