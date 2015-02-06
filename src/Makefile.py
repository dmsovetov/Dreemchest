core = StaticLibrary( 'Core', include = ['../dependency', '.'], sources = [ '.', 'utils/*' ], defines = [ 'DC_BUILD_LIBRARY' ] )
if Has( 'threads' ):  core.dirs( 'threads', 'threads/Task', 'threads/$(THREADS)' )
if Has( 'sound' ):    core.dirs( 'sound', 'sound/Drivers', 'sound/Decoders', 'sound/Drivers/$(SOUND)' )
if Has( 'renderer' ): core.dirs( 'renderer', 'renderer/$(RENDERER)', 'renderer/$(RENDERER)/$(PLATFORM)' )
if Has( 'platform' ): core.dirs( 'platform', 'platform/$(PLATFORM)' )

Include( '../dependency', 'tests' )