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

import os
import fnmatch
import json
import yaml
import random
from file import File
from .. import serializer


class Assets(object):
    """Loaded assets"""

    def __init__(self, input_path, output_path, cache, build_rules):
        """Constructs assets instance that are loaded from a specified path"""
        self._input_path = input_path
        self._output_path = output_path
        self._files = {}
        self._outdated = []
        self._build_rules = build_rules
        self._cache = cache

    @property
    def outdated(self):
        """Returns a list of outdated assets"""
        return self._outdated

    @property
    def files(self):
        """Returns all known asset files"""
        return self._files

    @property
    def input_path(self):
        """Returns an input assets path"""
        return self._input_path

    @property
    def output_path(self):
        """Returns an output assets path"""
        return self._output_path

    @property
    def cache(self):
        """Returns a cache instance"""
        return self._cache

    @property
    def build_rules(self):
        """Returns build rules"""
        return self._build_rules

    @classmethod
    def generate_uuid(cls):
        return '%032x' % random.getrandbits(128)

    def importer_by_file_name(self, file_name):
        """Returns an asset importer"""

        for k, importer in self.build_rules:
            if fnmatch.fnmatch(file_name, k):
                return importer()

        return None

    def _add_outdated_asset(self, asset_file):
        """Puts an asset file to an outdated list"""
        self._outdated.append(asset_file)

    def _add_asset(self, asset_file):
        """Puts an asset file to a registry"""

        if asset_file.uuid in self.files.keys():
            raise ValueError("duplicate asset id '%s'" % asset_file.uuid)

    def _load_asset(self, local_path):
        """Loads an asset by its file name"""

        absolute_input_path = os.path.join(self.input_path, local_path)

        if not os.path.exists(absolute_input_path + File.META_EXT):
            return None

        with open(os.path.join(self.input_path, local_path) + File.META_EXT) as fh:
            document = yaml.load(fh.read())
            asset = serializer.read(document, globals())
            asset.set_absolute_input_path(absolute_input_path)
            return asset

    def _create_asset(self, local_path):
        """Creates an asset from file"""

        absolute_input_path = os.path.join(self.input_path, local_path)

        importer = self.importer_by_file_name(local_path)

        if not importer:
            return None

        asset = File(absolute_input_path, uuid=self.generate_uuid(), importer=importer)
        asset.update_hash()
        asset.save()

        return asset

    def scan(self):
        """Scans an input directory for asset changes"""

        print 'Scanning %s...' % self.input_path

        for root, dirs, files in os.walk(self.input_path):
            for file_name in files:
                name = os.path.splitext(file_name)[0].lower()
                input_path = os.path.join(root, file_name)
                relative_path = os.path.relpath(input_path, self.input_path)

                if name.startswith('.'):
                    continue

                if file_name.endswith(File.META_EXT):
                    continue

                asset = self._load_asset(relative_path)

                if asset is None:
                    asset = self._create_asset(relative_path)

                if not asset.importer:
                    continue

                meta_hash = asset.meta_hash

                if self.cache.update_meta_hash(asset.uuid, meta_hash):
                    self.update_asset(asset, meta_hash)
                elif asset.update_hash():
                    self.update_asset(asset, meta_hash)
                elif not os.path.exists(os.path.join(self.output_path, asset.local_output_path)):
                    self.outdated.append(asset)

    def update_asset(self, asset, meta_hash):
        """Puts an asset to an outdated list and updates hash values"""

        self.outdated.append(asset)
        asset.save()
        self.cache.update_meta_hash(asset.uuid, meta_hash)

    def build(self, task_manager):
        """Builds each outdated file to an output folder"""

        def make_importer(asset_path, asset):
            return lambda: asset.importer.process(asset_path, asset)

        for item in self.outdated:
            task_manager.push(make_importer(self.output_path, item))

