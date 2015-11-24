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

import os, json, collections, shutil, actions, module

# Import the YAML parsing module
yaml = module.require('yaml', True)

# Saves object to a JSON file
def save_to_json(file_name, data):
    with open(file_name, 'wt') as fh:
        fh.write(json.dumps(data, indent=4))
        fh.close()

# yaml_from_string
def yaml_from_string(str):
    return yaml.safe_load( str )

# yaml_from_file
def yaml_from_file(file_name):
    with open(file_name) as fh:
        result = yaml_from_string(fh.read())
        fh.close()

    return result

# Parses an array of objects from YAML string
def yaml_objects_from_string(str):
    result = {}

    for stream in str.split('---'):
        if stream.find('%YAML') != -1 or stream.find('%TAG') != -1:
            continue

        nl   = stream.find( '\n' )
        tag  = stream[:nl]
        id   = tag.split(' ')[2][1:]
        str  = stream[nl:]

        str = str.replace('data:', '- data:') # Fixes the material properties

        data = yaml_from_string(str)

        data['id'] = int(id)
        result[id] = data

    return result

# Parses an array of objects from YAML file
def yaml_objects(file_name):
    with open(file_name) as fh:
        result = yaml_objects_from_string(fh.read())
        fh.close()
        return result

# The single project asset
class asset:
    paths = collections.namedtuple('paths', ['source', 'output', 'folder'])

    # ctor
    def __init__(self, file_name, full_path, meta):
        self._file_name     = file_name
        self._full_path     = full_path
        self._meta          = meta
        self._identifier    = os.path.splitext(full_path.replace('\\', '/').replace('Assets/', ''))[0].lower()
        self._type          = assets.asset_type(file_name)

    # format_paths
    def format_paths(self, source, output):
        source    = os.path.join(source, self.full_path)
        output    = os.path.join(output, self.full_path)
        folder    = os.path.dirname(output)

        return asset.paths(source=source, output=output, folder=folder)

    # uuid
    @property
    def uuid(self):
        return self._meta['guid']

    # identifier
    @property
    def identifier(self):
        return self._identifier

    # file_name
    @property
    def file_name(self):
        return self._file_name

    # full_path
    @property
    def full_path(self):
        return self._full_path

    # type
    @property
    def type(self):
        return self._type

# Stores all parsed project assets.
class assets:
    Unknown     = 'unknown'
    Mesh        = 'mesh'
    Scene       = 'scene'
    Texture     = 'image'
    Prefab      = 'prefab'
    Lightmap    = 'lightmap'
    Material    = 'material'

    # Maps file extensions to asset types
    extensions = dict( tga = Texture, tif = Texture, png = Texture, exr = Lightmap, fbx = Mesh, prefab = Prefab, unity = Scene, mat = Material )

    # ctor
    def __init__(self, path):
        self._path          = path
        self._files         = {}
        self._used_assets   = {}

    # Retuns an asset type by file name
    @staticmethod
    def asset_type(file_name):
        ext = os.path.splitext(file_name)[1][1:].lower()

        if ext in assets.extensions.keys():
            return assets.extensions[ext]

        return assets.Unknown

    # used_assets
    @property
    def used_assets(self):
        return self._used_assets

    # save
    def save(self, folder):
        print 'Saving assets..'

        file_name = os.path.join(folder, 'assets.json')

        # Append used assets
        result = []
        for guid, item in self.used_assets.items():
            result.append(dict(identifier=item.identifier, uuid=item.uuid, type=item.type))

        # Append scenes
        for item in self.filter_by_type(assets.Scene):
            result.append(dict(identifier=item.identifier, uuid=item.uuid, type=item.type))

        with open(file_name, 'wt') as fh:
            fh.write(json.dumps(result))
            fh.close()

    # parse
    def parse(self):
        print 'Parsing assets from', self._path

        for folder, dirs, files in os.walk(os.path.join(self._path, 'Assets')):
            for file in files:
                full_path = os.path.join(folder, file)
                name, ext = os.path.splitext(file)

                if ext == '.meta':
                    meta = yaml_from_file(full_path)
                    self._files[meta['guid']] = asset(name, os.path.relpath(os.path.join(folder, name), self._path), meta)

        print len(self._files), 'assets parsed'

    # use
    def use(self, uuid):
        if not uuid in self._used_assets.keys():
        #    print 'Using asset', self._files[uuid].full_path
            self._used_assets[uuid] = self._files[uuid]

        return uuid

    # resolve
    def resolve(self, guid):
        if guid in self._files.keys():
            file = self._files[guid]

            if not guid in self._usedAssets.keys():
                print file.fullPath, 'used'
                self._usedAssets[guid] = file

            return file

        return None

    # filter_by_type
    def filter_by_type(self, type):
        return [v for k, v in self._files.items() if v.type == type]

    # used_by_type
    def used_by_type(self, *types):
        result = []
        for k, v in self._usedAssets.items():
            if v.type in types:
                result.append( v )
        return result

