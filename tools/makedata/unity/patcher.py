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

import math
from parameter import ParameterType, Curve, Gradient

# Patches the object properties
class Patcher:
    # Renames the property
    class rename:
        # __init__
        def __init__(self, name, converter=None):
            self.name = name
            self.converter = converter

        # __call__
        def __call__(self, assets, object, property):
            if self.name == property:
                return

            object[self.name] = self.converter(object[property]) if self.converter is not None else object[property]
            del object[property]

    # Patches an object
    class object:
        # __init__
        def __init__(self, name, patchers):
            self.name = name
            self.patchers = patchers

        # __call__
        def __call__(self, assets, object, property):
            value = object[property]

            object[self.name] = Patcher.patch(assets, value, self.patchers)
            del object[property]

    # Patches the particle system module
    class module:
        # __init__
        def __init__(self, name, patchers):
            self.name = name
            self.patchers = patchers

        # __call__
        def __call__(self, assets, obj, property):
            value = obj[property]

            if 'enabled' not in value or value['enabled'] == 0:
                del obj[property]
                return

            obj[self.name] = Patcher.patch(assets, value, self.patchers)
            del obj[property]

    # Formats an asset
    class asset:
        # __init__
        def __init__(self, name):
            self.name = name

        # __call__
        def __call__(self, assets, object, property):
            value = object[property]

            if isinstance(value, dict):
                object[self.name] =  assets.use(value['guid']) if 'guid' in value.keys() else None
            else:
                object[self.name] = [assets.use(item['guid']) for item in value]

            del object[property]

    # Formats the reference
    class reference:
        # __init__
        def __init__(self, name):
            self.name = name

        # __call__
        def __call__(self, assets, object, property):
            objectId = str(object[property]['fileID'])
            object[self.name] = None if objectId == '0' else objectId
            del object[property]

    # Formats the vector
    class vector:
        # __init__
        def __init__(self, name, coords):
            self.name = name
            self.coords = coords

        # __call__
        def __call__(self, assets, object, property):
            value  = object[property]
            result = []

            for coord in self.coords:
                if coord in value.keys(): result.append(float(value[coord]))

            object[self.name] = result
            del object[property]

    # Formats the curve
    class curve:
        # __init__
        def __init__(self, name, converter=None):
            self.name = name
            self.converter = converter

        # __call__
        def __call__(self, assets, object, property):
            value  = object[property]
            result = []

            scalar = value['scalar']
            type   = value['minMaxState']
            min    = Curve()
            max    = Curve()

            min.parse(value['minCurve'], scalar, self.converter)
            max.parse(value['maxCurve'], scalar, self.converter)

            if type == ParameterType.CONSTANT: object[self.name] = max.constant
            elif type == ParameterType.RANDOM_BETWEEN_CONSTANTS: object[self.name] = [min.constant, max.constant]
            elif type == ParameterType.CURVE: object[self.name] = max.data
            elif type == ParameterType.RANDOM_BETWEEN_CURVES: object[self.name] = [min.data, max.data]

            if property != self.name:
                del object[property]

    # Formats emitter bursts
    class bursts:
        # __init__
        def __init__(self, name):
            self._name = name

        # __call__
        def __call__(self, assets, object, property):
            result = []

            for i in range(0, object['m_BurstCount']):
                result.append(object['time' + str(i)])
                result.append(object['cnt'  + str(i)])

            object[self._name] = result

    # Formats the gradient
    class gradient:
        # __init__
        def __init__(self, color_name, transparency_name):
            self._color_name = color_name
            self._transparency_name = transparency_name

        # __call__
        def __call__(self, assets, object, property):
            value  = object[property]
            result = []

            type   = value['minMaxState']
            min    = Gradient()
            max    = Gradient()

            min.parse(value['minGradient'])
            max.parse(value['maxGradient'])

            assert 'transparency' not in object.keys()

            if type == ParameterType.CURVE:
                object[self._color_name] = max.rgb
                object[self._transparency_name] = max.alpha
            elif type == ParameterType.RANDOM_BETWEEN_CURVES:
                object[self._color_name] = [min.rgb, max.rgb]
                object[self._transparency_name] = [min.alpha, max.alpha]
            else:
                object[self._color_name] = max.constant_rgb
                object[self._transparency_name] = max.constant_alpha

            del object[property]

    # Formats the shader name
    class shader:
        # __init__
        def __init__(self, name):
            self.name = name

        # __call__
        def __call__(self, assets, object, property):
            shaderById = {
        	    7:      'diffuse'
    	    ,   3:      'specular'
    	    ,   30:     'transparent.diffuse'
    	    ,   32:     'transparent.specular'
    	    ,   51:     'transparent.cutout.diffuse'
    	    ,   53:     'transparent.cutout.specular'
    	    ,   200:    'additive'
    	    ,   202:    'additive.soft'
    	    ,   10:     'emissive'
            ,   12:     'emissive.specular'
   		    ,   10511:  'nature.treeSoftOcclusionLeaves'
            ,   10720:  'additive'
            ,   10721:  'transparent'
		    }

            object[self.name] = shaderById[object[property]['fileID']]
            del object[property]

    # Formats the key-value properties
    class keyValue:
        # __init__
        def __init__(self, name, patchers):
            self.name = name
            self.patchers = patchers

        # __call__
        def __call__(self, assets, object, property):
            result = {}

            for prop in object[property]:
                key = prop['first']['name']
                value = {}

                if key in self.patchers.keys():
                    value[key] = prop['second']
                    value = Patcher.patch(assets, value, self.patchers)
                    result[value.keys()[0]] = value[value.keys()[0]]

            object[self.name] = result
            del object[property]

    @staticmethod
    def patch(assets, object, patchers):
        object_items = object.items()

        if '' in patchers.keys():
            patchers[''](assets, object, '')

        for name, value in object_items:
            if name in patchers.keys():
                patchers[name](assets, object, name)
            else:
                del object[name]

        return object

