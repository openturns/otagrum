%feature("docstring") OTAGRUM::Utils
"Utility functions"

%feature("docstring") OTAGRUM::Utils::Discretize
R"RAW(Discretize a continuous distribution.

Compute a probability table according to the domains defined
in a discretized aGrUM variable and a continuous OpenTURNS distribution.

.. math::

   \big\{ p_i \big\} = \big\{ \Prob{t_i < X \leq t_{i+1}}, i \in \left[ 1, n \right] \big\}

where :math:`$t_i$` are the aGrUM ticks, and :math:`$X$` is distributed according to the OpenTURNS distribution.

Parameters
----------
distribution : :py:class:`openturns.Distribution`
    An continuous 1-d distribution.
v : :py:class:`pyagrum.DiscretizedVariable`
    A discrete variable.

Returns
-------
pot : :py:class:`pyagrum.Tensor`
    The resulting potential.

Examples
--------
>>> import pyagrum as gum
>>> import openturns as ot
>>> import otagrum
>>> height = gum.DiscretizedVariable('Height', 'plant growth', [0.0, 10.0, 20.0, 30.0])
>>> height_dist = ot.Normal(18.0, 8.0)
>>> pot = otagrum.Utils.Discretize(height_dist, height)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OTAGRUM::Utils::FromMarginal
"Convert an 1-d probability table to a distribution.

Converts an univariate aGrUM probability table to an OpenTURNS distribution,
either :py:class:`openturns.Histogram` or :py:class:`openturns.UserDefined`.

Parameters
----------
p : :py:class:`pyagrum.Tensor`
    A potential with one variable.

Returns
-------
distribution : :py:class:`openturns.Distribution`
    A distribution.

Examples
--------
>>> import pyagrum as gum
>>> import otagrum
>>> light = gum.LabelizedVariable('Light', 'quality of light', 0).addLabel('Dim').addLabel('Bright')
>>> pot = gum.Tensor().add(light)
>>> pot[:]= [0.25, 0.75]
>>> str(otagrum.Utils.FromMarginal(pot))
'UserDefined({x = [0], p = 0.25}, {x = [1], p = 0.75})'"

// ---------------------------------------------------------------------

%feature("docstring") OTAGRUM::Utils::FromTensor
"Convert an n-d probability table to a distribution.

Converts a multivariate aGrUM probability table to an OpenTURNS distribution,
using :py:class:`openturns.MixedHistogramUserDefined`

Parameters
----------
p : :py:class:`pyagrum.Tensor`
    The potential.

Returns
-------
distribution : :py:class:`openturns.Distribution`
    A distribution.

Examples
--------
>>> import pyagrum as gum
>>> import otagrum
>>> light = gum.LabelizedVariable('Light', 'light', 0).addLabel('Dim').addLabel('Bright')
>>> moisture = gum.LabelizedVariable('Moisture', 'moisture', 0).addLabel('Dry').addLabel('Wet')
>>> pot = gum.Tensor().add(light).add(moisture)
>>> pot[{'Light': 'Dim',    'Moisture': 'Dry'}] = 0.3
>>> pot[{'Light': 'Dim',    'Moisture': 'Wet'}] = 0.2
>>> pot[{'Light': 'Bright', 'Moisture': 'Dry'}] = 0.1
>>> pot[{'Light': 'Bright', 'Moisture': 'Wet'}] = 0.4
>>> dist = otagrum.Utils.FromTensor(pot)"
