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

from color import Rgba

# Parameter type enumeration
class ParameterType:
    VALUE = -1
    CONSTANT = 0
    CURVE = 1
    RANDOM_BETWEEN_CURVES = 2
    RANDOM_BETWEEN_CONSTANTS = 3

    # Returns the string value of a parameter type
    @staticmethod
    def to_string(id):
        if id == ParameterType.VALUE: return 'value'
        elif id == ParameterType.CONSTANT: return 'constant'
        elif id == ParameterType.CURVE: return 'curve'
        elif id == ParameterType.RANDOM_BETWEEN_CURVES: return 'randomBetweenCurves'
        elif id == ParameterType.RANDOM_BETWEEN_CONSTANTS: return 'randomBetweenConstants'

        return None


# The curve class
class Curve:
    # Constructs the curve instance
    def __init__(self):
        self._keyframes = []

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return self._keyframes

    # Returns the curve constant value
    @property
    def constant(self):
        return self._keyframes[1]

    # Parses the curve from object
    def parse(self, object, scale_factor=1.0, converter=None):
        for v in object['m_Curve']:
            value = float(v['value']) * float(scale_factor)

            if converter is not None:
                value = converter(value)

            self._keyframes.append(v['time'])
            self._keyframes.append(value)


# The gradient class
class Gradient:
    # Constructs the curve instance
    def __init__(self):
        self._alphaKeyframes = []
        self._colorKeyframes = []

    # Returns the data object serializable to JSON
    @property
    def data(self):
        return dict(color=self._colorKeyframes, alpha=self._alphaKeyframes)

    # Returns the alpha gradient.
    @property
    def alpha(self):
        return self._alphaKeyframes

    # Returns the rgb gradient.
    @property
    def rgb(self):
        return self._colorKeyframes

    # Returns the constant rgb value.
    @property
    def constant_rgb(self):
        return self.rgb[1:4]

    # Returns the constant alpha value.
    @property
    def constant_alpha(self):
        return self.alpha[1]

    # Parses the curve from object
    def parse(self, object, converter=None):
        alphaKeysCount = object['m_NumAlphaKeys']
        colorKeysCount = object['m_NumColorKeys']

        keys = []

        for i in range(max(alphaKeysCount, colorKeysCount)):
            keys.append(Rgba(object['key' + str(i)]['rgba']))

        for i in range(0, alphaKeysCount):
            self._alphaKeyframes.append(float(object['atime' + str(i)]) / 65535)
            self._alphaKeyframes.append(keys[i].alpha)

        for i in range(0, colorKeysCount):
            self._colorKeyframes.append(float(object['ctime' + str(i)]) / 65535)
            self._colorKeyframes.append(keys[i].red)
            self._colorKeyframes.append(keys[i].green)
            self._colorKeyframes.append(keys[i].blue)