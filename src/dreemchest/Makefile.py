Project( externals = '../../externals', paths = [ '.', '../../externals/src' ] )

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
	, 'Scene/*'
	, 'Threads'
	, 'Threads/Task'
	, 'Threads/$(PLATFORM)'
	, 'Dreemchest.h'
]

SOUND = []
RENDERER = []

if Get( 'sound' ):
	SOUND = [
	  'Sound'
	, 'Sound/Decoders'
	, 'Sound/Drivers'
	, 'Sound/Drivers/$(SOUND)'
	]
	
if Get( 'renderer' ):
	RENDERER = [
	  'Renderer'
	, 'Renderer/$(RENDERER)'
	, 'Renderer/$(RENDERER)/$(PLATFORM)'
	]

Dreemchest = StaticLibrary( 'Dreemchest', sources = SOURCES + SOUND + RENDERER, defines = [ 'DC_BUILD_LIBRARY' ] )

if Windows:
	Dreemchest.linkExternal( Library( 'Winsock2', True ) )

if Get( 'renderer' ):
	Dreemchest.linkExternal( Library( Get( 'renderer' ), True ) )

	if iOS:
		Dreemchest.linkExternal( Library( 'QuartzCore', True ) )
