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

import math
from ..parameter import ParameterType, Curve, Gradient
from ..color import Rgba


def field(name, converter=None):
    """Renames property and optionally converts its value"""

    def invoke(assets, target, key):
        if name == key:
            target[key] = converter(target[key]) if converter else target[key]
            return

        target[name] = converter(target[key]) if converter else target[key]
        del target[key]

    return invoke


def patch(name, rules):
    """Patches a nested object with specified set of rules"""

    def invoke(assets, target, key):
        value = target[key]

        target[name] = patch_instance(assets, value, rules)
        del target[key]

    return invoke


def if_enabled(name, rules):
    """Patches a particle system module object"""

    def invoke(assets, target, key):
        value = target[key]

        if 'enabled' not in value or value['enabled'] == 0:
            del target[key]
            return

        target[name] = patch_instance(assets, value, rules)
        del target[key]

    return invoke


def asset(name):
    """Patches an asset property"""

    def invoke(assets, target, key):
        value = target[key]

        if isinstance(value, dict):
            target[name] = assets.use(value['guid']) if 'guid' in value.keys() else None
        else:
            target[name] = [assets.use(item['guid']) for item in value]

        del target[key]

    return invoke


def reference(name):
    """Patches an object reference"""

    def invoke(assets, target, key):
        object_id = str(target[key]['fileID'])
        target[name] = None if object_id == '0' else object_id
        del target[key]

    return invoke


def vector(name, components):
    """Patches a vector property"""

    def invoke(assets, target, key):
        vector_value = target[key]
        target[name] = [float(vector_value[component]) for component in components if component in vector_value.keys()]
        del target[key]

    return invoke


def curve(name, converter=None):
    """Patches a parameter curve property"""

    def invoke(assets, target, key):
        value = target[key]

        scalar = value['scalar']
        curve_type = value['minMaxState']
        min_curve = Curve()
        max_curve = Curve()

        min_curve.parse(value['minCurve'], scalar, converter)
        max_curve.parse(value['maxCurve'], scalar, converter)

        if curve_type == ParameterType.CONSTANT:
            target[name] = dict(type='constant', value=max_curve.constant)
        elif curve_type == ParameterType.RANDOM_BETWEEN_CONSTANTS:
            target[name] = dict(type='randomBetweenConstants', value=[min_curve.constant, max_curve.constant])
        elif curve_type == ParameterType.CURVE:
            target[name] = dict(type='curve', value=max_curve.data)
        elif curve_type == ParameterType.RANDOM_BETWEEN_CURVES:
            target[name] = dict(type='randomBetweenCurves', value=[min_curve.data, max_curve.data])

        if key != name:
            del target[key]

    return invoke


def bursts(name):
    """Patches emitter bursts property"""

    def invoke(assets, target, key):
        result = []

        for i in range(0, target['m_BurstCount']):
            result.append(target['time' + str(i)])
            result.append(target['cnt' + str(i)])

        target[name] = result

    return invoke


def gradient(color_name, transparency_name):
    """Patches a gradient property"""

    def invoke(assets, target, key):
        value = target[key]

        curve_type = value['minMaxState']
        min_curve = Gradient()
        max_curve = Gradient()

        min_curve.parse(value['minGradient'])
        max_curve.parse(value['maxGradient'])

        assert 'transparency' not in target.keys()

        if curve_type == ParameterType.CURVE:
            target[color_name] = dict(type="curve", value=max_curve.rgb)
            target[transparency_name] = dict(type="curve", value=max_curve.alpha)
        elif curve_type == ParameterType.RANDOM_BETWEEN_CURVES:
            target[color_name] = dict(type="randomBetweenCurves", value=[min_curve.rgb, max_curve.rgb])
            target[transparency_name] = dict(type="randomBetweenCurves", value=[min_curve.alpha, max_curve.alpha])
        elif curve_type == ParameterType.CONSTANT:
            if 'rgba' not in value['maxColor'].keys():
                rgba = Rgba.from_object(value['maxColor'])
            else:
                rgba = Rgba(value['maxColor']['rgba'])

            target[color_name] = dict(type="constant", value=[rgba.red, rgba.green, rgba.blue])
            target[transparency_name] = dict(type="constant", value=rgba.alpha)
        else:
            target[color_name] = dict(type="curve", value=max_curve.constant_rgb)
            target[transparency_name] = dict(type="curve", value=max_curve.constant_alpha)

        del target[key]

    return invoke


def map_to_string(mappings):
    """Maps from an input value to a string"""

    def convert(value):
        key = str(value)
        assert key in mappings.keys(), "unhandled value %s" % key
        return mappings[key]

    return convert


def file_id(name, converter=None):
    """Converts a file id property"""

    def invoke(assets, target, key):
        value = target[key]['fileID']

        if name == key:
            target[key] = converter(value) if converter else value
            return

        target[name] = converter(value) if converter else value
        del target[key]

    return invoke


def key_value(name, rules):
    """Patches a key-value property"""

    def invoke(assets, target, key):
        result = {}

        for prop in target[key]:
            k = prop['first']['name']
            value = {}

            if k in rules.keys():
                value[k] = prop['second']
                value = patch_instance(assets, value, rules)
                result[value.keys()[0]] = value[value.keys()[0]]

        target[name] = result
        del target[key]

    return invoke


def patch_instance(assets, instance, rules):
    """Patches an instance"""

    object_items = instance.items()

    if '' in rules.keys():
        rules[''](assets, instance, '')

    for name, value in object_items:
        if name in rules.keys():
            rules[name](assets, instance, name)
        else:
            del instance[name]

    return instance


def radians_to_degrees(value):
    return value * 180 / math.pi


def rescale(value):
    return value * 100


def multiply(factor):
    """Multiplies a value by a given factor"""

    def invoke(value):
        return rescale(float(value) * factor)

    return invoke
