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

import json
from .. import module

# Import the YAML parsing module
yaml = module.require('yaml', True)

# Saves object to a JSON file
def save_to_json(file_name, data):
    with open(file_name, 'wt') as fh:
        fh.write(json.dumps(data, indent=4))
        fh.close()

# Loads YAML from a string.
def from_string(str):
    return yaml.safe_load( str )

# Loads YAML from a file.
def from_file(file_name):
    with open(file_name) as fh:
        result = from_string(fh.read())
        fh.close()

    return result

# Parses an array of objects from YAML string
def objects_from_string(str):
    result = {}

    for stream in str.split('---'):
        if stream.find('%YAML') != -1 or stream.find('%TAG') != -1:
            continue

        nl   = stream.find( '\n' )
        tag  = stream[:nl]
        id   = tag.split(' ')[2][1:]
        str  = stream[nl:]

        str = str.replace('data:', '- data:') # Fixes the material properties

        data = from_string(str)

        data['id'] = int(id)
        result[id] = data

    return result

# Parses an array of objects from YAML file
def objects_from_file(file_name):
    with open(file_name) as fh:
        result = objects_from_string(fh.read())
        fh.close()
        return result