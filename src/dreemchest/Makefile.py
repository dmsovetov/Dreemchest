Project( externals = '../../externals', paths = [ '.', '../../externals/src' ], define = [ 'DC_BUILD_LIBRARY' ] )

#Module( url = 'https://github.com/dmsovetov/foo.git', folder = '../../externals/src' )

SOURCES = [
	  'Base/*'
	, 'Ecs/*'
#	, 'Event/*'
	, 'Fx/*'
	, 'Io'
	, 'Io/Serialization'
	, 'Io/Streams'
	, 'Network/*'
	, 'Platform'
	, 'Platform/$(PLATFORM)'
	, 'Renderer'
	, 'Renderer/$(RENDERER)'
	, 'Scene/*'
	, 'Threads'
	, 'Threads/Task'
	, 'Threads/$(PLATFORM)'
	, 'Sound'
	, 'Sound/Decoders'
	, 'Sound/Drivers'
	, 'Sound/Drivers/$(SOUND)'
	, 'Dreemchest.h'
]

StaticLibrary( 'Dreemchest', sources = SOURCES )

#Include( 'Ecs' )

#for folder in Folders( '*' ):
#	if Has( folder.name ):
#		Include( folder.name )