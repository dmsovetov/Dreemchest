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

import shutil
import os
import cmake
import git
import command_line


class InstallCommand(cmake.Command):
    """A base class for all installation commands"""

    def __init__(self, parser, generators=[]):
        """Constructs a third party install command"""

        cmake.Command.__init__(self, parser, output_dir='Build/Dependencies', source_dir='Externals')

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

        parser.add_argument('--generator',
                            help='build system generator to be used.',
                            choices=generators,
                            default=generators[0])

        parser.set_defaults(function=self.install)

    @staticmethod
    def install(options):
        install_path = os.path.abspath(options.output)
        binary_dir = os.path.abspath(os.path.join(options.source, 'Projects'))
        source_dir = os.path.abspath(options.source)

        # Make sure to pull all submodules before building them
        git.checkout_submodules()

        if not options.no_zlib:
            cmake.configure_and_build(options.generator,
                                      os.path.join(source_dir, 'zlib'),
                                      os.path.join(binary_dir, 'zlib'),
                                      prefix=install_path,
                                      target='install'
                                      )

        if not options.no_curl:
            cmake.configure_and_build(options.generator,
                                      os.path.join(source_dir, 'curl'),
                                      os.path.join(binary_dir, 'curl'),
                                      prefix=install_path,
                                      options=dict(BUILD_CURL_EXE=False, CURL_STATICLIB=True),
                                      target='install'
                                      )

        if not options.no_box2d:
            cmake.configure_and_build(options.generator,
                                      os.path.join(source_dir, 'Box2D/Box2D'),
                                      os.path.join(binary_dir, 'Box2D'),
                                      prefix=install_path,
                                      options=dict(BOX2D_BUILD_EXAMPLES=False, BOX2D_INSTALL=True),
                                      target='install'
                                      )

        if not options.no_jsoncpp:
            cmake.configure_and_build(options.generator,
                                      os.path.join(source_dir, 'jsoncpp'),
                                      os.path.join(binary_dir, 'jsoncpp'),
                                      prefix=install_path,
                                      options=dict(JSONCPP_WITH_TESTS=False, JSONCPP_WITH_POST_BUILD_UNITTEST=False),
                                      target='install'
                                      )

        if not options.no_ogg:
            cmake.configure_and_build(options.generator,
                                      os.path.join(source_dir, 'ogg'),
                                      os.path.join(binary_dir, 'ogg'),
                                      prefix=install_path,
                                      target='install'
                                      )

        if not options.no_vorbis:
            cmake.configure_and_build(options.generator,
                                      os.path.join(source_dir, 'vorbis'),
                                      os.path.join(binary_dir, 'vorbis'),
                                      prefix=install_path,
                                      target='install'
                                      )

        if not options.no_gtest:
            cmake.configure_and_build(options.generator,
                                      os.path.join(source_dir, 'GoogleTest'),
                                      os.path.join(binary_dir, 'GoogleTest'),
                                      prefix=install_path,
                                      target='install',
                                      options=dict(gtest_force_shared_crt=True)
                                      )

        if not options.no_libpng:
            cmake.configure_and_build(options.generator,
                                      os.path.join(source_dir, 'libpng'),
                                      os.path.join(binary_dir, 'libpng'),
                                      prefix=install_path,
                                      options=dict(PNG_TESTS=False),
                                      target='install'
                                      )

        if not options.no_libtiff:
            cmake.configure_and_build(options.generator,
                                      os.path.join(source_dir, 'libtiff'),
                                      os.path.join(binary_dir, 'libtiff'),
                                      prefix=install_path,
                                      options=dict(CMAKE_CXX_FLAGS='-D_XKEYCHECK_H'),
                                      target='install'
                                      )

        if not options.no_lua:
            cmake.configure_and_build(options.generator,
                                      os.path.join(source_dir, 'lua'),
                                      os.path.join(binary_dir, 'lua'),
                                      prefix=install_path,
                                      options=dict(BUILD_SHARED_LIBS=False, LUA_USE_C89=True),
                                      target='install'
                                      )

        if not options.no_openal:
            cmake.configure_and_build(options.generator,
                                      os.path.join(source_dir, 'OpenAL'),
                                      os.path.join(binary_dir, 'OpenAL'),
                                      prefix=install_path,
                                      options=dict(ALSOFT_EXAMPLES=False, ALSOFT_TESTS=False, LIBTYPE='STATIC'),
                                      target='install'
                                      )

    #    shutil.rmtree(binary_dir)

    @staticmethod
    def _add_library(parser, name):
        """Adds a third party build options"""
        parser.add_argument('--no-%s' % name.lower(),
                            help='do not build a %s.' % name,
                            action='store_true',
                            default=False
                            )


class MacOSInstallCommand(InstallCommand):
    """A command line tool to build and install third party libraries bundled with a source code distribution"""

    def __init__(self, parser):
        """Constructs a third party install command"""

        InstallCommand.__init__(self, parser, generators=['Xcode'])


class WindowsInstallCommand(InstallCommand):
    """A command line tool to build and install third party libraries bundled with a source code distribution"""

    def __init__(self, parser):
        """Constructs a third party install command"""

        InstallCommand.__init__(self, parser, generators=['Visual Studio 12'])


class Command(command_line.Tool):
    """A command line tool to build and install dependencies for a specified target system"""

    def __init__(self, parser):
        """Constructs a configure command line tool"""

        command_line.Tool.__init__(self, parser, 'available platforms')

        self._add_command('windows', WindowsInstallCommand)
        self._add_command('macos', MacOSInstallCommand)