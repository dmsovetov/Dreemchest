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


class Rgba:
    """Rgba color value"""

    # Constructs Rgba color instance.
    def __init__(self, value=0):
        self._value = value

    # Returns the red color component
    @property
    def red(self):
        return float(self._value >> 0 & 255) / 255

    # Returns the green color component
    @property
    def green(self):
        return float(self._value >> 8 & 255) / 255

    # Returns the blue color component
    @property
    def blue(self):
        return float(self._value >> 16 & 255) / 255

    # Returns the alpha color component
    @property
    def alpha(self):
        return float(self._value >> 24 & 255) / 255

    @classmethod
    def from_object(cls, color):
        r = int(color['r'] * 255)
        g = int(color['g'] * 255)
        b = int(color['b'] * 255)
        a = int(color['a'] * 255)

        return Rgba(r << 24 | g << 16 | b << 8 | a)
