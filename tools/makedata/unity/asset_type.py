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

# Asset type 
class AssetType:
    UNKNOWN = 'unknown'
    MESH = 'mesh'
    SCENE = 'scene'
    TEXTURE = 'image'
    PREFAB = 'prefab'
    LIGHTMAP = 'lightmap'
    MATERIAL = 'material'

    # Maps file extensions to asset types
    Extensions = dict(
          tga=TEXTURE
        , jpg=TEXTURE
        , tif=TEXTURE
        , png=TEXTURE
        , exr=LIGHTMAP
        , fbx=MESH
        , prefab=PREFAB
        , unity=SCENE
        , mat=MATERIAL
        )

    # Retuns an asset type by file name
    @staticmethod
    def map_from_file_name(file_name):
        ext = os.path.splitext(file_name)[1][1:].lower()

        if ext in AssetType.Extensions.keys():
            return AssetType.Extensions[ext]

        return AssetType.UNKNOWN