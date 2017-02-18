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

import shutil
import os


class Importer(object):
    """Base class for all asset importers"""

    def __init__(self):
        """Constructs an asset importer instance"""

    @property
    def name(self):
        """Returns an importer name"""
        return type(self).__name__

    def process(self, output_path, asset):
        """Invokes an importing process for a passed asset"""

        absolute_output_path = os.path.join(output_path, asset.local_output_path)
        target_folder = os.path.dirname(absolute_output_path)

        if not os.path.exists(target_folder):
            os.makedirs(target_folder)

        print 'Copying asset %s...' % asset.uuid
        shutil.copyfile(asset.absolute_input_path, absolute_output_path)
