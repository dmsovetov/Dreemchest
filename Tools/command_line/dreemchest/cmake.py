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
import sys
import subprocess
import shutil
import urllib2
import zipfile
import tarfile

GENERATE_COMMAND = '{cmake} -E chdir {output} {cmake} {source} -G "{generator}" {args} {rest}'


def get_cmake_executable():
    if sys.platform == 'darwin':
        return os.path.abspath('Tools/cmake/CMake.app/Contents/bin/cmake')
    else:
        return os.path.abspath('Tools/cmake/bin/cmake.exe')

try:
    with open(os.devnull, 'wb') as devnull:
        subprocess.check_call('cmake', shell=True, stdout=devnull, stderr=devnull)
        cmake = 'cmake'
except subprocess.CalledProcessError:
    cmake = get_cmake_executable()


def get_download_url():
    if sys.platform == 'darwin':
        return 'https://cmake.org/files/v3.6/cmake-3.6.3-Darwin-x86_64.tar.gz'
    else:
        return 'https://cmake.org/files/v3.6/cmake-3.6.3-win32-x86.zip'


def download():
    url = get_download_url()

    file_name = url.split('/')[-1]
    u = urllib2.urlopen(url)

    f = open(file_name, 'wb')
    meta = u.info()
    file_size = int(meta.getheaders("Content-Length")[0])
    print "Downloading: %s Bytes: %s" % (file_name, file_size)

    file_size_dl = 0
    block_sz = 8192
    while True:
        buffer = u.read(block_sz)
        if not buffer:
            break

        file_size_dl += len(buffer)
        f.write(buffer)
        status = "\r%10d  [%3.2f%%]" % (file_size_dl, file_size_dl * 100. / file_size)
        print status,
        sys.stdout.flush()

    f.close()

    name, ext = os.path.splitext(file_name)

    if ext == '.gz':
        tar = tarfile.open(file_name, "r")
        tar.extractall("Tools")
        name, ext = os.path.splitext(name)
    elif ext == '.zip':
        with zipfile.ZipFile(file_name, "r") as z:
            z.extractall("Tools")

    os.rename(os.path.join('Tools', name), 'Tools/cmake')

    os.remove(file_name)


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


def generate(generator, source, output, parameters, rest=''):
    """Invokes a CMake command to generate a build system"""

    # Create an output directory
    if not os.path.exists(output):
        os.makedirs(output)

    # Generate CMake parameter string
    cmd_args = ['-D%s=%s' % (key, parameters[key]) for key in parameters]

    # Generate a command line string
    command_line = GENERATE_COMMAND.format(
        output=output,
        source=os.path.abspath(source),
        generator=generator,
        args=' '.join(cmd_args),
        rest=rest,
        cmake=cmake
        )

    subprocess.check_call(command_line, shell=True)


def build(source, target, configuration):
    """Build a CMake-generated project binary tree."""

    # Generate a command line string
    command_line = '%s --build %s --target %s --config %s' % (cmake, source, target, configuration)

    subprocess.check_call(command_line, shell=True)


def configure_and_build(generator, source_dir, binary_dir,
                        target='ALL_BUILD',
                        configuration='Release',
                        prefix=None,
                        options=None):
    """Generates a binary tree and then builds a specified target for requested configuration"""

    if not options:
        options = dict()

    # Set the CMAKE_INSTALL_PREFIX variable
    if prefix:
        options['CMAKE_INSTALL_PREFIX:PATH'] = prefix

    try:
        # Generate a binary tree
        generate(generator, source_dir, binary_dir, options, '-Wno-dev')

        # Build a generated tree
        build(binary_dir, target, configuration)

    except subprocess.CalledProcessError:
        print 'Failed to build %s' % source_dir

    shutil.rmtree(binary_dir)


class Command:
    """A base class for all CMake commands"""

    def __init__(self, parser, source_dir='.', output_dir='.', prefix_path='.'):
        """Constructs a platform configuration command"""

        parser.add_argument('--source',
                            help='a source directory to generate build system.',
                            default=source_dir)
        parser.add_argument('--output',
                            help='an output directory to place generated build system.',
                            default=output_dir)
        parser.add_argument('--prefix-path',
                            help='a path used for searching packages.',
                            default=prefix_path)
        parser.add_argument('--cpp',
                            help='specifies the C++ standard whose features that are required by a build system.',
                            type=int,
                            choices=[98, 11, 14],
                            default=98)

        parser.set_defaults(function=self.configure)

    def configure(self, options):
        """Performs basic build system configuration"""
        pass
