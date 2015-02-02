# Platform specific settings
if platform == 'MacOS':
    Makefile.set( 'MACOS_SDK', 'macosx10.10' )
    Makefile.set( 'THREADING', 'Posix' )
elif platform == 'iOS':
    Makefile.set( 'IOS_SDK', 'iphoneos8.0' )
    Makefile.set( 'THREADING', 'Posix' )

# Project
project = Makefile.getProject()

# Core
StaticLibrary( 'Core', project, sources = [ '.', 'lib/*', 'threads', 'threads/Task', 'threads/$(THREADING)' ], defines = [ 'DC_BUILD_LIBRARY', 'DC_PLATFORM_OSX', 'DC_THREADING_POSIX' ] )

project.target( 'tests' )