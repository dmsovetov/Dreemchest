dScene = StaticLibrary( 'dScene', sources = [ '.', 'Assets', 'Components', 'Systems' ], defines = [ 'DC_BUILD_LIBRARY' ] )

if Has( 'physics2d' ):
	dScene.linkExternal( Library( Get( 'physics2d' ), True ) )