# Patches the object properties
class patcher:
    # Renames the property
    class rename:
        # __init__
        def __init__(self, name):
            self.name = name

        # __call__
        def __call__(self, assets, object, property):
            object[self.name] = object[property]
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

            object[self.name] = patcher.patch(assets, value, self.patchers)
            del object[property]

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
                    value = patcher.patch(assets, value, self.patchers)
                    result[value.keys()[0]] = value[value.keys()[0]]

            object[self.name] = result
            del object[property]

    @staticmethod
    def patch(assets, object, patchers):
        for name, value in object.items():
            if name in patchers.keys():
                patchers[name](assets, object, name)
            else:
                del object[name]

        return object

# Holds the info about a single scene
class scene:
    # ctor
    def __init__(self, assets):
        self._assets = assets

    # convert
    @staticmethod
    def convert(assets, source, output):
        # Parse scene
        instance = scene(assets)
        objects  = instance.parse(source)

        # Save parsed scene to a JSON file
        save_to_json(output, objects)

    # parse
    def parse(self, file_name):
        objects = yaml_objects(file_name)

        # Only this object types will be exported
        types = {
	        'GameObject', 'Transform', 'Renderer', 'Camera', 'Light'
        }

        # Resulting object
        result = {}
        meshes = {}

        # Collect all mesh filters
        for id, object in objects.items():
            # Get the object type
            type = [key for key in object.keys() if key != 'id'][0]
            
            if type != 'MeshFilter':
                continue

            # Get the parent scene object
            sceneObject = object['MeshFilter']['m_GameObject']['fileID']

            # Save the mesh filter asset
            meshes[sceneObject] = object['MeshFilter']['m_Mesh']['guid']

        # Patch objects
        for id, object in objects.items():
            # Get the object type
            type = [key for key in object.keys() if key != 'id'][0]

            # Should we skip this type
            if not type in types:
                continue

            # Get the scene object id
            sceneObject = object[type]['m_GameObject']['fileID'] if type != 'GameObject' else id

            # Get the actual properties
            data = patcher.patch(self._assets, object[type], ScenePatchers[type])

            # Link the mesh asset with renderer
            if type == 'Renderer':
                data['asset'] = self._assets.use(meshes[sceneObject])

            # Remove properties with None values
            for k, v in data.items():
               if v is None:
                  del data[k]

            # Save parsed object
            data['id']    = str(object['id'])
            data['class'] = type if type != 'GameObject' else 'SceneObject'
            result[id]    = data

        return result

# Holds the material info
class material:
    # Converts material to JSON
    @staticmethod
    def convert(assets, source, output):
        objects = yaml_objects(source)
        assert len(objects) == 1
        result = None

        for k, v in objects.items():
            result   = v['Material']
            properties = patcher.patch(assets, result['m_SavedProperties'], MaterialPropertiesPatcher)
            result = patcher.patch(assets, result, MaterialPatcher)

            for k, v in properties.items():
                result[k] = v

        # Save parsed scene to a JSON file
        save_to_json(output, result)

# The curve class
class Curve:
    # Constructs the curve instance
    def __init__(self):
        self._keyframes = []

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return self._keyframes

    # Parses the curve from object
    def parse(self, object):
        for v in object['m_Curve']:
            self._keyframes.append(v['time'])
            self._keyframes.append(v['value'])

# Parameter type enumeration
class ParameterType:
    VALUE = -1
    CONSTANT = 0
    CURVE = 1
    RANDOM_BETWEEN_CURVES = 2
    RANDOM_BETWEEN_CONSTANTS = 3

