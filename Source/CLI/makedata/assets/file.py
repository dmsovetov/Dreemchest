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
import signature
import yaml
from .. import serializer


class File:
    """Input asset file"""

    META_EXT = ".meta"

    def __init__(self, input_path, uuid=None, importer=None):
        """Constructs an input asset file"""

        self._input_path = input_path
        self._uuid = uuid
        self._importers = dict()

        if importer:
            self._importers[importer.name] = importer

    @property
    def absolute_input_path(self):
        """Returns absolute asset source path"""
        return self._input_path

    @property
    def local_output_path(self):
        """Returns an asset output path"""
        return os.path.join(self.uuid[0] + self.uuid[1], self.uuid)

    @property
    def meta_file(self):
        """Returns an absolute meta file path"""
        return os.path.join(self.absolute_input_path + File.META_EXT)

    @property
    def meta_hash(self):
        """Returns a meta file hash"""
        return signature.file_hash(self.meta_file)

    @property
    def file_hash(self):
        """Returns actual input file hash"""
        return signature.file_hash(self.absolute_input_path)

    @property
    def importers(self):
        """Returns an attached asset importers"""
        return self._importers

    @property
    def uuid(self):
        """Returns an asset UUID"""
        return self._uuid

    @uuid.setter
    def uuid(self, value):
        self._uuid = value

    def save(self):
        """Saves asset meta file"""

        # Write asset properties
        document = serializer.write(self, save_type=False)
        document['importers'] = self.save_importers()

        with open(self.meta_file, 'wt') as fh:
            fh.write(yaml.dump(document, default_flow_style=False))
            fh.close()

    def save_importers(self):
        """Saves attached asset importers to dictionary"""
        return {name: serializer.write(importer, save_type=False) for name, importer in self.importers.items()}

    def load_importers(self, importers, package):
        """Loads importers from a dictionary"""
        self._importers = {k: serializer.read_instance(getattr(package, k)(), v) for k, v in importers.items()}

    @classmethod
    def load(cls, file_name, importers):
        meta_file_name = file_name + File.META_EXT

        if not os.path.exists(meta_file_name):
            return None

        with open(meta_file_name) as fh:
            document = yaml.load(fh.read())
            asset = serializer.read_instance(File(input_path=file_name), document, importers)
            asset.load_importers(document['importers'], importers)
            return asset
