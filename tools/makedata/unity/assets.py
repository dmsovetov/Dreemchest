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

import collections, os, yaml
from asset_type import AssetType

# The single project asset
class Asset:
    paths = collections.namedtuple('paths', ['source', 'output', 'folder'])

    # Constructs the Asset instance
    def __init__(self, file_name, full_path, meta):
        self._file_name     = file_name
        self._full_path     = full_path
        self._meta          = meta
        self._identifier    = os.path.splitext(full_path.replace('\\', '/').replace('Assets/', ''))[0].lower()
        self._type          = AssetType.map_from_file_name(file_name)

    # Formats the asset input/output paths
    def format_paths(self, source, output):
        source    = os.path.join(source, self.full_path)
        output    = os.path.join(output, self.full_path)
        folder    = os.path.dirname(output)

        return Asset.paths(source=source, output=output, folder=folder)

    # Returns an asset uuid
    @property
    def uuid(self):
        return self._meta['guid']

    # Returns an asset identifier
    @property
    def identifier(self):
        return self._identifier

    # Returns an asset base name
    @property
    def base_name(self):
        return os.path.splitext(os.path.basename(self.file_name))[0]

    # Returns an asset file name
    @property
    def file_name(self):
        return self._file_name

    # Returns an asset full file path
    @property
    def full_path(self):
        return self._full_path

    # Returns an asset type
    @property
    def type(self):
        return self._type

# Stores all parsed project assets.
class Assets:

    # Constructs Assets instance
    def __init__(self, path, strip_unused, use_uuids):
        self._path          = path
        self._strip_unused  = strip_unused
        self._use_uuids     = use_uuids
        self._files         = {}
        self._used_assets   = {}

    # Returns the registry of used assets
    @property
    def used_assets(self):
        return self._used_assets

    # Returns flag indicating that asset UUIDs should be used instead of file names.
    @property
    def use_uuids(self):
        return self._use_uuids

    # Saves the asset bundle JSON
    def save(self, folder):
        print('Saving assets..')

        file_name = os.path.join(folder, 'assets.json')

        # Append used assets
        result = []
        for guid, item in self.used_assets.items():
            result.append(dict(identifier=item.identifier, uuid=self.asset_identifier(item), type=item.type))

        # Append scenes
        for item in self.filter_by_type(AssetType.SCENE):
            result.append(dict(identifier=item.identifier, uuid=self.asset_identifier(item), type=item.type))

        yaml.save_to_json(file_name, result)

    # Returns an asset identifier
    def asset_identifier(self, asset):
        return asset.uuid if self.use_uuids else asset.file_name

    # Parses the project assets
    def parse(self):
        print('Parsing assets from {0}'.format(self._path))

        for folder, dirs, files in os.walk(os.path.join(self._path, 'Assets')):
            for file in files:
                full_path = os.path.join(folder, file)
                name, ext = os.path.splitext(file)

                if ext == '.meta':
                    meta = yaml.from_file(full_path)
                    self._files[meta['guid']] = Asset(name, os.path.relpath(os.path.join(folder, name), self._path), meta)

        print('{0} assets parsed'.format(len(self._files)))

    # Marks an asset as used
    def use(self, uuid):
        if not uuid in self._files:
            return None

        asset = self._files[uuid]

        if not uuid in self._used_assets.keys():
            self._used_assets[uuid] = asset

        return self.asset_identifier(asset)

    # Returns the asset by an UUID and marks it as used
    def resolve(self, guid):
        if guid in self._files.keys():
            file = self._files[guid]

            if not guid in self._usedAssets.keys():
                print(file.fullPath, 'used')
                self._usedAssets[guid] = file

            return file

        return None

    # Returns an array of assets filtered by type
    def filter_by_type(self, type):
        return [v for k, v in self._files.items() if v.type == type]

    # Returns an array of used assets filtered by type
    def used_by_type(self, *types):
        result = []
        for k, v in self._usedAssets.items():
            if v.type in types:
                result.append( v )
        return result

    # Returns true if an asset should be stripped.
    def should_strip(self, asset):
        if not self._strip_unused:
            return False

        return asset.uuid not in self.used_assets.keys()