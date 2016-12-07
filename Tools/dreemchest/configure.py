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
import cmake
import command_line


class PlatformConfigurationCommand(cmake.Command):
    """A base class for all platform configuration commands"""

    def __init__(self, parser, rendering_backend):
        """Constructs a platform configuration command"""

        cmake.Command.__init__(self, parser)

        # List of all available libraries
        self._libraries = []

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

        # Add renderer options
        renderer = parser.add_mutually_exclusive_group()
        renderer.add_argument('--no-renderer',
                              help='build with no rendering support.',
                              action='store_true',
                              default=False)

        if len(rendering_backend) > 1:
            renderer.add_argument('--renderer',
                                  help='specifies a rendering backend to be used.',
                                  type=str,
                                  choices=rendering_backend,
                                  default=rendering_backend[0])

        parser.add_argument('--no-sound',
                            help='build with no sound support',
                            action='store_true',
                            default=False)

        # Add third party libraries options
        self._add_library(parser, 'tiff')
        self._add_library(parser, 'jsoncpp')
        self._add_library(parser, 'zlib')
        self._add_library(parser, 'Box2D')
        self._add_library(parser, 'gtest')
        self._add_library(parser, 'png')
        self._add_library(parser, 'lua')
        self._add_library(parser, 'ogg')
        self._add_library(parser, 'vorbis')
        self._add_library(parser, 'OpenAL')
        self._add_library(parser, 'curl')

        parser.set_defaults(function=self.configure)

    def _add_library(self, parser, name, is_bundled=True):
        """Adds a third party library option to parser object"""

        lib = parser.add_mutually_exclusive_group()
        lib.add_argument('--no-%s' % name.lower(),
                         help='disables the %s library support.' % name,
                         action='store_true',
                         default=False)
        lib.add_argument('--system-%s' % name.lower(),
                         help='use %s from the operating system if possible.' % name,
                         action='store_true',
                         default=False if is_bundled else True)

        if is_bundled:
            lib.add_argument('--%s' % name.lower(),
                             help='use %s bundled with a source code distribution.' % name,
                             action='store_true',
                             default=True)

        self._libraries.append(name)

    def configure(self, options):
        """Performs basic build system configuration"""
        pass

    def _prepare(self, options):
        """Generates a command line arguments from an input options"""

        # Generate platform independent CMake arguments from passed options
        parameters = dict(
            DC_USE_PCH=cmake.enable_option(options.pch),
            DC_COMPOSER_ENABLED=cmake.disable_option(options.no_composer or options.no_qt),
            DC_OPENGL_ENABLED=cmake.disable_option(options.no_renderer),
            DC_SOUND_ENABLED=cmake.disable_option(options.no_sound),
            DC_WITH_RELIGHT=cmake.disable_option(options.no_relight),
            DC_BUILD_TESTS=cmake.disable_option(options.no_tests),
            DC_BUILD_EXAMPLES=cmake.disable_option(options.no_examples),
            CMAKE_CXX_STANDARD=options.cpp,
            DC_QT_SUPPORT=('disabled' if options.no_qt else options.qt).capitalize(),
            PREFIX_PATH=os.path.abspath(options.prefix_path),
        )

        # Generate CMake arguments from added libraries
        for lib in self._libraries:
            cmake.library_option(parameters, lib, options)

        return parameters


class DesktopConfigureCommand(PlatformConfigurationCommand):
    """Performs desktop OS build system configuration"""

    def __init__(self, parser, rendering_backend):
        """Constructs desktop configuration command"""

        PlatformConfigurationCommand.__init__(self, parser, rendering_backend)

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

        # Add third party libraries
        self._add_library(parser, 'FBX', is_bundled=False)


class WindowsConfigureCommand(DesktopConfigureCommand):
    """Performs Windows build system configuration"""

    def __init__(self, parser):
        """Constructs Windows configuration command"""

        DesktopConfigureCommand.__init__(self, parser, ['opengl', 'direct3d9', 'direct3d12'])

    def configure(self, options):
        """Performs basic build system configuration"""

        # Perform basic build configuration
        cmake_parameters = self._prepare(options)

        # Invoke a CMake command to generate a build system
        cmake.generate('Visual Studio 12', options.source, options.output, cmake_parameters)


class MacOSConfigureCommand(DesktopConfigureCommand):
    """Performs MacOS build system configuration"""

    def __init__(self, parser):
        """Constructs MacOS configuration command"""

        DesktopConfigureCommand.__init__(self, parser, ['opengl'])

    def configure(self, options):
        """Performs MacOS build system configuration"""

        # Perform basic build configuration
        cmake_parameters = self._prepare(options)

        # Invoke a CMake command to generate a build system
        cmake.generate('Xcode', options.source, options.output, cmake_parameters)


class Command(command_line.Tool):
    """A command line tool to configure a build system for a specified target system"""

    def __init__(self, parser):
        """Constructs a configure command line tool"""

        command_line.Tool.__init__(self, parser, 'available platforms')

        self._add_command('windows', WindowsConfigureCommand)
        self._add_command('macos', MacOSConfigureCommand)