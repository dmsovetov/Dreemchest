import dreemchest

input = 'some_input.file'
output = 'any_output'
format = 'pvr'
files = [input, input, input]
generator = 'gles2'

dreemchest.export_mesh(input, output)

dreemchest.export_texture(input, output, 2.4)
dreemchest.compress_texture(input, output, format)
dreemchest.export_atlas(files, output)
dreemchest.cubemap_from_cross(input, output)
dreemchest.cubemap_from_panorama(input, output)
dreemchest.cubemap_convolve(input, output, 222, 2.2)
dreemchest.shader_convert(input, output, generator)