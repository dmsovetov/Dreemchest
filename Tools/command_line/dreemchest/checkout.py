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

import os
import git
import cmake


class Command:
    """A command line tool to checkout a source code from a remote repository"""

    def __init__(self, parser):
        """Constructs a pull command line tool"""

        parser.add_argument('path',
                            help='a destination folder to checkout a source code'
                            )
        parser.add_argument('--no-cmake',
                            help='do not download CMake distribution',
                            action='store_true',
                            default=False
                            )

        parser.set_defaults(function=self.checkout)

    @staticmethod
    def checkout(options):
        """Clones a remote repository"""

        # Save current directory
        current_dir = os.getcwd()

        # Checkout repository
        git.clone('https://github.com/dmsovetov/Dreemchest', options.path, branch='renderer-refactoring')

        # Change current directory
        os.chdir(options.path)

        # Download CMake
        if not options.no_cmake:
            cmake.download()

        # Initialize submodules
        git.checkout_submodule('Source/Nimble')
        git.checkout_submodule('Source/Relight')

        # Reset current directory
        os.chdir(current_dir)