# The particle system property value
class Value:
    # Constructs the ConstantParameter instance
    def __init__(self):
        self._value = None

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return self._value

    # Parses the parameter from an object
    def parse(self, object):
        self._value = object

# The particle system parameter class
class ConstantParameter:
    # Constructs the ConstantParameter instance
    def __init__(self):
        self._value = 0

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return dict(type='constant', value=self._value)

    # Parses the parameter from an object
    def parse(self, object):
        assert object['minMaxState'] == ParameterType.CONSTANT
        self._value = object['scalar']

# The particle system curve parameter
class CurveParameter:
    # Constructs the ConstantParameter instance
    def __init__(self):
        self._value = Curve()

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return dict(type='curve', value=self._value.data)

    # Parses the parameter from an object
    def parse(self, object):
        assert object['minMaxState'] == ParameterType.CURVE
        self._value.parse(object['maxCurve'])

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

# Particle system asset
class ParticleSystem:
    # Constructs the ParticleSystem instance
    def __init__(self):
        self._modules = {}

    # Adds the module to particle system
    def add_module(self, name, module):
        self._modules[name] = module

    # Parses the parameter object
    def _parse_parameter(self, object):
        type = object['minMaxState'] if isinstance(object, dict) else ParameterType.VALUE
        parameter = None

        if type == ParameterType.VALUE: parameter = Value()
        elif type == ParameterType.CONSTANT: parameter = ConstantParameter()
        elif type == ParameterType.CURVE: parameter = CurveParameter()

        parameter.parse(object)

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

            if k == 'ForceModule':
                particle_module = ParticleSystemModule('Force')
                particle_module.add_parameter('x', self._parse_parameter(v['x']))
                particle_module.add_parameter('y', self._parse_parameter(v['y']))
                particle_module.add_parameter('z', self._parse_parameter(v['z']))
            elif k == 'InitialModule':
                particle_module = ParticleSystemModule('Emitter')
                particle_module.add_parameter('startLifetime', self._parse_parameter(v['startLifetime']))
                particle_module.add_parameter('startSpeed', self._parse_parameter(v['startSpeed']))
                particle_module.add_parameter('startSize', self._parse_parameter(v['startSize']))
                particle_module.add_parameter('startRotation', self._parse_parameter(v['startRotation']))
                particle_module.add_parameter('maxNumParticles', self._parse_parameter(v['maxNumParticles']))
                particle_module.add_parameter('lengthInSec', self._parse_parameter(object['lengthInSec']))
                particle_module.add_parameter('speed', self._parse_parameter(object['speed']))
                particle_module.add_parameter('looping', self._parse_parameter(object['looping']))
                particle_module.add_parameter('moveWithTransform', self._parse_parameter(object['moveWithTransform']))
            elif k == 'EmissionModule':
                particle_module = ParticleSystemModule('Emission')
                particle_module.add_parameter('rate', self._parse_parameter(v['rate']))

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
        objects = yaml_objects(source)
        result  = None

        for k, v in objects.items():
            if not 'ParticleSystem' in v.keys():
                continue

            result = ParticleSystem()
            result.parse(v['ParticleSystem'])
            break

        # Save parsed scene to a JSON file
        if result is not None:
            save_to_json(output, result.data)

# Parses assets from a path
def parse_assets(path):
    result = assets(path)
    result.parse()
    return result

# Imports all scenes
def import_scenes(assets, source, output):
    for item in assets.filter_by_type(assets.Scene):
        paths = item.format_paths(source, output)
        dest = os.path.join(output, item.uuid)

    #    if item.file_name.find('Debug') == -1:
    #        continue

        print 'Importing scene', item.full_path
        scene.convert(assets, paths.source, dest)

# Import used materials
def import_materials(assets, source, output):
    for item in assets.filter_by_type(assets.Material):
        paths = item.format_paths(source, output)
        dest = os.path.join(output, item.uuid)

        if not item.uuid in assets.used_assets.keys():
            continue

        print 'Importing material', item.full_path
        material.convert(assets, paths.source, dest)

# Imports all used assets
def import_assets(assets, source, output):
    for uuid, item in assets.used_assets.items():
        paths = item.format_paths(source, output)
        dest = os.path.join(output, uuid)

        if item.type == assets.Mesh:
            actions.convert_fbx({}, paths.source, dest)()
        elif item.type == assets.Texture:
            actions.convert_to_raw({}, paths.source, dest)()

