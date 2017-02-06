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
import shutil
import subprocess
from collections import namedtuple


DREEMCHEST_HOME = 'DREEMCHEST_HOME'
DREEMCHEST_CMAKE = 'DREEMCHEST_CMAKE'
GENERATE_COMMAND = '{cmake}/cmake -E chdir {output} {cmake}/cmake {source} -G "{generator}" {args} {rest}'

EnvironmentPaths = namedtuple('EnvironmentPaths', ['home', 'cmake'])


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

    def set_variable(self, name, value):
        """Sets an environment variable"""

        raise Exception('an environment variable could not be set')

    def add_path(self, path):
        """Adds a specified path to an environment"""

        raise Exception('a path could not be added to an environment')

    def cmake_path(self):
        """Returns a CMake installation path"""
        return self._paths.cmake


class UnixEnvironment(Environment):
    """A base class for all Unix environments"""

    def __init__(self, bash_files):
        """Constructs the UnixEnvironment instance"""

        Environment.__init__(self)

        self._bash_files = [os.path.expanduser('~/' + bash_file) for bash_file in bash_files]
        self._bash_file = self.bash_file()
        #self.backup_file(self._bash_file)

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

    @staticmethod
    def backup_file(file_name):
        """Creates a backup file of a bash file to be modified"""

        # Nothing to backup
        if not os.path.exists(file_name):
            return

        backup_file_name = file_name + '.backup'

        if os.path.exists(backup_file_name):
            raise Exception('Dreemchest is already installed')

        shutil.copy(file_name, backup_file_name)

    def set_variable(self, name, value):
        """Writes an environment variable to bash configuration file"""

        self._append_to_bash('export {name}={value}\n'.format(name=name, value=value))
        print 'Environment variable {name} was set to {value}'.format(name=name, value=value)

    def add_path(self, path):
        """Writes path export to bash configuration file"""

        self._append_to_bash('export PATH={path}:$PATH\n'.format(path=path))
        print 'Path {path} added to environment'.format(path=path)

    def cmake_path(self):
        """Returns a CMake installation path"""

        paths = os.environ['PATH'].split(':')

        for path in paths:
            if os.path.exists(os.path.join(path, 'cmake')):
                return path

        return Environment.cmake_path(self)

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

        self.add_path('$' + DREEMCHEST_HOME)

        if paths.cmake:
            self.set_variable(DREEMCHEST_CMAKE, os.path.join('$' + DREEMCHEST_HOME, paths.cmake, 'CMake.app/Contents/bin'))
            self.add_path('$' + DREEMCHEST_CMAKE)


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

        self.add_path('%' + DREEMCHEST_HOME + '%')

        if paths.cmake:
            self.set_variable(DREEMCHEST_CMAKE, os.path.join('%' + DREEMCHEST_HOME + '%', paths.cmake, 'CMake.app/Contents/bin'))
            self.add_path('%' + DREEMCHEST_CMAKE + '%')

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
        with zipfile.ZipFile(file_name, "r") as z:
            z.extractall(path)

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
        subprocess.check_call(command, shell=True, stdout=None, stderr=subprocess.STDOUT)
        return True
    except subprocess.CalledProcessError:
        return False


def main():
    """A command line tool entry point"""

    parser = argparse.ArgumentParser(prog='setup', description='Dreemchest engine install tool')

    # CMake option
    cmake_options = parser.add_mutually_exclusive_group()
    cmake_options.add_argument(
        '--no-cmake', help='skips a CMake installation', action='store_true', default=False
    )
    cmake_options.add_argument(
        '--cmake', help='specifies a CMake version to be installed.', type=str, default='3.4.3'
    )

    args = parser.parse_args()

    # Download CMake
    cmake_path = None

    if not args.no_cmake:
        cmake_path = 'D:\\Projects\\github\\BFG9000\\src\\thirdParty\\Dreemchest\\tools\\cmake-3.4.3-win32-x86'
    #    cmake_path = download_cmake(args.cmake)
    #    print 'Installed CMake to ' + cmake_path

    # First setup environment variables
    paths = EnvironmentPaths(os.getcwd(), cmake_path)
    env = configure_environment(args, paths)

    cmake_install_path = env.cmake_path()

    if cmake_install_path:
        print 'CMake is located at', cmake_install_path
    else:
        raise Exception('CMake installation could not be found')

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print 'Error: %s' % e