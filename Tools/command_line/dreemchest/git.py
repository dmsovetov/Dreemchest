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

import subprocess


def invoke(command):
    """Invokes a command as a subprocess"""

    try:
        subprocess.check_call(command, shell=True, stdout=None, stderr=subprocess.STDOUT)
        return True
    except subprocess.CalledProcessError:
        return False


def clone(url, directory, branch=None):
    """Clones a remote repository to a specified directory and that switches to a desired branch"""

    if branch:
        return invoke('git clone %s --branch %s --single-branch %s' % (url, branch, directory))
    else:
        return invoke('git clone %s --single-branch %s' % (url, directory))


def checkout_submodule(directory):
    """Initializes and pulls a submodule in a specified directory"""

    return invoke('git submodule update --init -- %s' % directory)


def checkout_submodules():
    """Initializes and pulls all submodules"""

    return invoke('git submodule update --init --recursive')