# Material
MaterialPatcher = {
	  'm_Name': Patcher.rename('name')
    , 'm_Shader': Patcher.shader('shader')
}

# Material texture patcher
MaterialTexturePatcher = {
      'm_Texture': Patcher.asset('asset')
    , 'm_Scale': Patcher.vector('scale', ['x', 'y'])
    , 'm_Offset': Patcher.vector('offset', ['x', 'y'])
}

# Material textures
MaterialTexturesPatcher = {
    '_MainTex': Patcher.object('diffuse', MaterialTexturePatcher)
}

# Material parameters
MaterialParametersPatcher = {
    '_Shininess': Patcher.rename('shininess')
}

# Material colors
MaterialColorsPatcher = {
      '_Color': Patcher.vector('diffuse', ['r', 'g', 'b', 'a'])
    , '_SpecColor': Patcher.vector('specular', ['r', 'g', 'b', 'a'])
    , '_TintColor': Patcher.vector('tint', ['r', 'g', 'b', 'a'])
}

# Material properties
MaterialPropertiesPatcher = {
	  'm_TexEnvs': Patcher.keyValue('textures', MaterialTexturesPatcher)
    , 'm_Floats': Patcher.keyValue('parameters', MaterialParametersPatcher)
    , 'm_Colors': Patcher.keyValue('colors', MaterialColorsPatcher)
}

# Renderer patcher
RendererPatcher = {
	  'm_CastShadows': Patcher.rename('castShadows')
	, 'm_ReceiveShadows': Patcher.rename('receiveShadows')
	, 'm_Materials': Patcher.asset('materials')
	, 'm_Enabled': Patcher.rename('enabled')
	, 'm_GameObject': Patcher.reference('sceneObject')
}

# Transform
TransformPatcher = {
	  'm_LocalRotation': Patcher.vector('rotation', ['x', 'y', 'z', 'w'])
	, 'm_LocalScale': Patcher.vector('scale', ['x', 'y', 'z'])
	, 'm_LocalPosition': Patcher.vector('position', ['x', 'y', 'z'])
	, 'm_Enabled': Patcher.rename('enabled')
	, 'm_GameObject': Patcher.reference('sceneObject')
    , 'm_Father': Patcher.reference('parent')
}

# GameObject
GameObjectPatcher = {
	  'm_Name': Patcher.rename('name')
	, 'm_IsActive': Patcher.rename('active')
	, 'm_Layer': Patcher.rename('layer')
	, 'm_StaticEditorFlags': Patcher.rename('flags')
}

# Light
LightPatcher = {
	  'm_Intensity': Patcher.rename('intensity')
	, 'm_SpotAngle': Patcher.rename('spotAngle')
	, 'm_Type': Patcher.rename('type')
	, 'm_Range': Patcher.rename('range')
	, 'm_Color': Patcher.vector('color', ['r', 'g', 'b', 'a'])
	, 'm_Lightmapping': Patcher.rename('backed')
	, 'm_Enabled': Patcher.rename('enabled')
	, 'm_GameObject': Patcher.reference('sceneObject')
}

