#################################################################################
#
# The MIT License (MIT)
#
# Copyright (c) 2015 Dmitry Sovetov
#
# https://github.com/dmsovetov
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#################################################################################

import patcher

initial_module = dict(
    gravityModifier=patcher.curve('gravity', patcher.multiply(12)),
    maxNumParticles=patcher.field('maxParticles'),
    startLifetime=patcher.curve('life'),
    startSpeed=patcher.curve('speed', patcher.rescale),
    startColor=patcher.gradient('rgb', 'alpha'),
    startSize=patcher.curve('size', patcher.rescale),
    startRotation=patcher.curve('rotation', patcher.radians_to_degrees),
    )

emission_module = {
    'rate': patcher.curve('rate'),
    '': patcher.bursts('bursts'),
    }

size_module = dict(
    curve=patcher.curve('curve')
)

rotation_module = dict(
    curve=patcher.curve('curve', patcher.radians_to_degrees)
)

color_module = dict(
    gradient=patcher.gradient('rgb', 'alpha')
)

velocity_module = dict(
    x=patcher.curve('x', patcher.rescale),
    y=patcher.curve('y', patcher.rescale),
    z=patcher.curve('z', patcher.rescale),
    )

force_module = dict(
    x=patcher.curve('x', patcher.rescale),
    y=patcher.curve('y', patcher.rescale),
    z=patcher.curve('z', patcher.rescale),
    )

shape_module = dict(
    type=patcher.field('type'),
    radius=patcher.field('radius', patcher.rescale),
    angle=patcher.field('angle'),
    length=patcher.field('length', patcher.rescale),
    boxX=patcher.field('width', patcher.rescale),
    boxY=patcher.field('height', patcher.rescale),
    boxZ=patcher.field('depth', patcher.rescale),
    arc=patcher.field('arc'),
    randomDirection=patcher.field('randomDirection'),
    )

clamp_velocity = dict(
    magnitude=patcher.curve('magnitude', patcher.rescale)
    )

particle_system = dict(
    lengthInSec=patcher.field('duration'),
    speed=patcher.field('velocity'),
    looping=patcher.field('isLooped'),
    moveWithTransform=patcher.field('isLocal'),
    InitialModule=patcher.if_enabled('initial', initial_module),
    EmissionModule=patcher.if_enabled('emission', emission_module),
    SizeModule=patcher.if_enabled('size', size_module),
    RotationModule=patcher.if_enabled('angularVelocity', rotation_module),
    ColorModule=patcher.if_enabled('color', color_module),
    VelocityModule=patcher.if_enabled('velocity', velocity_module),
    ForceModulepatcher=patcher.if_enabled('acceleration', force_module),
    ClampVelocityModule=patcher.if_enabled('limitVelocity', clamp_velocity),
    ShapeModule=patcher.if_enabled('shape', shape_module),
    m_GameObject=patcher.reference('sceneObject'),
    )

