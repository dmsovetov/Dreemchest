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
import subprocess
import shutil


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


def cmake_generate(generator, source, output, parameters, rest='', quiet=False):
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


def cmake_build(source, target, configuration, quiet=False):
    """Build a CMake-generated project binary tree."""

    # Generate a command line string
    command_line = 'cmake --build %s --target %s --config %s' % (source, target, configuration)

    with open(os.devnull, 'wb') as devnull:
        subprocess.check_call(command_line, shell=True, stdout=devnull if quiet else None, stderr=subprocess.STDOUT)


class CommandLineTool:
    """A base class for all command line tools that have nested commands"""

    def __init__(self, parser, description):
        """Constructs a command line tool instance"""
        self._commands = parser.add_subparsers(help=description)

    def _add_command(self, name, cls):
        """Adds a nested command"""

        cls(self._commands.add_parser(name))


class CMakeCommand:
    """A base class for all CMake commands"""

    def __init__(self, parser, source_dir='.', output_dir='.'):
        """Constructs a platform configuration command"""

        parser.add_argument('--source',
                            help='a source directory to generate build system.',
                            default=source_dir)
        parser.add_argument('--output',
                            help='an output directory to place generated build system.',
                            default=output_dir)
        parser.add_argument('--cpp',
                            help='specifies the C++ standard whose features that are required by a build system.',
                            type=int,
                            choices=[98, 11, 14],
                            default=98)

        parser.set_defaults(function=self.configure)

    def configure(self, options):
        """Performs basic build system configuration"""
        pass


class PlatformConfigurationCommand(CMakeCommand):
    """A base class for all platform configuration commands"""

    def __init__(self, parser, rendering_backend):
        """Constructs a platform configuration command"""

        CMakeCommand.__init__(self, parser)

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
        self._add_library(parser, 'libtiff')
        self._add_library(parser, 'jsoncpp')
        self._add_library(parser, 'zlib')
        self._add_library(parser, 'Box2D')
        self._add_library(parser, 'gtest')
        self._add_library(parser, 'libpng')
        self._add_library(parser, 'lua')
        self._add_library(parser, 'oggvorbis')
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
            DC_USE_PCH=enable_option(options.pch),
            DC_COMPOSER_ENABLED=disable_option(options.no_composer or options.no_qt),
            DC_OPENGL_ENABLED=disable_option(options.no_renderer),
            DC_SOUND_ENABLED=disable_option(options.no_sound),
            DC_WITH_RELIGHT=disable_option(options.no_relight),
            DC_BUILD_TESTS=disable_option(options.no_tests),
            DC_BUILD_EXAMPLES=disable_option(options.no_examples),
            CMAKE_CXX_STANDARD=options.cpp,
            DC_QT_SUPPORT=('disabled' if options.no_qt else options.qt).capitalize(),
            CMAKE_PREFIX_PATH=os.path.abspath('Externals/Prebuilt/MacOS'),
        )

        # Generate CMake arguments from added libraries
        for lib in self._libraries:
            library_option(parameters, lib, options)

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

        raise NotImplementedError()


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
        cmake_generate('Xcode', options.source, options.output, cmake_parameters)


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