# Camera
CameraPatcher = {
	  'near clip plane': Patcher.rename('near')
	, 'far clip plane': Patcher.rename('far')
	, 'field of view': Patcher.rename('fov')
	, 'm_OcclusionCulling': Patcher.rename('occlusionCulling')
	, 'm_BackGroundColor': Patcher.vector('backgroundColor', ['r', 'g', 'b', 'a'])
	, 'm_NormalizedViewPortRect': Patcher.vector('ndc', ['x', 'y', 'width', 'height'])
	, 'm_Enabled': Patcher.rename('enabled')
	, 'm_GameObject': Patcher.reference('sceneObject')
}

def radians_to_degrees(value):
    return value * 180 / math.pi

def rescale(value):
    return value * 75

def multiply(factor):
    def _(value):
        return rescale(value * factor)
    return _

# InitialModulePatcher
InitialModulePatcher = {
      'gravityModifier': Patcher.rename('gravity', multiply(20))
    , 'maxNumParticles': Patcher.rename('maxParticles')
    , 'startLifetime': Patcher.curve('life')
    , 'startSpeed': Patcher.curve('speed', rescale)
    , 'startColor': Patcher.gradient('rgb', 'alpha')
    , 'startSize': Patcher.curve('size', rescale)
    , 'startRotation': Patcher.curve('rotation', radians_to_degrees)
}

# EmissionModulePatcher
EmissionModulePatcher = {
      'rate': Patcher.curve('rate')
    , '':     Patcher.bursts('bursts')
}

# SizeModulePatcher
SizeModulePatcher = {
    'curve': Patcher.curve('curve')
}

# RotationModulePatcher
RotationModulePatcher = {
    'curve': Patcher.curve('curve', radians_to_degrees)
}

# ColorModulePatcher
ColorModulePatcher = {
    'gradient': Patcher.gradient('rgb', 'alpha')
}

# VelocityModulePatcher
VelocityModulePatcher = {
      'x': Patcher.curve('x', rescale)
    , 'y': Patcher.curve('y', rescale)
    , 'z': Patcher.curve('z', rescale)
}

# ForceModulePatcher
ForceModulePatcher = {
      'x': Patcher.curve('x', rescale)
    , 'y': Patcher.curve('y', rescale)
    , 'z': Patcher.curve('z', rescale)
}

# ShapeModulePatcher
ShapeModulePatcher = {
      'type': Patcher.rename('type')
    , 'radius': Patcher.rename('radius', rescale)
    , 'angle': Patcher.rename('angle')
    , 'length': Patcher.rename('length', rescale)
    , 'boxX': Patcher.rename('width', rescale)
    , 'boxY': Patcher.rename('height', rescale)
    , 'boxZ': Patcher.rename('depth', rescale)
    , 'arc': Patcher.rename('arc')
    , 'randomDirection': Patcher.rename('randomDirection')
}

# ParticleSystem
ParticleSystemPatcher = {
      'lengthInSec': Patcher.rename('duration')
    , 'speed': Patcher.rename('velocity')
    , 'looping': Patcher.rename('isLooped')
    , 'moveWithTransform': Patcher.rename('isLocal')
    , 'InitialModule': Patcher.module('initial', InitialModulePatcher)
    , 'EmissionModule': Patcher.module('emission', EmissionModulePatcher)
    , 'SizeModule': Patcher.module('size', SizeModulePatcher)
    , 'RotationModule': Patcher.module('angularVelocity', RotationModulePatcher)
    , 'ColorModule': Patcher.module('color', ColorModulePatcher)
    , 'VelocityModule': Patcher.module('velocity', VelocityModulePatcher)
    , 'ForceModulePatcher': Patcher.module('acceleration', ForceModulePatcher)
    , 'ShapeModule': Patcher.module('shape', ShapeModulePatcher)
    , 'm_GameObject': Patcher.reference('sceneObject')
}

# ParticleSystemRendererPatcher
ParticleSystemRendererPatcher = {
      'm_Materials': Patcher.asset('materials')
    , 'm_Enabled': Patcher.rename('enabled')
    , 'm_GameObject': Patcher.reference('sceneObject')
}

# Scene patchers
ScenePatchers = {
	  'Renderer': RendererPatcher
    , 'Transform': TransformPatcher
    , 'GameObject': GameObjectPatcher
    , 'Light': LightPatcher
    , 'Camera': CameraPatcher
    , 'ParticleSystem': ParticleSystemPatcher
    , 'ParticleSystemRenderer': ParticleSystemRendererPatcher
}