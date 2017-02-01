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
import shutil

GENERATE_COMMAND = 'cmake -E chdir {output} cmake {source} -G "{generator}" {args} {rest} -Wno-dev'


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
    if values['no_%s' % name.lower()]:
        parameters['CMAKE_DISABLE_FIND_PACKAGE_%s' % name] = 'ON'


def generate(generator, source, output, parameters, rest=''):
    """Invokes a CMake command to generate a build system"""

    # Create an output directory
    if not os.path.exists(output):
        os.makedirs(output)

    # Generate CMake parameter string
    cmd_args = ['-D%s=%s' % (key, parameters[key]) for key in parameters]

    # Generate a command line string
    command_line = GENERATE_COMMAND.format(
        output=output,
        source=os.path.abspath(source),
        generator=generator,
        args=' '.join(cmd_args),
        rest=rest
        )

    subprocess.check_call(command_line, shell=True)


def build(source, target, configuration):
    """Build a CMake-generated project binary tree."""

    # Generate a command line string
    command_line = 'cmake --build %s --target %s --config %s' % (source, target, configuration)

    subprocess.check_call(command_line, shell=True)


def configure_and_build(generator, source_dir, binary_dir,
                        target='ALL_BUILD',
                        configuration='Release',
                        install_prefix=None,
                        prefix=None,
                        options=None,
                        toolchain=None):
    """Generates a binary tree and then builds a specified target for requested configuration"""

    if not options:
        options = dict()

    # Set the CMAKE_INSTALL_PREFIX variable
    if install_prefix:
        options['CMAKE_INSTALL_PREFIX'] = install_prefix

    # Set the CMAKE_PREFIX_PATH
    if prefix:
        options['CMAKE_PREFIX_PATH'] = prefix

    # Set the CMAKE_TOOLCHAIN_FILE variable
    if toolchain:
        options['CMAKE_TOOLCHAIN_FILE'] = toolchain

    try:
        # Generate a binary tree
        generate(generator, source_dir, binary_dir, options, '-Wno-dev')

        # Build a generated tree
        build(binary_dir, target, configuration)

    except subprocess.CalledProcessError:
        print 'Failed to build %s' % source_dir

    shutil.rmtree(binary_dir)


class Command:
    """A base class for all CMake commands"""

    def __init__(self, parser, generators, source_dir='.', output_dir='.', prefix_path='.'):
        """Constructs a platform configuration command"""

        if source_dir is not None:
            parser.add_argument('--source',
                                help='a source directory to generate build system.',
                                default=source_dir)

        if output_dir is not None:
            parser.add_argument('--output',
                                help='an output directory to place generated build system.',
                                default=output_dir)

        parser.add_argument('--prefix-path',
                            help='a path used for searching packages.',
                            default=prefix_path)
        parser.add_argument('--cpp',
                            help='specifies the C++ standard whose features that are required by a build system.',
                            type=int,
                            choices=[98, 11, 14],
                            default=11)
        parser.add_argument('--generator',
                            help='build system generator to be used.',
                            choices=generators,
                            default=generators[0])
        parser.add_argument('--configuration',
                            help='a configuration to be built.',
                            choices=['debug', 'release'],
                            default='release')

        parser.set_defaults(function=self.configure)

    def configure(self, options):
        """Performs basic build system configuration"""
        pass
