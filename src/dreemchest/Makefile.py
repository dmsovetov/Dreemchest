core = StaticLibrary( 'Dreemchest', paths = [ '.', '../modules' ], sources = [ '.' ], defines = [ 'DC_BUILD_LIBRARY' ] )

Module( url = 'https://github.com/dmsovetov/utils.git', folder = '../modules' )
Module( url = 'https://github.com/dmsovetov/math.git',  folder = '../modules' )

#############################   Threads     #############################

if Has( 'threads' ):
	core.dirs( 'threads', 'threads/Task', 'threads/$(THREADS)' )
	
#############################     IO        #############################

if Has( 'io' ):
	core.dirs( 'io/*' )

	jsoncpp = Library( 'jsoncpp', False )
	if jsoncpp: core.linkExternal( jsoncpp )
	
############################     Event        ###########################

if Has( 'event' ):
	core.dirs( 'event/*' )

#############################   Platform    #############################

if Has( 'platform' ):
	core.dirs( 'platform', 'platform/$(PLATFORM)' )

	if MacOS:
		core.linkExternal( Library( 'Cocoa', True ) )

#############################   Renderer    #############################

if Has( 'renderer' ):
	core.dirs( 'renderer' )

	if core.linkExternal( Library( Get( 'renderer' ), True ) ):
		core.dirs( 'renderer/$(RENDERER)', 'renderer/$(RENDERER)/$(PLATFORM)' )

		if iOS:
			core.linkExternal( Library( 'QuartzCore', True ) )

#############################   Sound engine    #############################

if Has( 'sound' ):
	core.dirs( 'sound', 'sound/Drivers' )
	core.files( 'sound/Decoders/SoundDecoder.cpp', 'sound/Decoders/SoundDecoder.h', 'sound/Decoders/WavSoundDecoder.cpp', 'sound/Decoders/WavSoundDecoder.h' )

	if core.linkExternal( Library( Get( 'sound' ), True ) ):
		core.dirs( 'sound/Drivers/$(SOUND)' )

	if core.linkExternal( Library( 'vorbis' ) ):
		core.files( 'sound/Decoders/OggSoundDecoder.cpp', 'sound/Decoders/OggSoundDecoder.h' )

	if core.linkExternal( Library( 'mad' ) ):
		core.files( 'sound/Decoders/Mp3SoundDecoder.cpp', 'sound/Decoders/Mp3SoundDecoder.h' )