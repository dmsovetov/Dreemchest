Project( paths = [ '.', '../modules' ] )

Module( url = 'https://github.com/dmsovetov/utils.git', folder = '../modules' )
Module( url = 'https://github.com/dmsovetov/math.git',  folder = '../modules' )

for folder in Folders( '*' ):
	if Has( folder.name ):
		Include( folder.name )