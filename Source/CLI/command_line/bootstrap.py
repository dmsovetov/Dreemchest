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
import env


class BootstrapCommand(cmake.Command):
    """A base class for all installation commands"""

    def __init__(self, parser, platforms, generators=[], fallback_generator=None):
        """Constructs a third party install command"""

        cmake.Command.__init__(self, parser, generators, output_dir=None, source_dir=None)

        self._platforms = platforms
        self._env = env.load()
        self._options = None
        self._fallback_generator = fallback_generator

        # Load external libraries descriptor
        with open(os.path.join(self.env.home, 'Externals', 'Externals.yml')) as fh:
            self._externals = yaml.load(fh.read())

        # Add libraries to command line options
        for external in self._externals:
            name = external['name']
            parser.add_argument('--no-%s' % name.lower(),
                                help='do not build a %s.' % name,
                                action='store_true',
                                default=False,
                                )

        # Add platforms to command line options
        for platform in platforms:
            parser.add_argument('--no-%s' % platform.lower(),
                                help='do not bootstrap for %s.' % platform,
                                action='store_true',
                                default=False,
                                )

        parser.add_argument('--no-build',
                            help='skips engine build and installation process',
                            action='store_true',
                            default=False,
                            )
        parser.set_defaults(function=self.bootstrap)

    @property
    def env(self):
        """Returns an active environment configuration"""
        return self._env

    @property
    def options(self):
        """Returns a command line options passed to a tool"""
        assert self._options is not None, "no command line options were set"
        return self._options

    def platform_toolchain(self, platform):
        """Returns a toolchain file for a requested platform"""
        return dict(
            iOS=self.env.ios_toolchain,
            Android=self.env.android_toolchain,
            Emscripten=self.env.emscripten_toolchain,
            macOS=None,
            Windows=None,
        )[platform]

    def platform_generator(self, platform):
        """Returns a generator used by a platform"""
        return self.options.generator if platform in ['iOS', 'macOS', 'Windows'] else self._fallback_generator

    def platform_options(self, platform):
        """Returns platform-specific configuration options"""

        if platform == 'Android':
            return dict(
                ANDROID_NATIVE_API_LEVEL='android-%s' % self.options.api_level,
                ANDROID_NDK=self.env.android_ndk
            )
        if platform == 'Emscripten':
            return dict(
                EMSCRIPTEN_ROOT_PATH=self.env.emscripten
            )

        return dict()

    @property
    def target_platforms(self):
        """Returns a list of target platforms"""
        return [platform for platform in self._platforms if getattr(self.options, 'no_' + platform.lower()) is False]

    @property
    def externals(self):
        """Returns a list of external libraries to be built"""
        result = [external for external in self._externals
                  if getattr(self.options, 'no_' + external['name'].lower()) is False]
        return result

    def build_externals(self, options):
        # Make sure to pull all submodules before building them
        git.checkout_submodules()

        if self.env.emscripten is None:
            print "Warning: '%s' environment variable is not set, Emscripten is disabled." % env.DREEMCHEST_EMSCRIPTEN
            options.no_emscripten = True

        if self.env.android is None:
            print "Warning: '%s' environment variable is not set, Android is disabled." % env.DREEMCHEST_ANDROID
            options.no_android = True

        # Now build each platform
        for platform in self.target_platforms:
            install_path = os.path.join(self.env.dependencies, platform)
            binary_dir = self.env.bootstrap_temp_dir
            source_dir = self.env.externals

            # Create a CMake instance to build for this platform
            cm = cmake.CMake(self.env.cmake,
                             toolchain=self.platform_toolchain(platform),
                             install_prefix=install_path,
                             prefix_path=install_path,
                             )

            # Build all external libraries
            for external in self.externals:
                name = external['name']
                cmake_options = external['options'] if 'options' in external.keys() else dict()
                source = external['source'] if 'source' in external.keys() else name
                platform_name = platform.lower()
                platform_options = external[platform_name] if platform_name in external.keys() else dict()

                if 'disabled' in platform_options.keys() and platform_options['disabled']:
                    continue

                if 'options' in platform_options.keys():
                    cmake_options.update(platform_options['options'])

                cmake_options.update(self.platform_options(platform))

                cm.build(self.platform_generator(platform),
                         os.path.join(source_dir, source),
                         os.path.join(binary_dir, name),
                         options=cmake_options)

            shutil.rmtree(binary_dir)

    def bootstrap(self, options):
        # Save command line options
        self._options = options

        # First build all required external libraries
        self.build_externals(options)

        # Now generate and build engine projects
        for platform in self.target_platforms:
            install_path = os.path.join(self.env.prebuilt, platform)
            binary_dir = os.path.join(self.env.projects, platform)
            prefix_path = os.path.join(self.env.dependencies, platform)

            # Create a CMake instance to build for this platform
            cm = cmake.CMake(self.env.cmake,
                             toolchain=self.platform_toolchain(platform),
                             install_prefix=install_path,
                             prefix_path=prefix_path,
                             )

            # Generate project
            cm.configure(self.platform_generator(platform), self.env.source, binary_dir, self.platform_options(platform))

            # Build project
            if self.options.no_build is False:
                try:
                    cm.build_tree(binary_dir)
                except Exception:
                    pass


class MacOSBootstrapCommand(BootstrapCommand):
    """A command line tool to build and install third party libraries bundled with a source code distribution"""

    def __init__(self, parser):
        """Constructs a third party install command"""

        BootstrapCommand.__init__(self,
                                  parser,
                                  ['macOS', 'iOS', 'Android', 'Emscripten'],
                                  generators=['Xcode'],
                                  fallback_generator='Unix Makefiles'
                                  )


class WindowsBootstrapCommand(BootstrapCommand):
    """A command line tool to build and install third party libraries bundled with a source code distribution"""

    def __init__(self, parser):
        """Constructs a third party install command"""

        BootstrapCommand.__init__(self,
                                  parser,
                                  ['Windows', 'Android', 'Emscripten'],
                                  generators=['Visual Studio 12'],
                                  fallback_generator='MinGW Makefiles'
                                  )