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
import sys
import os
import urllib2
import tarfile
import zipfile
import subprocess
import shutil
from collections import namedtuple

DREEMCHEST_HOME = 'DREEMCHEST_HOME'
DREEMCHEST_CMAKE = 'DREEMCHEST_CMAKE'
DREEMCHEST_CMAKE_BIN = 'DREEMCHEST_CMAKE_BIN'
DREEMCHEST_ANDROID = 'DREEMCHEST_ANDROID'
DREEMCHEST_EMSCRIPTEN = 'DREEMCHEST_EMSCRIPTEN'
ANDROID = 'ANDROID_HOME'
EMSCRIPTEN = 'EMSCRIPTEN'

EnvironmentPaths = namedtuple('EnvironmentPaths', ['home', 'cmake', 'android', 'emscripten'])


def platform_name():
    """Returns a platform identifier"""
    if sys.platform == 'win32':
        return 'windows'
    if sys.platform == 'darwin':
        return 'mac'
    if sys.platform.startswith('linux'):
        return 'linux'


class Environment:
    """A base class for all environment configurations"""

    def __init__(self):
        """Constructs the Environment instance"""
        pass

    def configure(self, paths):
        """Performs basic environment configuration"""

        self._paths = paths
        self.set_variable(DREEMCHEST_HOME, paths.home)
        self.set_variable(DREEMCHEST_ANDROID, paths.android)
        self.set_variable(DREEMCHEST_CMAKE, paths.cmake)

        if paths.emscripten:
            self.set_variable(DREEMCHEST_EMSCRIPTEN, paths.emscripten)

    def set_variable(self, name, value):
        """Sets an environment variable"""

        raise Exception('an environment variable could not be set')

    def add_path(self, path):
        """Adds a specified path to an environment"""

        raise Exception('a path could not be added to an environment')


class UnixEnvironment(Environment):
    """A base class for all Unix environments"""

    def __init__(self, bash_files):
        """Constructs the UnixEnvironment instance"""

        Environment.__init__(self)

        self._bash_files = [os.path.expanduser('~/' + bash_file) for bash_file in bash_files]
        self._bash_file = self.bash_file()

    def bash_file(self):
        """Returns a bash file name to be updated"""
        for file_name in self._bash_files:
            if os.path.exists(file_name):
                return file_name

        return self._bash_files[0]

    def _append_to_bash(self, line):
        """Appends text to a bash configuration file"""

        with open(self._bash_file, 'a') as fh:
            fh.write(line)
            fh.close()

    def configure(self, paths):
        """Performs Unix environment configuration"""

        self.uninstall()

        Environment.configure(self, paths)

    def set_variable(self, name, value):
        """Writes an environment variable to bash configuration file"""

        self._append_to_bash('export {name}={value}\n'.format(name=name, value=value))
        print 'Environment variable {name} was set to {value}'.format(name=name, value=value)

    def add_path(self, path):
        """Writes path export to bash configuration file"""

        self._append_to_bash('export PATH={path}:$PATH\n'.format(path=path))
        print 'Path {path} added to environment'.format(path=path)

    def uninstall(self):
        """Cleans up previous environment configuration"""

        content = ""

        with open(self._bash_file) as fh:
            for line in fh.readlines():
                if line.find('DREEMCHEST_') == -1:
                    content += line
            fh.close()

        with open(self._bash_file, 'wt') as fh:
            if not content.endswith('\n'):
                content += '\n'

            fh.write(content)
            fh.close()


class MacOSEnvironment(UnixEnvironment):
    """Configures mac OS environment variables"""

    def __init__(self, args):
        """Constructs the MacOSEnvironment instance."""

        UnixEnvironment.__init__(self, ['.bash_profile', '.bash_login', '.profile'])
        self._args = args

    def configure(self, paths):
        """Performs Unix environment configuration"""

        UnixEnvironment.configure(self, paths)

        # Append macOS bundle path
        cmake_bin = os.path.join(paths.cmake, 'CMake.app', 'Contents', 'bin')
        self.set_variable(DREEMCHEST_CMAKE_BIN, cmake_bin)

        # Add DREEMCHEST_HOME variable to path
        self.add_path('$' + DREEMCHEST_HOME)


class WindowsEnvironment(Environment):
    """Configures Windows environment"""

    def __init__(self, args):
        """Constructs the WindowsEnvironment instance."""

        Environment.__init__(self)
        self._path = os.environ['PATH']

    def set_variable(self, name, value):
        """Sets an environment variable"""
        import _winreg
        try:
            env = None
            env = _winreg.OpenKeyEx(_winreg.HKEY_CURRENT_USER,
                                    'Environment',
                                    0,
                                    _winreg.KEY_SET_VALUE | _winreg.KEY_READ)
            _winreg.SetValueEx(env, name, 0, _winreg.REG_SZ, value)
            _winreg.FlushKey(env)
            _winreg.CloseKey(env)
        except Exception:
            if env:
                _winreg.CloseKey(env)
            raise Exception("failed to set '%s' environment variable" % name)

    def configure(self, paths):
        """Performs Windows environment configuration"""

        Environment.configure(self, paths)

        # Append CMake executable location
        cmake_bin = os.path.join(paths.cmake, 'bin')
        self.set_variable(DREEMCHEST_CMAKE_BIN, cmake_bin)

        # Add DREEMCHEST_HOME to path
        self.add_path('%' + DREEMCHEST_HOME + '%')

    def add_path(self, path):
        """Adds a specified path to an environment"""

        if not self._path.endswith(';'):
            self._path += ';'
        self._path += path

        self.set_variable('PATH', self._path)
        print 'Path {path} added to environment'.format(path=path)