class InstallCommand(CMakeCommand):
    """A command line tool to build and install third party libraries bundled with a source code distribution"""

    def __init__(self, parser):
        """Constructs a third party install command"""

        CMakeCommand.__init__(self, parser, output_dir='Externals/Prebuilt/MacOS')

        self._add_library(parser, 'zlib')
        self._add_library(parser, 'Box2D')
        self._add_library(parser, 'OpenAL')
        self._add_library(parser, 'GTest')
        self._add_library(parser, 'jsoncpp')
        self._add_library(parser, 'libpng')
        self._add_library(parser, 'libtiff')
        self._add_library(parser, 'lua')
        self._add_library(parser, 'ogg')
        self._add_library(parser, 'vorbis')
        self._add_library(parser, 'curl')

        parser.set_defaults(function=self.install)

    def install(self, options):
        install_path = os.path.abspath(options.output)

        if not options.no_zlib:
            self._build_and_install('zlib',
                                    'Xcode',
                                    'Externals/zlib',
                                    install_path,
                                    dict()
                                    )

        if not options.no_curl:
            self._build_and_install('curl',
                                    'Xcode',
                                    'Externals/curl',
                                    install_path,
                                    dict(BUILD_CURL_EXE=False, CURL_STATICLIB=True))

        if not options.no_box2d:
            self._build_and_install('Box2D',
                                    'Xcode',
                                    'Externals/Box2D/Box2D',
                                    install_path,
                                    dict(BOX2D_BUILD_EXAMPLES=False)
                                    )

        if not options.no_jsoncpp:
            self._build_and_install('jsoncpp',
                                    'Xcode',
                                    'Externals/jsoncpp',
                                    install_path,
                                    dict(JSONCPP_WITH_TESTS=False, JSONCPP_WITH_POST_BUILD_UNITTEST=False)
                                    )

        if not options.no_ogg:
            self._build_and_install('ogg',
                                    'Xcode',
                                    'Externals/ogg',
                                    install_path,
                                    dict())

        if not options.no_vorbis:
            self._build_and_install('vorbis',
                                    'Xcode',
                                    'Externals/vorbis',
                                    install_path,
                                    dict())

        if not options.no_gtest:
            self._build_and_install('GoogleTest',
                                    'Xcode',
                                    'Externals/GoogleTest',
                                    install_path,
                                    dict())

        if not options.no_libpng:
            self._build_and_install('libpng',
                                    'Xcode',
                                    'Externals/libpng',
                                    install_path,
                                    dict(PNG_TESTS=False))

        if not options.no_libtiff:
            self._build_and_install('libtiff',
                                    'Xcode',
                                    'Externals/libtiff',
                                    install_path,
                                    dict())

        if not options.no_lua:
            self._build_and_install('lua',
                                    'Xcode',
                                    'Externals/lua',
                                    install_path,
                                    dict(BUILD_SHARED_LIBS=False))

        if not options.no_openal:
            self._build_and_install('OpenAL',
                                    'Xcode',
                                    'Externals/OpenAL',
                                    install_path,
                                    dict(ALSOFT_EXAMPLES=False, ALSOFT_TESTS=False, LIBTYPE='STATIC'))

        shutil.rmtree('Projects/Externals')

    @staticmethod
    def _build_and_install(library, generator, source, output, options):
        """Generates a binary tree, then builds it and installs to a specified destination folder"""

        # Set the CMAKE_INSTALL_PREFIX variable
        options['CMAKE_INSTALL_PREFIX:PATH'] = output

        # Generate a binary tree path
        binary_dir = 'Projects/Externals/%s' % library

        print '%s => %s' % (library, output)

        try:
            # Generate a binary tree
            cmake_generate(generator, source, binary_dir, options, '-Wno-dev')

            # Build a generated tree
            cmake_build(binary_dir, 'install', 'Release')

        except subprocess.CalledProcessError:
            print 'Failed to build %s' % library

        shutil.rmtree(binary_dir)

    @staticmethod
    def _add_library(parser, name):
        """Adds a third party build options"""
        parser.add_argument('--no-%s' % name.lower(),
                            help='do not build a %s.' % name,
                            action='store_true',
                            default=False
                            )


class Main(CommandLineTool):
    """An entry point to a command line tool"""

    def __init__(self, parser):
        """Constructs a command line tool."""

        CommandLineTool.__init__(self, parser, 'available commands')

        self._add_command('configure', ConfigureCommand)
        self._add_command('checkout', CheckoutCommand)
        self._add_command('install', InstallCommand)

# Entry point
if __name__ == "__main__":
    root = argparse.ArgumentParser(description='Dreemchest engine command line tool')

    Main(root)

    args = root.parse_args()
    args.function(args)