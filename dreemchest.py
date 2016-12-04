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
import sys
import os

class CommandLineTool:
    """A base class for all command line tools"""

    def __init__(self, description):
        """Constructs a command line tool"""

        # Create a parser instance
        self._parser = argparse.ArgumentParser(description=description)

    def _ensure_command_exists(self, name, message):
        """Ensures that a command is supported by this command line tool"""

        if not hasattr(self, name):
            print message
            self._parser.print_help()
            exit(1)

    def _dispatch(self, command, args, message):
        """Dispatches an execution to a selected command"""

        self._ensure_command_exists(command, message)

        # Use dispatch pattern to invoke method with same name
        getattr(self, command)(args)


class Configure(CommandLineTool):
    """A command line tool to configure a working copy"""

    def __init__(self):
        """Constructs a configuration command line tool"""

        CommandLineTool.__init__(self, 'Generates a build system for a specified target platform')

        self._parser.add_argument('platform', help='A target platform', choices=['windows'])
        self._parser.add_argument('--source', help='A source directory to generate build system.', default='.')
        self._parser.add_argument('--output', help='An output directory to place generated build system.', default='.')

        args = self._parser.parse_args(sys.argv[2:])

        self._dispatch(args.platform, args, 'unrecognized platform')

    @staticmethod
    def windows(args):
        """Generates a build system for Windows platform"""

        # Create an output directory
        if not os.path.exists(args.output):
            os.makedirs(args.output)

        # Invoke a CMake command to generate a build system
        os.system('cmake -E chdir %s cmake %s -G "Visual Studio 12" -DDC_BUILD_TESTS=OFF -DDC_WITH_RELIGHT=OFF -DDC_QT_SUPPORT=OFF -DDC_COMPOSER_ENABLED=OFF -DDC_USE_PCH=ON' % (args.output, os.path.abspath(args.source)))

        print args.source, args.output


class Main(CommandLineTool):
    """An entry point to a command line tool"""

    def __init__(self):
        """Constructs a command line tool."""

        CommandLineTool.__init__(self, 'Dreemchest engine command line tool')

        self._parser.add_argument('command', help='A command to run.', choices=['configure', 'pull'])

        # exclude the rest of the args too, or validation will fail
        args = self._parser.parse_args(sys.argv[1:2])

        # Invoke a selected command
        self._dispatch(args.command, args, 'unrecognized command')

    @staticmethod
    def configure(args):
        """Runs a working copy configuration"""

        Configure()

    '''
    # Pulls a source code from a remote repository
    def pull(self):
        parser = argparse.ArgumentParser(
            description='Record changes to the repository')
        # prefixing the argument with -- means it's optional
        parser.add_argument('--list', help='lists all available versions')
        # now that we're inside a subcommand, ignore the first
        # TWO argvs, ie the command (git) and the subcommand (commit)
        args = parser.parse_args(sys.argv[2:])
        print 'Running git commit, amend=%s' % args.amend

    def configure(self):
        parser = argparse.ArgumentParser(
            description='Download objects and refs from another repository')
        # NOT prefixing the argument with -- means it's not optional
        parser.add_argument('repository')
        args = parser.parse_args(sys.argv[2:])
        print 'Running git fetch, repository=%s' % args.repository

    # Ensures that a requested command can be invoked from a command line tool
    def _assert_command_exists(self, parser, name):
        if not hasattr(self, name):
            print 'Unrecognized command'
            parser.print_help()
            exit(1)
    '''

    USAGE = '''
    Usage: dreemchest [command] [arguments...]
    Available commands:
        configure   -   runs a working copy configuration.
    '''

# Entry point
if __name__ == "__main__":
    Main()