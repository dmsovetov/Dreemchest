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

from ..importer import Importer


class TextureFormat(object):
    """Available image formats"""

    RAW = 'raw'
    PNG = 'png'
    QUANTIZED_PNG = 'quantized_png'
    AVAILABLE = [RAW, PNG, QUANTIZED_PNG]
    
    
class TextureCompression(object):
    """Available image compression modes"""
    
    DISABLED = 'disabled'
    PVR = 'pvr'
    DXT = 'dxt'
    ETC = 'etc'
    AVAILABLE = [DISABLED, PVR, DXT, ETC]


class TextureFilter(object):
    """Available image filters"""

    NEAREST = 'nearest'
    LINEAR = 'linear'
    BILINEAR = 'bilinear'
    TRILINEAR = 'trilinear'
    AVAILABLE = [NEAREST, LINEAR, BILINEAR, TRILINEAR]


class ImageFormats(object):
    """Supported input image formats"""

    TGA = 'tga'
    PNG = 'png'
    HDR = 'hdr'
    EXR = 'exr'
    JPG = 'jpg'
    AVAILABLE = [TGA, PNG, HDR, EXR, JPG]


class TextureImporter(Importer):
    """Imports a image as 2D image"""

    def __init__(self, texture_format=TextureFormat.RAW):
        """Constructs a image importer instance"""
        self._format = texture_format
        self._mipmaps = False
        self._compressed = False
        self._filtering = TextureFilter.LINEAR

    @property
    def format(self):
        """Returns an output image format"""

        return self._format

    @format.setter
    def format(self, value):
        """Sets an output image format"""

        if value not in TextureFormat.AVAILABLE:
            raise ValueError("invalid output image format '%s'" % value)

        self._format = value

    @property
    def mipmaps(self):
        """Returns true if mipmap generation is enabled"""
        return self._mipmaps

    @mipmaps.setter
    def mipmaps(self, value):
        """Enables or disables mipmap generation"""
        self._mipmaps = value

    @property
    def compressed(self):
        """Returns true if a image should be compressed"""
        return self._compressed

    @compressed.setter
    def compressed(self, value):
        """Enables or disables hardware image compression"""
        self._compressed = value

    @property
    def filtering(self):
        """Returns image filtering"""
        return self._filtering

    @filtering.setter
    def filtering(self, value):
        """Sets image filtering value"""

        if value not in TextureFilter.AVAILABLE:
            raise ValueError("invalid image filtering mode '%s'" % value)

        self._filtering = value
