#################################################################################
#
#   http://stackoverflow.com/a/29681646/5719384
#
#################################################################################

import sys
import os
from PIL import Image
from math import pi, atan2, hypot, floor
from numpy import clip


def pixel_to_point(i, j, face, edge):
    """
    Returns x,y,z coordinates from out image pixels coordinates

    :param i: pixel coord x
    :param j: pixel coord y
    :param face: a face number
    :param edge: edge length
    :return: a world space point
    """
    a = 2.0 * float(i) / edge
    b = 2.0 * float(j) / edge
    if face == 0:  # back
        (x, y, z) = (-1.0, 1.0-a, 3.0 - b)
    elif face == 1:  # left
        (x, y, z) = (a-3.0, -1.0, 3.0 - b)
    elif face == 2:  # front
        (x, y, z) = (1.0, a - 5.0, 3.0 - b)
    elif face == 3:  # right
        (x, y, z) = (7.0-a, 1.0, 3.0 - b)
    elif face == 4:  # top
        (x, y, z) = (b-1.0, a -5.0, 1.0)
    elif face == 5:  # bottom
        (x, y, z) = (5.0-b, a-5.0, -1.0)
    return x, y, z


def panorama_to_cross(src, output):
    """Converts from equirectangular panorama to a cube map cross"""

    in_size = src.size
    out_size = output.size
    in_pix = src.load()
    out_pix = output.load()
    edge = in_size[0] / 4   # the length of each edge in pixels

    for i in xrange(out_size[0]):
        face = int(i/edge)  # 0 - back, 1 - left 2 - front, 3 - right
        print '\r\tprocessing %d/%d' % (i, out_size[0]),
        sys.stdout.flush()

        if face == 2:
            rng = xrange(0, edge * 3)
        else:
            rng = xrange(edge, edge * 2)

        for j in rng:
            if j < edge:
                face2 = 4 # top
            elif j >= 2 * edge:
                face2 = 5 # bottom
            else:
                face2 = face

            (x, y, z) = pixel_to_point(i, j, face2, edge)
            theta = atan2(y, x)  # range -pi to pi
            r = hypot(x, y)
            phi = atan2(z, r)  # range -pi/2 to pi/2

            # source img coords
            uf = (2.0 * edge * (theta + pi) / pi)
            vf = (2.0 * edge * (pi/2 - phi) / pi)

            # Use bilinear interpolation between the four surrounding pixels
            ui = floor(uf)  # coord of pixel to bottom left
            vi = floor(vf)
            u2 = ui+1       # coords of pixel to top right
            v2 = vi+1
            mu = uf-ui      # fraction of way across pixel
            nu = vf-vi

            # Pixel values of four corners
            A = in_pix[ui % in_size[0], clip(vi, 0, in_size[1] - 1)]
            B = in_pix[u2 % in_size[0], clip(vi, 0, in_size[1] - 1)]
            C = in_pix[ui % in_size[0], clip(v2, 0, in_size[1] - 1)]
            D = in_pix[u2 % in_size[0], clip(v2, 0, in_size[1] - 1)]

            # interpolate
            (r, g, b) = (
                A[0]*(1-mu)*(1-nu) + B[0]*(mu)*(1-nu) + C[0]*(1-mu)*nu+D[0]*mu*nu,
                A[1]*(1-mu)*(1-nu) + B[1]*(mu)*(1-nu) + C[1]*(1-mu)*nu+D[1]*mu*nu,
                A[2]*(1-mu)*(1-nu) + B[2]*(mu)*(1-nu) + C[2]*(1-mu)*nu+D[2]*mu*nu
            )

            out_pix[i, j] = (int(round(r)), int(round(g)), int(round(b)))


def cross_to_faces(src):
    """Converts from a cross cube map image to a set of cube map faces"""

    in_size = src.size
    edge_length = in_size[0] / 4

    regions = [(0, 1), (1, 1), (2, 1), (3, 1), (2, 0), (2, 2)]
    faces = []

    for region in regions:
        x = region[0] * edge_length
        y = region[1] * edge_length
        coordinates = (x, y, x + edge_length, y + edge_length)
        faces.append(src.crop(coordinates))

    return faces

files = [
    'Factory_Catwalk', 'MonValley_DirtRoad', 'Sierra_Madre_B', 'Siggraph2007_UpperFloor', 'Tropical_Beach', 'Ueno_Shrine', 'Walk_Of_Fame'
]

for file in files:
    path = os.path.join('/Users/dmitry.sovetov/Downloads', file)
    file_name = os.path.join(path, file + '_8k.jpg')

    img_in = Image.open(file_name)
    out_size = 512
    ratio = float(out_size * 4) / img_in.size[0]
    img_in = img_in.resize((int(img_in.size[0]*ratio), int(img_in.size[1]*ratio)))
    in_size = img_in.size
    img_out = Image.new("RGB", (in_size[0], in_size[0]*3/4), "black")
    panorama_to_cross(img_in, img_out)

    file_names = ['left', 'front', 'right', 'back', 'top', 'bottom']

    index = 0
    for face in cross_to_faces(img_out):
        face.save(os.path.join(path, "%s.tga") % file_names[index])
        index += 1