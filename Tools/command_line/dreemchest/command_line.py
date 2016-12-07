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


class Tool:
    """A base class for all command line tools that have nested commands"""

    def __init__(self, parser, description):
        """Constructs a command line tool instance"""
        self._commands = parser.add_subparsers(help=description)

    def _add_command(self, name, cls):
        """Adds a nested command"""

        cls(self._commands.add_parser(name))


def add_component(parser, name, description=None, options=[]):
    """Adds a library component option to a parser object"""

    option = parser.add_mutually_exclusive_group()
    option.add_argument('--no-%s' % name.lower(),
                        help=description,
                        action='store_true',
                        default=False)

    if len(options) > 1:
        option.add_argument('--renderer',
                            help='specifies a rendering backend to be used.',
                            type=str,
                            choices=options,
                            default=options[0])


def add_system_library(parser, name, versions=None):
    """Adds a system library option to a parser object"""

    option = parser.add_mutually_exclusive_group()
    option.add_argument('--no-%s' % name.lower(),
                        help='disables a Qt library support.',
                        action='store_true',
                        default=False)
    option.add_argument('--%s' % name.lower(),
                        help='specifies the %s library version that is required by a build system.' % name,
                        type=str,
                        choices=versions,
                        default=versions[0])


def add_library(parser, name, is_bundled=True):
    """Adds a third party library option to a parser object"""

    option = parser.add_mutually_exclusive_group()
    option.add_argument('--no-%s' % name.lower(),
                        help='disables the %s library support.' % name,
                        action='store_true',
                        default=False)
    option.add_argument('--system-%s' % name.lower(),
                        help='use %s from the operating system if possible.' % name,
                        action='store_true',
                        default=False if is_bundled else True)

    if is_bundled:
        option.add_argument('--%s' % name.lower(),
                            help='use %s bundled with a source code distribution.' % name,
                            action='store_true',
                            default=True)