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

    def __init__(self, input_path=None, uuid=None, importer=None):
        """Constructs an input asset file"""

        self._input_path = input_path
        self._uuid = uuid
        self._hash = None
        self._importer = importer

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
    def hash(self):
        """Returns an asset input file hash"""
        return self._hash

    @property
    def file_hash(self):
        """Returns actual input file hash"""
        return signature.file_hash(self.absolute_input_path)

    @hash.setter
    def hash(self, value):
        """Sets an asset input file hash"""
        self._hash = value

    @property
    def importer(self):
        """Returns an attached asset importer"""
        return self._importer

    @importer.setter
    def importer(self, value):
        """Sets an asset importer"""
        self._importer = value

    @property
    def uuid(self):
        """Returns an asset UUID"""
        return self._uuid

    @uuid.setter
    def uuid(self, value):
        self._uuid = value

    def set_absolute_input_path(self, value):
        """Sets an asset input file path"""
        self._input_path = value

    def set_importer(self, value):
        """Sets an asset importer"""
        self._importer = value

    def update_hash(self):
        """Updates an input file hash, returns true if hash values were distinct"""

        if self.file_hash == self.hash:
            return False

        self.hash = self.file_hash
        return True

    def save(self):
        """Saves asset meta file"""

        assert self.importer is not None, 'asset %s has no importer' % self.absolute_input_path

        document = serializer.write(self)

        with open(self.meta_file, 'wt') as fh:
            fh.write(yaml.dump(document, default_flow_style=False))
            fh.close()
