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
import hashlib
import base64


def update_hash(hash_value, input_file):
    while True:
        data = input_file.read(8192)

        if not data:
            break

        hash_value.update(data)


def file_hash(file_path):
    """Computes file hash"""

    hash_value = hashlib.md5()

    with open(file_path) as fh:
        update_hash(hash_value, fh)

    return base64.b64encode(hash_value.digest())


def folder_hash(path):
    """Computes folder hash"""

    hash_value = hashlib.md5()

    for root, dir_names, files in os.walk(path):
        for file_name in files:
            update_hash(hash_value, os.path.join(root, file_name))

    return base64.b64encode(hash_value.digest())
