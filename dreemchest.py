#!/usr/bin/python

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

import argparse
import os


def enable_option(value):
    """Converts a boolean option to a CMake ON/OFF switch"""
    return 'ON' if value else 'OFF'


def disable_option(value):
    """Converts a boolean option to a CMake ON/OFF switch"""
    return 'OFF' if value else 'ON'


class CommandLineTool:
    """A base class for all command line tools that have nested commands"""

    def __init__(self, parser, description):
        """Constructs a command line tool instance"""
        self._commands = parser.add_subparsers(help=description)

    def _add_command(self, name, cls):
        """Adds a nested command"""

        cls(self._commands.add_parser(name))


class PlatformConfigurationCommand:
    """A base class for all platform configuration commands"""

    def __init__(self, parser):
        """Constructs a platform configuration command"""
        parser.add_argument('--source',
                            help='a source directory to generate build system.',
                            default='.')
        parser.add_argument('--output',
                            help='an output directory to place generated build system.',
                            default='.')
        parser.add_argument('--cpp',
                            help='specifies the C++ standard whose features that are required by a build system.',
                            type=int,
                            choices=[98, 11, 14],
                            default=98)
        parser.add_argument('--pch',
                            help='generate a build system that uses precompiled headers.',
                            action='store_true',
                            default=False)
        parser.add_argument('--no-composer',
                            help='do not build the Composer tool.',
                            action='store_true',
                            default=False)
        parser.add_argument('--no-relight',
                            help='do not build the Relight lightmapping library.',
                            action='store_true',
                            default=False)
        parser.add_argument('--no-tests',
                            help='do not build unit tests.',
                            action='store_true',
                            default=False)
        parser.add_argument('--no-examples',
                            help='do not build examples.',
                            action='store_true',
                            default=False)

        # Add Qt library options
        qt = parser.add_mutually_exclusive_group()
        qt.add_argument('--no-qt',
                        help='disables a Qt library support.',
                        action='store_true',
                        default=False)
        qt.add_argument('--qt',
                        help='specifies the Qt library version that is required by a build system.',
                        type=str,
                        choices=['auto', 'qt4', 'qt5'],
                        default='auto')

        # Add renderer options
        renderer = parser.add_mutually_exclusive_group()
        renderer.add_argument('--no-renderer',
                            help='build with no rendering support.',
                            action='store_true',
                            default=False)
        renderer.add_argument('--renderer',
                            help='specifies a rendering backend to be used.',
                            type=str,
                            choices=['opengl'],
                            default='opengl')
        parser.set_defaults(function=self.configure)

    def configure(self, options):
        """Performs basic build system configuration"""

        raise NotImplementedError()

    @staticmethod
    def _prepare(options):
        """Generates a command line arguments from an input options"""

        # Create an output directory
        if not os.path.exists(options.output):
            os.makedirs(options.output)

        # Generate platform independent CMake arguments from passed options
        return dict(
            DC_USE_PCH=enable_option(options.pch),
            DC_COMPOSER_ENABLED=disable_option(options.no_composer or options.no_qt),
            DC_WITH_RELIGHT=disable_option(options.no_relight),
            DC_BUILD_TESTS=disable_option(options.no_tests),
            DC_BUILD_EXAMPLES=disable_option(options.no_examples),
            CMAKE_CXX_STANDARD=options.cpp,
            DC_QT_SUPPORT=('disabled' if options.no_qt else options.qt).capitalize()
        )

    @staticmethod
    def _generate_build_system(generator, options, parameters):
        """Invokes a CMake command to generate a build system"""

        # Generate CMake parameter string
        cmake_parameters = ['-D%s=%s' % (key, parameters[key]) for key in parameters]

        os.system('cmake -E chdir %s cmake %s -G "%s" %s' % (args.output, os.path.abspath(args.source), generator, ' '.join(cmake_parameters)))


class WindowsConfigureCommand(PlatformConfigurationCommand):
    """Performs Windows build system configuration"""

    def __init__(self, parser):
        """Constructs Windows configuration command"""

        PlatformConfigurationCommand.__init__(self, parser)

    def configure(self, options):
        """Performs basic build system configuration"""

        raise NotImplementedError()


class MacOSConfigureCommand(PlatformConfigurationCommand):
    """Performs MacOS build system configuration"""

    def __init__(self, parser):
        """Constructs MacOS configuration command"""

        PlatformConfigurationCommand.__init__(self, parser)

    def configure(self, options):
        """Performs MacOS build system configuration"""

        # Perform basic build configuration
        cmake = self._prepare(options)

        # Invoke a CMake command to generate a build system
        self._generate_build_system('Xcode', options, cmake)


class ConfigureCommand(CommandLineTool):
    """A command line tool to configure a build system for a specified target system"""

    def __init__(self, parser):
        """Constructs a configure command line tool"""

        CommandLineTool.__init__(self, parser, 'available platforms')

        self._add_command('windows', WindowsConfigureCommand)
        self._add_command('macos', MacOSConfigureCommand)


class CheckoutCommand:
    """A command line tool to checkout a source code from a remote repository"""

    def __init__(self, parser):
        """Constructs a pull command line tool"""


class Main(CommandLineTool):
    """An entry point to a command line tool"""

    def __init__(self, parser):
        """Constructs a command line tool."""

        CommandLineTool.__init__(self, parser, 'available commands')

        self._add_command('configure', ConfigureCommand)
        self._add_command('checkout', CheckoutCommand)

# Entry point
if __name__ == "__main__":
    root = argparse.ArgumentParser(description='Dreemchest engine command line tool')

    Main(root)

    args = root.parse_args()
    args.function(args)