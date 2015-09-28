Project( externals = '../../externals', paths = [ '.', '../../externals/src' ], define = [ 'DC_BUILD_LIBRARY' ] )

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
	, 'Dreemchest.h'
]

SOUND = []

if Get( 'SOUND' ):
	SOUND = [
	  'Sound'
	, 'Sound/Decoders'
	, 'Sound/Drivers'
	, 'Sound/Drivers/$(SOUND)'
	]

StaticLibrary( 'Dreemchest', sources = SOURCES + SOUND )