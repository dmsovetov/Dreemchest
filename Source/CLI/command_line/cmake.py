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

GENERATE_COMMAND = '{bin}/cmake -E chdir {output} {bin}/cmake {source} -G "{generator}" {args} {rest} -Wno-dev'


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


class CMake:
    """A wrapper class used to invoke cmake command"""
    def __init__(self, home, toolchain=None, prefix_path=None, install_prefix=None, cxx_std=11):
        self._home = home
        self._options = dict(
            CMAKE_INSTALL_PREFIX=install_prefix,
            CMAKE_PREFIX_PATH=prefix_path,
            CMAKE_CXX_STANDARD=cxx_std
        )

        if toolchain:
            self._options['CMAKE_TOOLCHAIN_FILE'] = toolchain

    @property
    def home(self):
        """Returns a CMake home directory"""
        return self._home

    def build(self, generator, source_dir, binary_dir, target='install', configuration='Release', options=None):
        """Generates a binary tree and then builds a specified target for requested configuration"""

        # Prepare CMake options
        if not options:
            options = self._options
        else:
            options.update(self._options)

        try:
            # Generate a binary tree
            self.configure(generator, source_dir, binary_dir, options, '-Wno-dev -DCMAKE_BUILD_TYPE=' + configuration)

            # Build a generated tree
            rest = ''
            if generator == 'Unix Makefiles' or generator == 'MinGW Makefiles':
                rest = '-j8'

            self.build_tree(binary_dir, target=target, configuration=configuration, rest=rest)

        except subprocess.CalledProcessError:
            print 'Failed to build %s' % source_dir

        shutil.rmtree(binary_dir)

    def configure(self, generator, source, output, parameters, rest=''):
        """Invokes a CMake command to generate a build system"""

        # Create an output directory
        if not os.path.exists(output):
            os.makedirs(output)

        # Generate CMake parameter string
        parameters.update(self._options)
        cmd_args = ['-D%s=%s' % (key, parameters[key]) for key in parameters]

        # Generate a command line string
        command_line = GENERATE_COMMAND.format(
            bin=self._home,
            output=output,
            source=os.path.abspath(source),
            generator=generator,
            args=' '.join(cmd_args),
            rest=rest
        )

        subprocess.check_call(command_line, shell=True)

    def build_tree(self, source, target='install', configuration='Release', rest=''):
        """Build a CMake-generated project binary tree."""

        # Generate a command line string
        command_line = '{bin}/cmake --build {source} --target {target} --config {configuration} -- {rest}'.format(
            bin=self._home,
            source=source,
            target=target,
            configuration=configuration,
            rest=rest,
        )

        subprocess.check_call(command_line, shell=True)


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
        parser.add_argument('--api-level',
                            help='specifies the Android API level.',
                            default=24)

        parser.set_defaults(function=self.configure)

    def configure(self, options):
        """Performs basic build system configuration"""
        pass
