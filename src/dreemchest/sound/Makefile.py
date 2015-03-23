dSound = StaticLibrary( 'dSound', sources = [ '.', 'Drivers' ], defines = [ 'DC_BUILD_LIBRARY' ] )

dSound.files( 'Decoders/SoundDecoder.cpp', 'Decoders/SoundDecoder.h', 'Decoders/WavSoundDecoder.cpp', 'Decoders/WavSoundDecoder.h' )

if dSound.linkExternal( Library( Get( 'sound' ), True ) ):
	dSound.dirs( 'Drivers/$(SOUND)' )

if dSound.linkExternal( Library( 'vorbis' ) ):
	dSound.files( 'Decoders/OggSoundDecoder.cpp', 'Decoders/OggSoundDecoder.h' )

if dSound.linkExternal( Library( 'mad' ) ):
	dSound.files( 'Decoders/Mp3SoundDecoder.cpp', 'Decoders/Mp3SoundDecoder.h' )