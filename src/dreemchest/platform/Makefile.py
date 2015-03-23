dPlatform = StaticLibrary( 'dPlatform', sources = [ '.', '$(PLATFORM)' ], defines = [ 'DC_BUILD_LIBRARY' ] )

if MacOS:
	dPlatform.linkExternal( Library( 'Cocoa', True ) )