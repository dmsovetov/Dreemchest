dNetwork = StaticLibrary( 'dNetwork', sources = [ '.', 'NetworkHandler/*', 'Sockets/*' ], defines = [ 'DC_BUILD_LIBRARY' ], link = [ 'dIO' ] )

if Windows:
	dNetwork.linkExternal( Library( 'Winsock2', True ) )