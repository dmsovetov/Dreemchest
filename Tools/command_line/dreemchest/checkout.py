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

import subprocess

REMOTE_URL = 'https://github.com/dmsovetov/Dreemchest'
BRANCH = 'renderer-refactoring'


class Command:
    """A command line tool to checkout a source code from a remote repository"""

    def __init__(self, parser):
        """Constructs a pull command line tool"""

        parser.add_argument('path',
                            help='a destination folder to checkout a source code'
                            )

        parser.set_defaults(function=self.checkout)

    @staticmethod
    def checkout(options):
        """Clones a remote repository"""

        command_line = 'git clone %s --branch %s --single-branch %s' % (REMOTE_URL, BRANCH, options.path);

        try:
            subprocess.check_call(command_line, shell=True, stdout=None, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError:
            pass
