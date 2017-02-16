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

import shutil, os, struct, collections, module

try:
    import fbx
except ImportError as e:
    print e

from PIL import Image, ImageOps


# action
class action:
    # ctor
    def __init__(self, item, source, dest):
        self.item, self.source, self.dest = item, source, dest

    # call
    def __call__(self, type):
        print('{0} <= {1}'.format(type, self.source))
        self.item['type'] = type


# copy
class copy(action):
    # ctor
    def __init__(self, item, source, dest):
        action.__init__(self, item, source, dest)

    # call
    def __call__(self):
        print('Copying {0} from {1}'.format(self.source, self.dest))
        shutil.copyfile(self.source, self.dest)


# image
class image(action):
    # ctor
    def __init__(self, item, source, dest):
        action.__init__(self, item, source, dest)

    # call
    def __call__(self):
        action.__call__(self, 'image')

        # Load image from file
        img = Image.open(self.source)

        # Update asset info
        self.item['width'] = img.size[0]
        self.item['height'] = img.size[1]

        return img


# compress
class compress(action):
    # ctor
    def __init__(self, item, source, dest):
        action.__init__(self, item, source, dest)

    # call
    def __call__(self):
        print('Compress {0}'.format(self.source))


# png_quant
class png_quant(action):
    # ctor
    def __init__(self, item, source, dest):
        action.__init__(self, item, source, dest)
        
        if not 'PNG_QUANT' in os.environ:
            raise Exception('Pngquant not found, make sure that PNG_QUANT enviroment variable is set')


# convert_to_raw
class convert_to_raw(image):
    # ctor
    def __init__(self, item, source, dest):
        image.__init__(self, item, source, dest)

    # call
    def __call__(self):
        img = image.__call__(self)

        if img is None:
            return

        width = img.size[0]
        height = img.size[1]
        channels = 3 if img.mode == 'RGB' else 4
        img = ImageOps.flip(img)

        self.item['format'] = 'raw'

        with open(self.dest, 'wb') as fh:
            pixels = img.tobytes()
            data = struct.pack('HHB%us' % len(pixels), width, height, channels, pixels)
            fh.write(data)
            fh.close()


# convert_fbx
class convert_fbx(action):
    chunk = collections.namedtuple('chunk', ['vertices', 'indices'])

    # ctor
    def __init__(self, item, source, dest):
        action.__init__(self, item, source, dest)

    # call
    def __call__(self):
        action.__call__(self, 'mesh')

        manager  = fbx.FbxManager.Create()
        importer = fbx.FbxImporter.Create(manager, 'Importer')
        status   = importer.Initialize(self.source)

        if status == False:
           print('ERROR: {0}'.format(self.source))
           return

        scene = fbx.FbxScene.Create(manager, 'Scene')
        importer.Import(scene)
        importer.Destroy()

        converter = fbx.FbxGeometryConverter(manager)
        converter.Triangulate(scene, True)
        converter.SplitMeshesPerMaterial(scene, True)

        # processEachMesh
        def processEachMesh(node, callback):
            # Check if node has a mesh and than extract it
            attribute = node.GetNodeAttribute()

            if attribute and attribute.ClassId == fbx.FbxMesh.ClassId:
                mesh        = node.GetMesh()
                geometry    = node.GetGeometry()

                for i in range(0, node.GetNodeAttributeCount()):
                    attribute = node.GetNodeAttributeByIndex(i)

                    if attribute.ClassId == fbx.FbxMesh.ClassId:
                        callback(attribute)

            # Process each node child
            for i in range(0, node.GetChildCount()):
                processEachMesh(node.GetChild(i), callback)

        # get_uv_set
        def get_uv_set(mesh):
            result = []

            for i in range(0, mesh.GetLayerCount()):
                result.append(mesh.GetLayer(i).GetUVSets()[0].GetName())

            return result

        # class Vertex
        class Vertex:
            def __init__(self, pos, normal, uv0, uv1):
                self.pos = pos
                self.normal = normal
                self.uv0 = uv0
                self.uv1 = uv1

        # load_mesh
        def load_mesh(mesh):
            controlPointsCount = mesh.GetControlPointsCount()
            controlPoints      = mesh.GetControlPoints()

            uv = get_uv_set(mesh)
            vertices = []
            indices = []

            for i in range(0, mesh.GetPolygonCount()):
                polygonSize = mesh.GetPolygonSize(i)
                assert polygonSize == 3

                for j in range(0, polygonSize):
                    controlPointIndex = mesh.GetPolygonVertex(i, j)
                    if controlPointIndex > controlPointsCount:
                        continue

                    uv0     = fbx.FbxVector2()
                    uv1     = fbx.FbxVector2()
                    normal  = fbx.FbxVector4()
                    pos     = controlPoints[controlPointIndex]

                    mesh.GetPolygonVertexNormal(i, j, normal)
                    mesh.GetPolygonVertexUV(i, j, uv[0], uv0)

                    if len(uv) > 1:
                        mesh.GetPolygonVertexUV(i, j, uv[1], uv1)

                    vertices.append(Vertex(pos, normal, uv0, uv1))
                    indices.append(len(vertices) - 1)

            load_mesh.chunks.append(convert_fbx.chunk(vertices=vertices, indices=indices))

        load_mesh.chunks   = []

        processEachMesh(scene.GetRootNode(), load_mesh)

        chunks = load_mesh.chunks

        with open(self.dest, 'wb') as fh:
            Header = struct.Struct('I')
            ChunkHeader = struct.Struct('I I')
            Vertex = struct.Struct('f f f f f f f f f f')
            Index  = struct.Struct('H')

            fh.write(Header.pack(len(chunks)))

            for i in range(0, len(chunks)):
                vertices = chunks[i].vertices
                indices = chunks[i].indices

                fh.write(ChunkHeader.pack(len(vertices), len(indices)))

                for v in vertices:
                    fh.write(Vertex.pack(v.pos[0], v.pos[1], v.pos[2], v.normal[0], v.normal[1], v.normal[2], v.uv0[0], v.uv0[1], v.uv1[0], v.uv1[1]))

                for i in indices:
                    fh.write(Index.pack(i))