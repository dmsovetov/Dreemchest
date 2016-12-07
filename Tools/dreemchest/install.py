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

import cmake
import shutil
import os
import subprocess


class Command(cmake.Command):
    """A command line tool to build and install third party libraries bundled with a source code distribution"""

    def __init__(self, parser):
        """Constructs a third party install command"""

        cmake.Command.__init__(self, parser, output_dir='Externals/Prebuilt/')

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

        parser.add_argument('generator',
                            help='build system generator to be used.',
                            choices=['Xcode', 'Visual Studio 12'])

        parser.set_defaults(function=self.install)

    def install(self, options):
        install_path = os.path.abspath(options.output)
        binary_dir = os.path.join(options.source, 'Projects')
        source_dir = os.path.abspath(options.source)

        if not options.no_zlib:
            self._build_and_install('zlib',
                                    options.generator,
                                    os.path.join(source_dir, 'zlib'),
                                    install_path,
                                    binary_dir,
                                    dict()
                                    )

        if not options.no_curl:
            self._build_and_install('curl',
                                    options.generator,
                                    os.path.join(source_dir, 'curl'),
                                    install_path,
                                    binary_dir,
                                    dict(BUILD_CURL_EXE=False, CURL_STATICLIB=True))

        if not options.no_box2d:
            self._build_and_install('Box2D',
                                    options.generator,
                                    os.path.join(source_dir, 'Box2D/Box2D'),
                                    install_path,
                                    binary_dir,
                                    dict(BOX2D_BUILD_EXAMPLES=False, BOX2D_INSTALL=True)
                                    )

        if not options.no_jsoncpp:
            self._build_and_install('jsoncpp',
                                    options.generator,
                                    os.path.join(source_dir, 'jsoncpp'),
                                    install_path,
                                    binary_dir,
                                    dict(JSONCPP_WITH_TESTS=False, JSONCPP_WITH_POST_BUILD_UNITTEST=False)
                                    )

        if not options.no_ogg:
            self._build_and_install('ogg',
                                    options.generator,
                                    os.path.join(source_dir, 'ogg'),
                                    install_path,
                                    binary_dir,
                                    dict())

        if not options.no_vorbis:
            self._build_and_install('vorbis',
                                    options.generator,
                                    os.path.join(source_dir, 'vorbis'),
                                    install_path,
                                    binary_dir,
                                    dict())

        if not options.no_gtest:
            self._build_and_install('GoogleTest',
                                    options.generator,
                                    os.path.join(source_dir, 'GoogleTest'),
                                    install_path,
                                    binary_dir,
                                    dict())

        if not options.no_libpng:
            self._build_and_install('libpng',
                                    options.generator,
                                    os.path.join(source_dir, 'libpng'),
                                    install_path,
                                    binary_dir,
                                    dict(PNG_TESTS=False))

        if not options.no_libtiff:
            self._build_and_install('libtiff',
                                    options.generator,
                                    os.path.join(source_dir, 'libtiff'),
                                    install_path,
                                    binary_dir,
                                    dict(CMAKE_CXX_FLAGS='-D_XKEYCHECK_H'))

        if not options.no_lua:
            self._build_and_install('lua',
                                    options.generator,
                                    os.path.join(source_dir, 'lua'),
                                    install_path,
                                    binary_dir,
                                    dict(BUILD_SHARED_LIBS=False))

        if not options.no_openal:
            self._build_and_install('OpenAL',
                                    options.generator,
                                    os.path.join(source_dir, 'OpenAL'),
                                    install_path,
                                    binary_dir,
                                    dict(ALSOFT_EXAMPLES=False, ALSOFT_TESTS=False, LIBTYPE='STATIC'))

        shutil.rmtree(binary_dir)

    @staticmethod
    def _build_and_install(library, generator, source, output, binary_dir, options):
        """Generates a binary tree, then builds it and installs to a specified destination folder"""

        # Set the CMAKE_INSTALL_PREFIX variable
        options['CMAKE_INSTALL_PREFIX:PATH'] = output

        # Generate a binary tree path
        binary_dir = os.path.join(binary_dir, library)

        print '%s => %s' % (library, output)

        try:
            # Generate a binary tree
            cmake.generate(generator, source, binary_dir, options, '-Wno-dev')

            # Build a generated tree
            cmake.build(binary_dir, 'install', 'Release')

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
