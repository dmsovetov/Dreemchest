dThreads = StaticLibrary( 'dThreads', sources = [ '.', 'Task', '$(THREADS)' ], defines = [ 'DC_BUILD_LIBRARY' ] )

if Linux:
	dThreads.linkExternal( Library( 'pthread', True ) )