def configure_environment(args, paths):
    """Configures system environment"""
    environments = dict(mac=MacOSEnvironment, windows=WindowsEnvironment)

    name = platform_name()

    if name not in environments.keys():
        raise Exception('unsupported platform')

    environment = environments[name](args)
    environment.configure(paths)

    return environment


def download(url, file_name):
    """Downloads a file from a specified remote URL"""
    u = urllib2.urlopen(url)

    f = open(file_name, 'wb')
    meta = u.info()
    file_size = int(meta.getheaders("Content-Length")[0])
    print "Downloading: %s Bytes: %s" % (url, file_size)

    file_size_dl = 0
    block_sz = 8192
    while True:
        dl_buffer = u.read(block_sz)
        if not dl_buffer:
            break

        file_size_dl += len(dl_buffer)
        f.write(dl_buffer)
        status = "\r%10d  [%3.2f%%]" % (file_size_dl, file_size_dl * 100. / file_size)
        print status,
        sys.stdout.flush()

    print '\n'
    f.close()


def extract(file_name, path):
    """Extracts all files from an archive"""
    name, ext = os.path.splitext(file_name)

    # Create a target directory
    if not os.path.exists(path):
        os.makedirs(path)

    if ext == '.gz':
        tar = tarfile.open(file_name, "r")
        tar.extractall(path)
        name, ext = os.path.splitext(name)
    elif ext == '.zip':
        def extract_file(zf, info, extract_dir):
            zf.extract(info.filename, path=extract_dir)
            out_path = os.path.join(extract_dir, info.filename)

            perm = info.external_attr >> 16L
            os.chmod(out_path, perm)

        with zipfile.ZipFile(file_name, "r") as z:
            for info in z.infolist():
                extract_file(z, info, path)

    os.remove(file_name)

    return os.path.join(path, name)


def download_cmake(version):
    """
    Downloads a CMake build system of a specified version
    CMake download URL is formed by a following principle:
        https://cmake.org/files/v{major}/cmake-{version}.{minor}-{platform}.{extension}
    """

    platform = platform_name()
    extensions = {'windows': 'zip', 'linux': 'tar.gz', 'mac': 'tar.gz'}
    platforms = {'windows': 'win32-x86', 'linux': 'Linux-x86_64', 'mac': 'Darwin-x86_64'}

    if platform not in extensions:
        raise Exception('CMake could not be downloaded, unknown extension')
    if platform not in platforms:
        raise Exception('CMake could not be downloaded, unsupported platform')

    numbers = version.split('.')
    major = '.'.join(numbers[:2])
    minor = numbers[2]

    url = 'https://cmake.org/files/v{major}/cmake-{major}.{minor}-{platform}.{extension}'.format(
        major=major,
        minor=minor,
        platform=platforms[platform],
        extension=extensions[platform]
    )

    file_name = url.split('/')[-1]

    try:
        # Download CMake
        download(url, file_name)

        # Extract CMake
        path = extract(file_name, 'Tools')

        return path
    except:
        raise Exception('CMake could not be downloaded, no such version')


def invoke(command):
    """Invokes a command as a subprocess"""

    try:
        subprocess.check_call(command, shell=True)
        return True
    except subprocess.CalledProcessError:
        return False


def install_cmake(version):
    # Skip CMake installation if we already have a valid installation
    if DREEMCHEST_CMAKE_BIN in os.environ.keys():
        try:
            cmake_path = os.environ[DREEMCHEST_CMAKE_BIN]
            cmake_command = os.path.join(cmake_path, 'cmake --version')

            if not os.path.exists(os.path.join(cmake_path, 'cmake')):
                raise Exception('cmake executable does not exist')

            result = subprocess.check_output(cmake_command, shell=True)
            cmake_version = result.split("\n\n")[0].split(' ')[2]

            if cmake_version == version:
                print 'Found existing CMake %s executable at %s' % (cmake_version, cmake_path)
                return cmake_path

            print 'Found CMake %s, but %s was requested, downloading the required version...' % (cmake_version, version)
        except subprocess.CalledProcessError:
            print "Warning: 'DREEMCHEST_CMAKE' variable points to an invalid folder"
        except Exception as err:
            print "Warning: %s" % err

    # Download and extract CMake
    return download_cmake(version)


