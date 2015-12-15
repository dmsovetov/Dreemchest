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

import os, json, collections, shutil, actions, module, math, yaml, patcher

from assets import Assets
from asset_type import AssetType
from scene import Scene

# Holds the material info
class material:
    # Converts material to JSON
    @staticmethod
    def convert(assets, source, output):
        objects = yaml.objects_from_file(source)
        assert len(objects) == 1
        result = None

        for k, v in objects.items():
            result   = v['Material']
            properties = patcher.Patcher.patch(assets, result['m_SavedProperties'], patcher.MaterialPropertiesPatcher)
            result = patcher.Patcher.patch(assets, result, patcher.MaterialPatcher)

            for k, v in properties.items():
                result[k] = v

        # Save parsed scene to a JSON file
        yaml.save_to_json(output, result)

# Emitter shape type enumeration
class ShapeType:
    SPHERE = 0
    SPHERE_SHELL = 1
    HEMISPHERE = 2
    HEMISPHERE_SHELL = 3
    CONE = 4
    CONE_SHELL = 7
    CONE_VOLUME = 8
    CONE_VOLUME_SHELL = 9
    BOX = 5
    MESH = 6
    CIRCLE = 10
    EDGE = 12

# The particle system property value
class Value:
    # Constructs the ConstantParameter instance
    def __init__(self, value=None):
        self._value = value

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return self._value

    # Parses the parameter from an object
    def parse(self, object, converter):
        self._value = object

        if converter is not None:
            self._value = converter(self._value)

# The particle system scalar parameter
class ScalarParameter:
    # Constructs the ScalarParameter instance
    def __init__(self, type):
        self._type = type
        self._min = Curve()
        self._max = Curve()

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return dict(type=self._type, min=self._min.data, max=self._max.data)

    # Parses the parameter from an object
    def parse(self, object, converter):
        self._min.parse(object['minCurve'], object['scalar'], converter)
        self._max.parse(object['maxCurve'], object['scalar'], converter)

# The particle system color parameter
class GradientParameter:
    # Constructs the GradientParameter instance
    def __init__(self, type):
        self._type = type
        self._min = Gradient()
        self._max = Gradient()

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return dict(type=self._type, min=self._min.data, max=self._max.data)

    # Parses the parameter from an object
    def parse(self, object, converter):
        self._min.parse(object['minGradient'], converter)
        self._max.parse(object['maxGradient'], converter)

# Particle system module
class ParticleSystemModule:
    # Constructs the ParticleSystemModule instance
    def __init__(self, name):
        self._name = name
        self._parameters = dict()

    # Returns the module name
    @property
    def name(self):
        return self._name

    # Returns the data object serializable to JSON
    @property
    def data(self):
        result = dict()

        for k, v in self._parameters.items():
            result[k] = v.data

        return result

    # Adds the named parameter to module
    def add_parameter(self, name, parameter):
        self._parameters[name] = parameter

# Base class for emission zone types
class EmissionZone(ParticleSystemModule):
    # Constructs EmissionZone instance.
    def __init__(self):
        ParticleSystemModule.__init__(self, 'Zone')

# Box particle system emission zone
class BoxEmissionZone(EmissionZone):
    # Constructs the BoxEmissionZone instance.
    def __init__(self):
        EmissionZone.__init__(self)

        self._width = 0
        self._height = 0
        self._depth = 0

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return dict(type='box', width=self._width, height=self._height, depth=self._depth)

    # Parses the box zone from object
    def parse(self, object):
        self._width = object['boxX']
        self._height = object['boxY']
        self._depth = object['boxZ']

# Sphere particle system emission zone
class SphereEmissionZone(EmissionZone):
    # Constructs the BoxEmissionZone instance.
    def __init__(self, emitFromShell):
        EmissionZone.__init__(self)

        self._radius = 0
        self._emitFromShell = emitFromShell

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return dict(type='sphere', emitFromShell=self._emitFromShell, radius=self._radius)

    # Parses the box zone from object
    def parse(self, object):
        self._radius = object['radius']

