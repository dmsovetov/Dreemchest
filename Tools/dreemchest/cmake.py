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
import subprocess


def enable_option(value):
    """Converts a boolean option to a CMake ON/OFF switch"""
    return 'ON' if value else 'OFF'


def disable_option(value):
    """Converts a boolean option to a CMake ON/OFF switch"""
    return 'OFF' if value else 'ON'


def library_option(parameters, name, options):
    """Adds a library parameter base on passed options"""

    # Convert arguments to dictionary
    values = vars(options)

    # Convert a library name to lowercase
    name = name.lower()

    if values['no_%s' % name]:
        return

    if values['system_%s' % name]:
        parameters['SYSTEM_%s' % name.upper()] = True
    else:
        parameters['BUNDLED_%s' % name.upper()] = True


def generate(generator, source, output, parameters, rest='', quiet=False):
    """Invokes a CMake command to generate a build system"""

    # Create an output directory
    if not os.path.exists(output):
        os.makedirs(output)

    # Generate CMake parameter string
    cmd_args = ['-D%s=%s' % (key, parameters[key]) for key in parameters]

    # Generate a command line string
    command_line = 'cmake -E chdir %s cmake %s -G "%s" %s %s' % (output, os.path.abspath(source),
                                                                 generator,
                                                                 ' '.join(cmd_args),
                                                                 rest
                                                                 )

    with open(os.devnull, 'wb') as devnull:
        subprocess.check_call(command_line, shell=True, stdout=devnull if quiet else None, stderr=subprocess.STDOUT)


def build(source, target, configuration, quiet=False):
    """Build a CMake-generated project binary tree."""

    # Generate a command line string
    command_line = 'cmake --build %s --target %s --config %s' % (source, target, configuration)

    with open(os.devnull, 'wb') as devnull:
        subprocess.check_call(command_line, shell=True, stdout=devnull if quiet else None, stderr=subprocess.STDOUT)


class Command:
    """A base class for all CMake commands"""

    def __init__(self, parser, source_dir='.', output_dir='.'):
        """Constructs a platform configuration command"""

        parser.add_argument('--source',
                            help='a source directory to generate build system.',
                            default=source_dir)
        parser.add_argument('--output',
                            help='an output directory to place generated build system.',
                            default=output_dir)
        parser.add_argument('--prefix-path',
                            help='a path used for searching packages.',
                            default='.')
        parser.add_argument('--cpp',
                            help='specifies the C++ standard whose features that are required by a build system.',
                            type=int,
                            choices=[98, 11, 14],
                            default=98)

        parser.set_defaults(function=self.configure)

    def configure(self, options):
        """Performs basic build system configuration"""
        pass