def install_android(version, api_levels, architectures):
    # Skip Android SDK installation if we already have a valid installation
    if DREEMCHEST_ANDROID in os.environ.keys():
        print 'Android SDK installation found at %s' % os.environ[DREEMCHEST_ANDROID]
        return os.environ[DREEMCHEST_ANDROID]

    if ANDROID in os.environ.keys():
        print 'Android SDK installation found at %s' % os.environ[ANDROID]
        return os.environ[ANDROID]

    # Download an Android SDK
    platform = {'windows': 'windows', 'linux': 'linux', 'mac': 'macosx'}[platform_name()]

    # Format an SDK url and file name
    url = 'https://dl.google.com/android/repository/tools_r{version}-{platform}.zip'.format(
        version=version,
        platform=platform
    )

    try:
        file_name = 'android-sdk.zip'

        # Download Android SDK
        download(url, file_name)

        # Extract Android SDK
        android_path = extract(file_name, os.path.join('Tools', 'android-%s' % version))
        android_path = android_path.replace('android-sdk', '')  # HOTFIX
    except:
        raise Exception('Android SDK could not be downloaded, no such version')

    sdk_manager = os.path.join(android_path, 'tools', 'bin', 'sdkmanager')

    # Install all required packages
    for api_level in api_levels:
        print 'Installing Android API level %s...' % api_level
        if not invoke(sdk_manager + ' "platforms;android-%s"' % api_level):
            raise Exception('Failed to install Android API level %s' % api_level)

        for arch in architectures:
            print 'Installing Android system image for %s architecture with API level %s' % (arch, api_level)
            invoke(sdk_manager + ' "system-images;android-%s;default;%s"' % (api_level, arch))

    print 'Installing Android tools...'
    if not invoke(sdk_manager + ' tools'):
        raise Exception('Failed to install Android tools')

    print 'Installing Android platform tools...'
    if not invoke(sdk_manager + ' platform-tools'):
        raise Exception('Failed to install Android platform tools')

    print 'Installing Android NDK...'
    if not invoke(sdk_manager + ' ndk-bundle'):
        raise Exception('Failed to install an Android NDK')

    return android_path


def install_emscripten(version):
    # Skip Android SDK installation if we already have a valid installation
    if DREEMCHEST_EMSCRIPTEN in os.environ.keys():
        print 'Emscripten installation found at %s' % os.environ[DREEMCHEST_EMSCRIPTEN]
        return os.environ[DREEMCHEST_EMSCRIPTEN]

    if EMSCRIPTEN in os.environ.keys():
        print 'Emscripten installation found at %s' % os.environ[EMSCRIPTEN]
        return os.environ[EMSCRIPTEN]

    # Download an Emscripten SDK
    url = dict(
        windows='https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-1.35.0-portable-64bit.zip',
        linux='https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz',
        mac='https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz',
    )[platform_name()]

    file_name = url.split('/')[-1]

    download(url, file_name)

    # Extract contents
    emscripten_home = extract(file_name, 'Tools')
    emscripten_home = emscripten_home.replace('emsdk-portable', 'emsdk_portable')  # HOTFIX

    # Install Emscripten SDK
    emsdk = os.path.join(emscripten_home, 'emsdk')
    print 'Installing Emscripten SDK...'
    invoke(emsdk + ' update')
    invoke(emsdk + ' install latest')
    invoke(emsdk + ' activate latest')

    # Patch the Emscripten.cmake toolchain file
    emscripten_path = os.path.join(emscripten_home, 'emscripten', '1.35.0')
    source_toolchain = os.path.join('CMake', 'Toolchains', 'Emscripten.cmake')
    dest_toolchain = os.path.join(emscripten_path, 'cmake', 'Modules', 'Platform', 'Emscripten.cmake')

    shutil.copyfile(source_toolchain, dest_toolchain)

    return emscripten_home


def main():
    """A command line tool entry point"""

    parser = argparse.ArgumentParser(prog='setup', description='Dreemchest engine install tool')
    parser.add_argument('--cmake', help='a CMake version to be installed.', type=str, default='3.4.3')

    android_options = parser.add_mutually_exclusive_group()
    android_options.add_argument('--no-android',
                                 help="skips an Android SDK installation",
                                 action='store_true',
                                 default=False)
    android_options.add_argument('--android',
                                 help='an Android SDK version to be installed',
                                 type=str,
                                 default='25.2.3')

    emscripten_options = parser.add_mutually_exclusive_group()
    emscripten_options.add_argument('--no-emscripten',
                                 help="skips an Emscripten SDK installation",
                                 action='store_true',
                                 default=False)
    emscripten_options.add_argument('--emscripten',
                                    help='an Android SDK version to be installed',
                                    action='store_true',
                                    default=True)

    args = parser.parse_args()

    # Download CMake
    cmake_path = install_cmake(args.cmake)
    android_path = install_android(args.android, [24], ['arm64-v8a', 'armeabi-v7a', 'x86', 'x86_64'])
    emscripten_path = install_emscripten('1.35.0')

    # Make all paths absolute
    cmake_path = os.path.abspath(cmake_path)
    android_path = os.path.abspath(android_path)
    emscripten_path = os.path.abspath(emscripten_path)

    # First setup environment variables
    paths = EnvironmentPaths(os.getcwd(), cmake_path, android_path, emscripten_path)
    env = configure_environment(args, paths)

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print 'Error: %s' % e