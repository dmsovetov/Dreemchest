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
import particles

renderer = dict(
    m_CastShadows=patcher.field('castShadows'),
    m_ReceiveShadows=patcher.field('receiveShadows'),
    m_Materials=patcher.asset('materials'),
    m_Enabled=patcher.field('enabled'),
    m_GameObject=patcher.reference('sceneObject'),
    )

transform = dict(
    m_LocalRotation=patcher.vector('rotation', ['x', 'y', 'z', 'w']),
    m_LocalScale=patcher.vector('scale', ['x', 'y', 'z']),
    m_LocalPosition=patcher.vector('position', ['x', 'y', 'z']),
    m_Enabled=patcher.field('enabled'),
    m_GameObject=patcher.reference('sceneObject'),
    m_Father=patcher.reference('parent'),
    )

game_object = dict(
    m_Name=patcher.field('name'),
    m_IsActive=patcher.field('active'),
    m_Layer=patcher.field('layer'),
    m_StaticEditorFlags=patcher.field('flags'),
    )

light = dict(
    m_Intensity=patcher.field('intensity'),
    m_SpotAngle=patcher.field('spotAngle'),
    m_Type=patcher.field('type'),
    m_Range=patcher.field('range'),
    m_Color=patcher.vector('color', ['r', 'g', 'b', 'a']),
    m_Lightmapping=patcher.field('backed'),
    m_Enabled=patcher.field('enabled'),
    m_GameObject=patcher.reference('sceneObject'),
    )

camera = {
    'near clip plane': patcher.field('near'),
    'far clip plane': patcher.field('far'),
    'field of view': patcher.field('fov'),
    'm_OcclusionCulling': patcher.field('occlusionCulling'),
    'm_BackGroundColor': patcher.vector('backgroundColor', ['r', 'g', 'b', 'a']),
    'm_NormalizedViewPortRect': patcher.vector('ndc', ['x', 'y', 'width', 'height']),
    'm_Enabled': patcher.field('enabled'),
    'm_GameObject': patcher.reference('sceneObject'),
}

particles_renderer = dict(
    m_Materials=patcher.asset('materials'),
    m_Enabled=patcher.field('enabled'),
    m_GameObject=patcher.reference('sceneObject'),
    )

scene = dict(
    Renderer=renderer,
    Transform=transform,
    GameObject=game_object,
    Light=light,
    Camera=camera,
    ParticleSystem=particles.particle_system,
    ParticleSystemRenderer=particles_renderer
    )
