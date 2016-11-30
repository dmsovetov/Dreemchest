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

# The Dreemchest engine command line tool.
class Dreemchest:
    # Constructs a command line tool.
    def __init__(self):
        # Parse arguments
        parser = argparse.ArgumentParser(description='Dreemchest configuration tool', usage='''dreemchest <command> [<args>]
        The most commonly used commands are:
           pull         Download source code from a remote repository.
           configure    Configures a local working copy.
        ''', formatter_class=argparse.ArgumentDefaultsHelpFormatter)

        parser.add_argument('command', help='Subcommand to run')

        # parse_args defaults to [1:] for args, but you need to
        # exclude the rest of the args too, or validation will fail
        args = parser.parse_args(sys.argv[1:2])

        # Ensure that command exists
        self._assert_command_exists(parser, args.command)

        # 0e dispatch pattern to invoke method with same name
        getattr(self, args.command)()

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

# Entry point
if __name__ == "__main__":
    Dreemchest()

   # subparsers = parser.add_subparsers(help='sub-command help')

   # pull = subparsers.add_parser('pull', help='pulls a specified version from a remote repository.')
    #pull.add_argument('action', help='lists all available versions.')
    '''
    parser.add_argument( "-a",  "--action",      type = str,  default  = 'build',                            help = "Build action.", choices = ["clean", "build", "install", "import"] )
    parser.add_argument( "-s",  "--source",      type = str,  required = True,                               help = "Input resource path." )
    parser.add_argument( "-o",  "--output",      type = str,  required = True,                               help = "Output path." )
 #   parser.add_argument( "-tc", "--compression", type = str,  default  = TextureCompression.Disabled,        help = "Hardware texture compression." )
 #   parser.add_argument( "-tf", "--texFormat",   type = str,  default  = TextureFormat.Raw,                  help = "Exported image format." )
 #   parser.add_argument( "-p",  "--platform",    type = str,  default  = TargetPlatform.Win,                 help = "Target platform.", choices = TargetPlatform.Available )
    parser.add_argument( "-v",  "--version",     type = str,  default  = '1.0',                              help = "Resource version" )
    parser.add_argument( "-w",  "--workers",     type = int,  default  = 8,                                  help = "The number of concurrent workers." )
#    parser.add_argument( "-q",  "--quality",     type = str,  default  = TextureQuality.HD,                  help = "Texture quality.", choices = TextureQuality.Available )
    parser.add_argument( "-c",  "--cache",       type = str,  default  = '[source]/[platform]/cache',        help = "Cache file name." )
    parser.add_argument( "--strip-unused",       type = bool, default  = False,                              help = "The unused assets won't be imported." )
    parser.add_argument( "--use-uuids",          type = int,  default  = 1,                                  help = "The UUIDs will be used instead of file names." )
    parser.add_argument( "--skip-scenes",        type = int,  default  = 0,                                  help = "Scenes wont be imported." )
    '''
    args = parser.parse_args()