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

import inspect


def serializable_properties(instance):
    return inspect.getmembers(instance.__class__, lambda m: isinstance(m, property) and m.fset is not None)


def write(instance, save_type=True):
    """Writes an instance to a dictionary object"""

    if type(instance) in [int, str, bool]:
        return instance

    if isinstance(instance, list):
        return [write(item) for item in instance]

    if isinstance(instance, dict):
        return {k: write(v) for (k, v) in instance.items()}

    result = {k: write(getattr(instance, k)) for k, v in serializable_properties(instance)}

    if save_type:
        result.setdefault('type', instance.__class__.__name__)

    return result


def read(document, package):
    """Reads an instance from dictionary object"""

    if isinstance(document, list):
        return [read(item, package) for item in document]

    if isinstance(document, dict):
        if 'type' in document.keys():
            type_name = document.get('type')

            if not hasattr(package, type_name):
                raise AssertionError('unknown type %s' % type_name)

            cls = getattr(package, type_name)
            instance = cls()

            for key, v in document.items():
                if key == 'type':
                    continue

                class_property = getattr(cls, key)

                if class_property.fset is None:
                    AssertionError('%s is a read-only property' % key)

                value = read(v, package)
                class_property.fset(instance, value)

            return instance
        else:
            return {key: read(v, package) for (key, v) in document.items()}

    return document


def read_instance(instance, document, package=None):
    """Reads instance properties from a document"""

    if not isinstance(document, dict):
        raise ValueError('dict object expected')

    properties = serializable_properties(instance)

    for key, class_property in properties:
        if key not in document.keys():
            continue

        value = read(document[key], package)
        class_property.fset(instance, value)

    return instance
