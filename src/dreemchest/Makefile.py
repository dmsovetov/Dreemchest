Project( externals = '../../externals', paths = [ '.', '../../externals/src' ], define = [ 'DC_BUILD_ENGINE' ] )

Module( url = 'https://github.com/dmsovetov/foo.git', folder = '../../externals/src' )

for folder in Folders( '*' ):
	if Has( folder.name ):
		Include( folder.name )