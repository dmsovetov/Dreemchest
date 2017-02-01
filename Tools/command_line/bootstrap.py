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
import yaml


class BootstrapCommand(cmake.Command):
    """A base class for all installation commands"""

    def __init__(self, parser, platforms, generators=[]):
        """Constructs a third party install command"""

        cmake.Command.__init__(self, parser, generators, output_dir=None, source_dir=None)

        # Load home directory from environment
        if 'DREEMCHEST_HOME' not in os.environ.keys():
            raise Exception('DREEMCHEST_HOME environment variable was not set')

        self._home = os.environ['DREEMCHEST_HOME']
        self._platforms = platforms

        # Load external libraries descriptor
        with open(os.path.join(self._home, 'Externals', 'Externals.yml')) as fh:
            self._externals = yaml.load(fh.read())

        # Add libraries to command line options
        for external in self._externals:
            name = external['name']
            parser.add_argument('--no-%s' % name.lower(),
                                help='do not build a %s.' % name,
                                action='store_true',
                                default=False
                                )

        # Add platforms to command line options
        for platform in platforms:
            parser.add_argument('--no-%s' % platform.lower(),
                                help='do not bootstrap for %s.' % platform,
                                action='store_true',
                                default=False
                                )

        parser.set_defaults(function=self.bootstrap)

    def bootstrap(self, options):
        # Make sure to pull all submodules before building them
        git.checkout_submodules()

        #if not getattr(options, 'no_emscripten'):
        #    if 'EMSCRIPTEN' not in os.environ.keys():
        #        raise Exception('No EMSCRIPTEN environment variable set')

        # Toolchain files for each platform
        toolchains = dict(
            iOS=os.path.join(self._home, 'CMake', 'Toolchains', 'iOS.cmake'),
            Android=os.path.join(self._home, 'CMake', 'Toolchains', 'Android.cmake'),
            macOS=None,
            Windows=None,
        #    Emscripten=os.path.join(os.environ['EMSCRIPTEN'], 'cmake', 'Modules', 'Platform', 'Emscripten.cmake')
        )

        # Now build each platform
        for platform in self._platforms:
            if getattr(options, 'no_' + platform.lower()):
                continue

            install_path = os.path.join(self._home, 'Build/Dependencies', platform)
            binary_dir = os.path.join(self._home, 'Externals/Projects')
            source_dir = os.path.join(self._home, 'Externals')

            # Build all external libraries
            for external in self._externals:
                name = external['name']
                cmake_options = external['options'] if 'options' in external.keys() else dict()
                source = external['source'] if 'source' in external.keys() else name
                platform_name = platform.lower()
                platform_options = external[platform_name] if platform_name in external.keys() else dict()

                if getattr(options, 'no_' + name.lower()):
                    continue

                if 'disabled' in platform_options.keys() and platform_options['disabled']:
                    continue

                if 'options' in platform_options.keys():
                    cmake_options.update(platform_options['options'])

                cmake.configure_and_build(options.generator if platform in ['iOS', 'macOS'] else 'Unix Makefiles',
                                          os.path.join(source_dir, source),
                                          os.path.join(binary_dir, name),
                                          prefix=install_path,
                                          install_prefix=install_path,
                                          target='install',
                                          configuration='Release',
                                          options=cmake_options,
                                          toolchain=toolchains[platform])

            shutil.rmtree(binary_dir)


class MacOSBootstrapCommand(BootstrapCommand):
    """A command line tool to build and install third party libraries bundled with a source code distribution"""

    def __init__(self, parser):
        """Constructs a third party install command"""

        BootstrapCommand.__init__(self, parser, ['macOS', 'iOS'], generators=['Xcode'])


class WindowsBootstrapCommand(BootstrapCommand):
    """A command line tool to build and install third party libraries bundled with a source code distribution"""

    def __init__(self, parser):
        """Constructs a third party install command"""

        BootstrapCommand.__init__(self, parser, ['Windows'], generators=['Visual Studio 12'])