# Imports all particle systems
def import_particles(assets, source, output):
    for item in assets.filter_by_type(assets.Prefab):
        paths = item.format_paths(source, output)
        dest = os.path.join(output, item.uuid)

    #    if not item.uuid in assets.used_assets.keys():
    #        continue

        print 'Importing particles', item.full_path
        ParticleSystemParser.convert(assets, paths.source, dest)

# Renderer patcher
RendererPatcher = {
	  'm_CastShadows': patcher.rename('castShadows')
	, 'm_ReceiveShadows': patcher.rename('receiveShadows')
	, 'm_Materials': patcher.asset('materials')
	, 'm_Enabled': patcher.rename('enabled')
	, 'm_GameObject': patcher.reference('sceneObject')
}

# Transform
TransformPatcher = {
	  'm_LocalRotation': patcher.vector('rotation', ['x', 'y', 'z', 'w'])
	, 'm_LocalScale': patcher.vector('scale', ['x', 'y', 'z'])
	, 'm_LocalPosition': patcher.vector('position', ['x', 'y', 'z'])
	, 'm_Enabled': patcher.rename('enabled')
	, 'm_GameObject': patcher.reference('sceneObject')
    , 'm_Father': patcher.reference('parent')
}

# GameObject
GameObjectPatcher = {
	  'm_Name': patcher.rename('name')
	, 'm_IsActive': patcher.rename('active')
	, 'm_Layer': patcher.rename('layer')
	, 'm_StaticEditorFlags': patcher.rename('flags')
}

# Light
LightPatcher = {
	  'm_Intensity': patcher.rename('intensity')
	, 'm_SpotAngle': patcher.rename('spotAngle')
	, 'm_Type': patcher.rename('type')
	, 'm_Range': patcher.rename('range')
	, 'm_Color': patcher.vector('color', ['r', 'g', 'b', 'a'])
	, 'm_Lightmapping': patcher.rename('backed')
	, 'm_Enabled': patcher.rename('enabled')
	, 'm_GameObject': patcher.reference('sceneObject')
}

# Camera
CameraPatcher = {
	  'near clip plane': patcher.rename('near')
	, 'far clip plane': patcher.rename('far')
	, 'field of view': patcher.rename('fov')
	, 'm_OcclusionCulling': patcher.rename('occlusionCulling')
	, 'm_BackGroundColor': patcher.vector('backgroundColor', ['r', 'g', 'b', 'a'])
	, 'm_NormalizedViewPortRect': patcher.vector('ndc', ['x', 'y', 'width', 'height'])
	, 'm_Enabled': patcher.rename('enabled')
	, 'm_GameObject': patcher.reference('sceneObject')
}

# Material
MaterialPatcher = {
	  'm_Name': patcher.rename('name')
    , 'm_Shader': patcher.shader('shader')
}

# Material texture patcher
MaterialTexturePatcher = {
      'm_Texture': patcher.asset('asset')
    , 'm_Scale': patcher.vector('scale', ['x', 'y'])
    , 'm_Offset': patcher.vector('offset', ['x', 'y'])
}

# Material textures
MaterialTexturesPatcher = {
    '_MainTex': patcher.object('diffuse', MaterialTexturePatcher)
}

# Material parameters
MaterialParametersPatcher = {
    '_Shininess': patcher.rename('shininess')
}

# Material colors
MaterialColorsPatcher = {
      '_Color': patcher.vector('diffuse', ['r', 'g', 'b', 'a'])
    , '_SpecColor': patcher.vector('specular', ['r', 'g', 'b', 'a'])
    , '_TintColor': patcher.vector('tint', ['r', 'g', 'b', 'a'])
}

# Material properties
MaterialPropertiesPatcher = {
	  'm_TexEnvs': patcher.keyValue('textures', MaterialTexturesPatcher)
    , 'm_Floats': patcher.keyValue('parameters', MaterialParametersPatcher)
    , 'm_Colors': patcher.keyValue('colors', MaterialColorsPatcher)
}

# Scene patchers
ScenePatchers = {
	  'Renderer': RendererPatcher
    , 'Transform': TransformPatcher
    , 'GameObject': GameObjectPatcher
    , 'Light': LightPatcher
    , 'Camera': CameraPatcher
}
