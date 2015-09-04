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

import os, json, yaml, collections, shutil

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
        data = yaml_from_string(stream[nl:])

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
        self._file_name = file_name
        self._full_path = full_path
        self._meta      = meta
        self._type      = assets.asset_type(file_name)

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
    Texture     = 'texture'
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
    def save(self, fileName):
        print 'Saving assets..'

        assets = []
        for guid, asset in self._usedAssets.items():
            asset._meta['fileName'] = asset.fullPath
            assets.append( Prettify.prettify( asset._meta, Prettify.Asset ) )

        with open(file_name, 'wt') as fh:
            fh.write(json.dumps(assets))
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
            print 'Using asset', self._files[uuid].full_path
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
def patch(assets, object):
    # Renames the property
    class rename:
        # __init__
        def __init__(self, name):
            self.name = name

        # __call__
        def __call__(self, object, property):
            object[self.name] = object[property]
            del object[property]

    # Formats an asset
    class asset:
        # __init__
        def __init__(self, name):
            self.name = name

        # __call__
        def __call__(self, object, property):
            value = object[property]

            if isinstance(value, dict):
                object[self.name] =  assets.use(value['guid']) if 'guid' in value.keys() else None
            else:
                object[self.name] = [assets.use(item['guid']) for item in value]

            del object[property]

    # Formats an array of object ids
    class objectIds:
        # __init__
        def __init__(self, name):
            self.name = name

        # __call__
        def __call__(self, object, property):
            ids = []

            for nested in object[property]:
                for k, v in nested.items():
                    ids.append(v['fileID'])

            object[self.name] = ids
            del object[property]

    # Formats the reference
    class reference:
        # __init__
        def __init__(self, name):
            self.name = name

        # __call__
        def __call__(self, object, property):
            object[self.name] = str(object[property]['fileID'])
            del object[property]

    # Formats the vector
    class vector:
        # __init__
        def __init__(self, name, coords):
            self.name = name
            self.coords = coords

        # __call__
        def __call__(self, object, property):
            value  = object[property]
            result = []

            for coord in self.coords:
                if coord in value.keys(): result.append(float(value[coord]))

            object[self.name] = result
            del object[property]

    # Property patchers
    patchers = {}

    # Renderer
    patchers['m_CastShadows'] = rename('castShadows')
    patchers['m_ReceiveShadows'] = rename('receiveShadows')
    patchers['m_Materials'] = asset('materials')
    patchers['m_Mesh'] = asset('mesh')
    patchers['m_PrefabParentObject'] = asset('asset')

    # Components
    patchers['m_Enabled'] = rename('enabled')
    patchers['m_GameObject'] = reference('sceneObject')

    # Transform
    patchers['m_LocalRotation'] = vector('rotation', ['x', 'y', 'z', 'w'])
    patchers['m_LocalScale'] = vector('scale', ['x', 'y', 'z'])
    patchers['m_LocalPosition'] = vector('position', ['x', 'y', 'z'])

    # GameObject
    patchers['m_Name'] = rename('name')
    patchers['m_IsActive'] = rename('active')
    patchers['m_Layer'] = rename('layer')
    patchers['m_StaticEditorFlags'] = rename('flags')
    #patchers['m_Component'] = objectIds('components')

    # Light
    patchers['m_Intensity'] = rename('intensity')
    patchers['m_SpotAngle'] = rename('spotAngle')
    patchers['m_Type'] = rename('type')
    patchers['m_Range'] = rename('range')
    patchers['m_Color'] = vector('color', ['r', 'g', 'b', 'a'])
    patchers['m_Lightmapping'] = rename('backed')

    # Camera
    patchers['near clip plane']             = rename('near')
    patchers['far clip plane']              = rename('far')
    patchers['field of view']               = rename('fov')
    patchers['m_OcclusionCulling']          = rename('occlusionCulling')
    patchers['m_BackGroundColor']           = vector('backgroundColor', ['r', 'g', 'b', 'a'])
    patchers['m_NormalizedViewPortRect']    = vector('ndc', ['x', 'y', 'width', 'height'])

    # Patch the properties
    for name, value in object.items():
        if name in patchers.keys():
            patchers[name](object, name)
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
        types = ['GameObject', 'Transform', 'Renderer', 'Camera', 'Light']

        # Resulting object
        result = {}

        for id, object in objects.items():
            # Get the object type
            type = [key for key in object.keys() if key != 'id'][0]

            # Should we skip this type
            if not type in types:
                continue

            # Get the actual properties
            data = patch(self._assets, object[type])

            # Remove properties with None values
            for k, v in data.items():
               if v is None:
                  del data[k]

            # Save parsed object
            data['id']    = str(object['id'])
            data['class'] = type
            result[id]    = data

        return result

# Parses assets from a path
def parse_assets(path):
    result = assets(path)
    result.parse()
    return result

# Imports all scenes
def import_scenes(assets, source, output):
    for item in assets.filter_by_type(assets.Scene):
        paths = item.format_paths(source, output)

        print 'Importing scene', item.full_path
        scene.convert(assets, paths.source, os.path.join(output, item.uuid))

# Imports all used assets
def import_assets(assets, source, output):
	for uuid, asset in assets.used_assets.items():
		paths = asset.format_paths(source, output)

		print 'Importing asset', asset.full_path
		shutil.copyfile(paths.source, os.path.join(output, uuid))