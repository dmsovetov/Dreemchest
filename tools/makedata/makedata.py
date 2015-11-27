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

import argparse, time, files, os, actions, tasks, unity

# substitute_variables
def substitute_variables( args, *variables ):
	argv = vars( args )

	for k, v in argv.items():
		if isinstance( v, str ):
			for var in variables:
				argv[k] = argv[k].replace( '[' + var + ']', argv[var] )

# class TextureQuality
class TextureQuality:
    HD = 'hd'
    SD = 'sd'
    Available = [HD, SD]

# class TargetPlatform
class TargetPlatform:
    Win = 'win'
    Mac = 'mac'
    iOS = 'ios'
    Android = 'android'
    Available = [Win, Mac, iOS, Android]

# class TextureCompression
class TextureCompression:
    Disabled = 'disabled'
    Pvr = 'pvr'
    Dxt = 'dxt'
    Etc = 'etc'

# class TextureFormat
class TextureFormat:
    Raw = 'raw'
    Png = 'png'
    Tga = 'tga'

    @staticmethod
    def convert_to(format):
        if format == TextureFormat.Raw: return actions.convert_to_raw
        if format == TextureFormat.Png: return actions.png_quant
        if format == TextureFormat.Tga: return actions.compress

# class ExportError
class ExportError(Exception):
    def __init__(self, msg):
        Exception.__init__(self, msg)

# Imports the project
def import_project(args, source, output):
    # Parse project assets
    assets = unity.project.parse_assets(args)
    
    # Import scenes
    unity.project.import_scenes(assets, source, output)

    # Import prefabs
    unity.project.import_prefabs(assets, source, output)

    # Import materials
    unity.project.import_materials(assets, source, output)

    # Import assets
    unity.project.import_assets(assets, source, output)

    # Save the assets
    assets.save(output)

# Builds the data to a specified folder
def build(args, source, output):
    rules = {
            '*.tga': TextureFormat.convert_to(args.texFormat)
        ,   '*.png': TextureFormat.convert_to(args.texFormat)
        ,   '*.fbx': actions.convert_fbx
        }

    queue    = tasks.create(args.workers)
    outdated = files.find_outdated(source)

    files.build(queue, outdated, output, rules)

    queue.start()

    # Write the manifest file
    with open(os.path.join(output, 'assets.json'), 'wt') as fh:
        fh.write(files.generate_manifest(outdated))
        fh.close()

# Entry point
if __name__ == "__main__":
    # Parse arguments
    parser = argparse.ArgumentParser( description = 'Dreemchest make data tool', formatter_class = argparse.ArgumentDefaultsHelpFormatter )

    parser.add_argument( "-a",  "--action",      type = str,  default  = 'build',                            help = "Build action.", choices = ["clean", "build", "install", "import"] )
    parser.add_argument( "-s",  "--source",      type = str,  required = True,                               help = "Input resource path." )
    parser.add_argument( "-o",  "--output",      type = str,  required = True,                               help = "Output path." )
    parser.add_argument( "-tc", "--compression", type = str,  default  = TextureCompression.Disabled,        help = "Hardware texture compression." )
    parser.add_argument( "-tf", "--texFormat",   type = str,  default  = TextureFormat.Raw,                  help = "Exported image format." )
    parser.add_argument( "-p",  "--platform",    type = str,  default  = TargetPlatform.Win,                 help = "Target platform.", choices = TargetPlatform.Available )
    parser.add_argument( "-v",  "--version",     type = str,  default  = '1.0',                              help = "Resource version" )
    parser.add_argument( "-w",  "--workers",     type = int,  default  = 8,                                  help = "The number of concurrent workers." )
    parser.add_argument( "-q",  "--quality",     type = str,  default  = TextureQuality.HD,                  help = "Texture quality.", choices = TextureQuality.Available )
    parser.add_argument( "-c",  "--cache",       type = str,  default  = '[source]/[platform]/cache',        help = "Cache file name." )
    parser.add_argument( "--strip-unused",       type = bool, default  = False,                              help = "The unused assets won't be imported." )
    parser.add_argument( "--use-uuids",          type = int,  default  = 0,                                  help = "The UUIDs will be used instead of file names." )

    args = parser.parse_args()

    substitute_variables( args, 'version', 'compression', 'platform', 'quality', 'source' )

    # Check the input arguments
    if not os.path.exists(args.source):
        raise AssertionError('the input folder does not exist')

    # Create the output folder
    if not os.path.exists(args.output):
        os.makedirs(args.output)

    print '---', 'Building [', args.platform, '] data package to [', args.output, '] with [', args.compression, '] texture compression', '---'
    start = time.time()

    try:
        if args.action == 'build':
            build(args, args.source, args.output)
        elif args.action == 'import':
            import_project(args, args.source, args.output)
    except ExportError as e:
        print e.message

    print '---', int(time.time() - start), 'seconds'