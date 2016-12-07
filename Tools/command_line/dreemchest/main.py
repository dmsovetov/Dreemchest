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
import install
import checkout
import configure
import command_line


class Main(command_line.Tool):
    """An entry point to a command line tool"""

    def __init__(self, parser):
        """Constructs a command line tool."""

        command_line.Tool.__init__(self, parser, 'available commands')

        self._add_command('configure', configure.Command)
        self._add_command('checkout', checkout.Command)
        self._add_command('install', install.Command)


def main():
    root = argparse.ArgumentParser(prog='dreemchest', description='Dreemchest engine command line tool')

    Main(root)

    args = root.parse_args()
    args.function(args)


if __name__ == "__main__":
    main()