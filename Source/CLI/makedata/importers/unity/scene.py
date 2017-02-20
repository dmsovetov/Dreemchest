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

import file_format
import patcher

# Holds the info about a single scene
class Scene:
    # Constructs the Scene instance.
    def __init__(self, assets):
        self._assets = assets

    # Converts the scene
    @staticmethod
    def convert(assets, source, output):
        # Parse scene
        instance = Scene(assets)
        objects  = instance.parse(source)

        # Save parsed scene to a JSON file
        file_format.save_to_json(output, objects)

    # Parses the scene from YAML file
    def parse(self, file_name):
        objects = file_format.objects_from_file(file_name)

        # Only this object types will be exported
        types = {
	        'GameObject', 'Transform', 'Renderer', 'Camera', 'Light', 'ParticleSystem'
        }

        # Resulting object
        result = {}
        meshes = {}
        particles = {}

        # Collect all mesh filters & particle renderers
        for id, object in objects.items():
            # Get the object type
            type = [key for key in object.keys() if key != 'id'][0]

            # Skip objects with no m_GameObject reference
            if 'm_GameObject' not in object[type]:
                continue

            # Get the parent scene object
            sceneObject = object[type]['m_GameObject']['fileID']

            # Save the mesh filter asset
            if type == 'MeshFilter':
                meshes[sceneObject] = object[type]['m_Mesh']['guid']
            elif type == 'ParticleSystemRenderer':
                particles[sceneObject] = object[type]['m_Materials'][0]['guid']

        # Patch objects
        for id, object in objects.items():
            # Get the object type
            type = [key for key in object.keys() if key != 'id'][0]

            # Should we skip this type
            if not type in types:
                continue

            # Get the scene object id
            sceneObject = object[type]['m_GameObject']['fileID'] if type != 'GameObject' else id

            # Get the actual properties
            data = patcher.patch_instance(self._assets, object[type], patcher.scene.scene[type])

            # Link the mesh asset with renderer
            if type == 'Renderer':
                data['asset'] = self._assets.use(meshes[sceneObject])
            elif type == 'ParticleSystem':
                data['material'] = self._assets.use(particles[sceneObject])

            # Remove properties with None values
            for k, v in data.items():
               if v is None:
                  del data[k]

            # Save parsed object
            data['id']    = str(object['id'])

            if type == 'GameObject': data['class'] = 'SceneObject'
            elif type == 'ParticleSystem': data['class'] = 'Particles'
            elif type == 'ParticleSystemRenderer': data['class'] = 'ParticleRenderer'
            else: data['class'] = type
            result[id]    = data

        return result