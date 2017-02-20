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
import file_format
import patcher
from assets import Assets
from asset_type import AssetType
from scene import Scene


def convert_material(assets, source, output):
    """Converts a material asset file"""

    objects = file_format.objects_from_file(source)
    result = None

    for k, v in objects.items():
        result = v['Material']
        properties = patcher.patch_instance(assets, result['m_SavedProperties'], patcher.material.properties)
        result = patcher.patch_instance(assets, result, patcher.material.material)

        for key, value in properties.items():
            result[key] = value

    # Save parsed scene to a JSON file
    file_format.save_to_json(output, result)

# Parses assets from a path
def parse_assets(source, queue):
    result = Assets(source, False, True)
    result.parse(queue)
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
        convert_material(assets, paths.source, dest)

# Imports all prefabs
def import_prefabs(assets, source, output):
    for item in assets.filter_by_type(AssetType.PREFAB):
        paths = item.format_paths(source, output)
        dest = os.path.join(output, assets.asset_identifier(item))

        if assets.should_strip(item):
            continue

        print('Importing prefab {0}'.format(item.full_path))
        assets.use(item.uuid)
        Scene.convert(assets, paths.source, dest)