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

from collections import namedtuple
import os
import tempfile

# Define named tuple for environment configuration
Configuration = namedtuple('Configuration', ['home', 'cmake', 'android', 'emscripten'])

# Environment variable that points to a Dreemchest home directory
DREEMCHEST_HOME = 'DREEMCHEST_HOME'

# Environemt variable that points to a CMake folder used by Dreemchest
DREEMCHEST_CMAKE = 'DREEMCHEST_CMAKE'

# Environment variable that points to a CMake bin folder
DREEMCHEST_CMAKE_BIN = 'DREEMCHEST_CMAKE_BIN'

# Environment variable that points to Android SDK used by Dreemchest
DREEMCHEST_ANDROID = 'DREEMCHEST_ANDROID'

# Environment variable that points to Emscripten SDK used by Dreemchest
DREEMCHEST_EMSCRIPTEN = 'DREEMCHEST_EMSCRIPTEN'


class Configuration:
    """An active environment configuration"""
    def __init__(self):
        # Load home directory
        if DREEMCHEST_HOME not in os.environ.keys():
            raise Exception("'%s' environment variable should point to a Dreemchest home directory." % DREEMCHEST_HOME)

        self.home = os.environ[DREEMCHEST_HOME]

        # Load CMake directory
        if DREEMCHEST_CMAKE_BIN not in os.environ.keys():
            raise Exception("'%s' environment variable should point to a CMake directory." % DREEMCHEST_CMAKE_BIN)

        self._cmake = os.environ[DREEMCHEST_CMAKE_BIN]

        # Load Android SDK directory
        self._android = None

        if DREEMCHEST_ANDROID in os.environ.keys():
            self._android = os.environ[DREEMCHEST_ANDROID]

        # Load Emscripten SDK directory
        self._emscripten = None

        if DREEMCHEST_EMSCRIPTEN in os.environ.keys():
            self._emscripten = os.environ[DREEMCHEST_EMSCRIPTEN]

    @property
    def home(self):
        """Returns the Dreemchest home directory"""
        return self._home

    @property
    def cmake(self):
        """Returns CMake home directory"""
        return self._cmake

    @property
    def emscripten(self):
        """Returns the Emscripten SDK home directory"""
        return self._emscripten

    @property
    def emscripten_toolchain(self):
        if self.emscripten is None:
            return None

        return os.path.join(self.emscripten, 'cmake', 'Modules', 'Platform', 'Emscripten.cmake')

    @property
    def ios_toolchain(self):
        """Returns an iOS toolchain file"""
        return os.path.join(self.home, 'CMake', 'Toolchains', 'iOS.cmake')

    @property
    def android_toolchain(self):
        """Returns an Android toolchain file"""
        return os.path.join(self.home, 'CMake', 'Toolchains', 'Android.cmake')

    @property
    def android(self):
        """Returns the Android SDK home directory"""
        return self._android

    @property
    def android_ndk(self):
        """Returns the Android NDK home directory"""
        return os.path.join(self.android, 'ndk-bundle')

    @property
    def dependencies(self):
        """Returns a directory where all precompiled dependencies are stored"""
        return os.path.join(self.build_dir, 'Dependencies')

    @property
    def build_dir(self):
        """Returns a build directory"""
        return os.path.join(self.home, 'Build')

    @property
    def prebuilt(self):
        """Returns an prebuilt directory path"""
        return os.path.join(self.build_dir, 'Prebuilt')

    @property
    def externals(self):
        """Returns externals source directory"""
        return os.path.join(self.home, 'Externals')

    @property
    def source(self):
        """Returns engine source directory"""
        return os.path.join(self.home, 'Source')

    @property
    def projects(self):
        """Returns the Projects directory path"""
        return os.path.join(self.home, 'Projects')

    @property
    def bootstrap_temp_dir(self):
        """Returns a temporary directory where to store all intermediate artifacts for bootstrap process"""
        return os.path.join(tempfile.gettempdir(), 'Bootstrap')


def load():
    """Loads an active configuration from environment"""
    return Configuration()