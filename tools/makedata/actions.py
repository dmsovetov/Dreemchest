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

import shutil, os

# action
class action:
    # ctor
    def __init__(self, source, dest):
        self.source, self.dest = source, dest

# copy
class copy(action):
    # ctor
    def __init__(self, source, dest):
        action.__init__(self, source, dest)

    # call
    def __call__(self):
        print 'Copying', self.source, 'from', self.dest
        shutil.copyfile(self.source, self.dest)

# compress
class compress(action):
    # ctor
    def __init__(self, source, dest):
        action.__init__(self, source, dest)

    # call
    def __call__(self):
        print 'Compress', self.source

# png_quant
class png_quant(action):
    # ctor
    def __init__(self, source, dest):
        action.__init__(self, source, dest)
        
        if not 'PNG_QUANT' in os.environ:
            raise Exception('Pngquant not found, make sure that PNG_QUANT enviroment variable is set')