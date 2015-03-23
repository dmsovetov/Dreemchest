dRenderer = StaticLibrary( 'dRenderer', sources = [ '.', '$(RENDERER)', '$(RENDERER)/$(PLATFORM)' ], defines = [ 'DC_BUILD_LIBRARY' ] )

Library( Get( 'renderer' ), True )

if iOS:
	dRenderer.linkExternal( Library( 'QuartzCore', True ) )