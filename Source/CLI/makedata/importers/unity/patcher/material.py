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

shader = {
    '7': 'diffuse',
    '3': 'specular',
    '30': 'transparent.diffuse',
    '32': 'transparent.specular',
    '51': 'transparent.cutout.diffuse',
    '53': 'transparent.cutout.specular',
    '200': 'additive',
    '202': 'additive.soft',
    '10': 'emissive',
    '12': 'emissive.specular',
    '10511': 'nature.treeSoftOcclusionLeaves',
    '10720': 'additive',
    '10721': 'transparent',
    }

material = dict(
    m_Name=patcher.field('name'),
    m_Shader=patcher.file_id('shader', patcher.map_to_string(shader)),
    )

texture = dict(
    m_Texture=patcher.asset('asset'),
    m_Scale=patcher.vector('scale', ['x', 'y']),
    m_Offset=patcher.vector('offset', ['x', 'y']),
    )

textures = dict(
    _MainTex=patcher.patch('diffuse', texture)
    )

parameters = dict(
    _Shininess=patcher.field('shininess')
    )

colors = dict(
    _Color=patcher.vector('diffuse', ['r', 'g', 'b', 'a']),
    _SpecColor=patcher.vector('specular', ['r', 'g', 'b', 'a']),
    _TintColor=patcher.vector('tint', ['r', 'g', 'b', 'a']),
    )

properties = dict(
    m_TexEnvs=patcher.key_value('textures', textures),
    m_Floats=patcher.key_value('parameters', parameters),
    m_Colors=patcher.key_value('colors', colors),
    )