# Cone particle system emission zone
class ConeEmissionZone(EmissionZone):
    # Constructs the BoxEmissionZone instance.
    def __init__(self):
        EmissionZone.__init__(self)

        self._radius = 0
        self._angle = 0

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return dict(type='cone', radius=self._radius, angle=self._angle)

    # Parses the box zone from object
    def parse(self, object):
        self._radius = object['radius']
        self._angle = object['angle']

# Hemisphere particle system emission zone
class HemiSphereEmissionZone(EmissionZone):
    # Constructs HemiSphereEmissionZone instance.
    def __init__(self):
        EmissionZone.__init__(self)

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return dict(type='hemisphere', radius=self._radius)

    # Parses the box zone from object
    def parse(self, object):
        self._radius = object['radius']

# Particle system asset
class ParticleSystem:
    # Constructs the ParticleSystem instance
    def __init__(self):
        self._modules = {}

    # Adds the module to particle system
    def add_module(self, name, module):
        self._modules[name] = module

    # Parses the parameter object
    def _parse_parameter(self, object, converter=None):
        type = object['minMaxState'] if isinstance(object, dict) else ParameterType.VALUE
        parameter = None

        if type == ParameterType.VALUE: parameter = Value()
        elif type == ParameterType.CONSTANT: parameter = ScalarParameter('constant')
        elif type == ParameterType.CURVE: parameter = ScalarParameter('curve')
        elif type == ParameterType.RANDOM_BETWEEN_CONSTANTS: parameter = ScalarParameter('randomBetweenConstants')
        elif type == ParameterType.RANDOM_BETWEEN_CURVES: parameter = ScalarParameter('randomBetweenCurves')

        parameter.parse(object, converter)

        return parameter

    # Parses the gradient object
    def _parse_gradient(self, object, converter=None):
        type = object['minMaxState']
        parameter = None

        if type == ParameterType.CURVE: parameter = GradientParameter('gradient')
        elif type == ParameterType.RANDOM_BETWEEN_CURVES: parameter = GradientParameter('randomBetweenGradients')

        parameter.parse(object, converter)

        return parameter

    # Performs particle system parsing from an object
    def parse(self, object):
        for k, v in object.items():
            if not isinstance(v, dict):
                continue

            if not 'enabled' in v.keys():
                continue

            if v['enabled'] == 0:
                continue

            particle_module = None

            def radiansToDegrees(value):
                return value * 180 / math.pi

            if k == 'ForceModule':
                particle_module = ParticleSystemModule('Acceleration')
                particle_module.add_parameter('x', self._parse_parameter(v['x']))
                particle_module.add_parameter('y', self._parse_parameter(v['y']))
                particle_module.add_parameter('z', self._parse_parameter(v['z']))
            elif k == 'InitialModule':
                particle_module = ParticleSystemModule('Emitter')
                particle_module.add_parameter('startLifetime', self._parse_parameter(v['startLifetime']))
                particle_module.add_parameter('startSpeed', self._parse_parameter(v['startSpeed']))
                particle_module.add_parameter('startSize', self._parse_parameter(v['startSize']))
                particle_module.add_parameter('startRotation', self._parse_parameter(v['startRotation'], radiansToDegrees))
                particle_module.add_parameter('maxNumParticles', self._parse_parameter(v['maxNumParticles']))
                particle_module.add_parameter('lengthInSec', self._parse_parameter(object['lengthInSec']))
                particle_module.add_parameter('speed', self._parse_parameter(object['speed']))
                particle_module.add_parameter('looping', self._parse_parameter(object['looping']))
                particle_module.add_parameter('moveWithTransform', self._parse_parameter(object['moveWithTransform']))
            elif k == 'SizeModule':
                particle_module = ParticleSystemModule('SizeOverLife')
                particle_module.add_parameter('curve', self._parse_parameter(v['curve']))
            elif k == 'ShapeModule':
                type = v['type'];

                if type == ShapeType.BOX: particle_module = BoxEmissionZone()
                elif type == ShapeType.SPHERE: particle_module = SphereEmissionZone(False)
                elif type == ShapeType.SPHERE_SHELL: particle_module = SphereEmissionZone(True)
                elif type == ShapeType.CONE: particle_module = ConeEmissionZone()
                elif type == ShapeType.HEMISPHERE: particle_module = HemiSphereEmissionZone()

                if particle_module is None:
                    raise Exception('Unhandled shape type {0}'.format(type))

                particle_module.parse(v)
            elif k == 'ColorModule':
                particle_module = ParticleSystemModule('ColorOverLife')
                particle_module.add_parameter('gradient', self._parse_gradient(v['gradient']))
            elif k == 'EmissionModule':
                particle_module = ParticleSystemModule('Emission')
                particle_module.add_parameter('rate', self._parse_parameter(v['rate']))

                bursts = []
                for i in range(0, v['m_BurstCount']):
                    bursts.append(v['time' + str(i)])
                    bursts.append(v['cnt'  + str(i)])

                particle_module.add_parameter('bursts', Value(bursts))
            elif k == 'VelocityModule':
                particle_module = ParticleSystemModule('VelocityOverLife')
                particle_module.add_parameter('x', self._parse_parameter(v['x']))
                particle_module.add_parameter('y', self._parse_parameter(v['y']))
                particle_module.add_parameter('z', self._parse_parameter(v['z']))

            if particle_module is not None:
                self._modules[particle_module.name] = particle_module

    # Returns the data object serializable to JSON
    @property
    def data(self):
        result = dict()

        for k, v in self._modules.items():
            result[k] = v.data

        return result

