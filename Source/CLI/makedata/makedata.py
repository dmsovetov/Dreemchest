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

import time
import os
import tasks
#import unity
import importers
import serialization
from assets import Assets
from assets import Cache
from importers import unity


# substitute_variables
def substitute_variables(args, *variables):
    argv = vars(args)
    has_changes = True

    while has_changes:
        has_changes = False

        for k, v in argv.items():
            if isinstance(v, str):
                for var in variables:
                    key = '[' + var + ']'

                    if argv[k].find(key) != -1:
                        argv[k] = argv[k].replace(key, argv[var])
                        has_changes = True


# class TextureQuality
class TextureQuality(object):
    HD = 'hd'
    SD = 'sd'
    Available = [HD, SD]


# class TargetPlatform
class TargetPlatform(object):
    Win = 'win'
    Mac = 'mac'
    iOS = 'ios'
    Android = 'android'
    Available = [Win, Mac, iOS, Android]


# class ExportError
class ExportError(Exception):
    def __init__(self, msg):
        Exception.__init__(self, msg)


# Imports the project
def import_project(args, source, output):
    # Parse project assets
    assets = unity.project.parse_assets(source, tasks.create(args.workers))
    
    # Import scenes
    #unity.project.import_scenes(assets, source, output)

    # Import prefabs
    #unity.project.import_prefabs(assets, source, output)

    # Import materials
    unity.project.import_materials(assets, source, output)

    # Import assets
    #unity.project.import_assets(assets, source, output)

    # Save the assets
    assets.save(output)


def build(args):
    """Builds the data to a specified folder"""

    substitute_variables(args, 'version', 'compression', 'platform', 'quality', 'source', 'output')

    # Check the input arguments
    if not os.path.exists(args.source):
        raise AssertionError('the input folder does not exist')

    # Create the output folder
    if not os.path.exists(args.output):
        os.makedirs(args.output)

    print('--- Building [{0}] data package to [{1}] with [{2}] image compression ---'.format(args.platform, args.output, args.compression))
    start = time.time()

    rules = []

    for image_format in importers.ImageFormats.AVAILABLE:
        rules.append(('*.panorama.%s' % image_format, importers.PanoramaImporter))
        rules.append(('*.%s' % image_format, importers.TextureImporter))

    for mesh_format in importers.MeshFormats.AVAILABLE:
        rules.append(('*.%s' % mesh_format, importers.MeshImporter))

    document_format = serialization.document_formats[args.format]

    try:
        queue = tasks.create(args.workers)
        asset_bundle = Assets(args.source,
                              args.output,
                              Cache(args.cache),
                              rules,
                              importers=importers,
                              document_format=document_format)
        asset_bundle.scan()

        print '%d files to build' % len(asset_bundle.outdated)
        asset_bundle.build(queue)

        queue.start()

        # Write the manifest file
        #with open(os.path.join(args.output, 'assets.json'), 'wt') as fh:
        #    fh.write(assets.generate_manifest(outdated))
        #    fh.close()
    except ExportError as e:
        print e.message

    print '--- %s seconds ---' % int(time.time() - start)


def command_line(parser):
    parser.add_argument("--source",
                        required=True,
                        help="input resource path.")

    parser.add_argument("--output",
                        required=True,
                        help="output path.")

    parser.add_argument('--format',
                        choices=serialization.document_formats.keys(),
                        default=serialization.document_formats.keys()[0],
                        help="document format to be used.")

    parser.add_argument("--cache",
                        default='[source]/[platform]/cache',
                        help="cache folder.")

    parser.add_argument("--platform",
                        default=TargetPlatform.Win,
                        help="target platform.",
                        choices=TargetPlatform.Available)

    parser.add_argument("--compression",
                        default=importers.TextureCompression.DISABLED,
                        choices=importers.TextureCompression.AVAILABLE,
                        help="hardware image compression.")

    parser.add_argument("--version",
                        default='1.0',
                        help="resource version")

    parser.add_argument("--workers",
                        type=int,
                        default=8,
                        help="The number of concurrent workers.")

    parser.add_argument("--quality",
                        default=TextureQuality.HD,
                        help="Texture quality.",
                        choices=TextureQuality.Available)

    parser.set_defaults(function=build)