# Performs the particle system parsing
class ParticleSystemParser:
    # Converts particles to JSON
    @staticmethod
    def convert(assets, source, output):
        objects = yaml.objects_from_file(source)
        result  = None

        for k, v in objects.items():
            if not 'ParticleSystem' in v.keys():
                continue

            result = ParticleSystem()
            result.parse(v['ParticleSystem'])
            break

        # Save parsed scene to a JSON file
        if result is not None:
            yaml.save_to_json(output, result.data)

# Parses assets from a path
def parse_assets(args):
    result = Assets(args.source, args.strip_unused, args.use_uuids)
    result.parse()
    return result

# Imports all scenes
def import_scenes(assets, source, output):
    for item in assets.filter_by_type(AssetType.SCENE):
        paths = item.format_paths(source, output)
        dest = os.path.join(output, assets.asset_identifier(item))

    #    if item.file_name.find('Debug') == -1:
    #        continue

        print('Importing scene {0}'.format(item.full_path))
        Scene.convert(assets, paths.source, dest)

# Import used materials
def import_materials(assets, source, output):
    for item in assets.filter_by_type(AssetType.MATERIAL):
        paths = item.format_paths(source, output)
        dest = os.path.join(output, assets.asset_identifier(item))

        if assets.should_strip(item):
            continue

        print('Importing material {0}'.format(item.full_path))
        material.convert(assets, paths.source, dest)

# Imports all used assets
def import_assets(assets, source, output):
    for uuid, item in assets.used_assets.items():
        paths = item.format_paths(source, output)
        dest = os.path.join(output, assets.asset_identifier(item))

        if item.type == AssetType.MESH:
            actions.convert_fbx({}, paths.source, dest)()
        elif item.type == AssetType.TEXTURE:
            actions.convert_to_raw({}, paths.source, dest)()

# Imports all prefabs
def import_prefabs(assets, source, output):
    for item in assets.filter_by_type(AssetType.PREFAB):
        paths = item.format_paths(source, output)
        dest = os.path.join(output, assets.asset_identifier(item))

        if assets.should_strip(item):
            continue

    #    if item.identifier != 'engine_fire':
    #        continue

        print('Importing prefab {0}'.format(item.full_path))
        assets.use(item.uuid)
        Scene.convert(assets, paths.source, dest)