# Platform specific settings
if platform == 'MacOS':
    Makefile.set( 'MACOS_SDK', 'macosx10.10' )
    Makefile.set( 'THREADING', 'Posix' )
    Makefile.set( 'SOUND', 'OpenAL' )
    Makefile.set( 'RENDERER', 'OpenGL' )
    Makefile.set( 'PLATFORM', 'MacOS' )
elif platform == 'iOS':
    Makefile.set( 'IOS_SDK', 'iphoneos8.0' )
    Makefile.set( 'THREADING', 'Posix' )
    Makefile.set( 'SOUND', 'OpenAL' )
    Makefile.set( 'RENDERER', 'OpenGL' )
    Makefile.set( 'PLATFORM', 'iOS' )

# Project
project = Makefile.getProject()

# Core
core = StaticLibrary( 'Core', project, include = ['../dependency'], sources = [ '.', 'lib/*', 'threads', 'threads/Task', 'threads/$(THREADING)' ], defines = [ 'DC_BUILD_LIBRARY', 'DC_PLATFORM_OSX', 'DC_THREADING_POSIX' ] )
core.dirs( 'sound', 'sound/Drivers', 'sound/Decoders', 'sound/Drivers/$(SOUND)' )
core.dirs( 'renderer', 'renderer/$(RENDERER)', 'renderer/$(RENDERER)/$(PLATFORM)' )
core.dirs( 'platform', 'platform/$(PLATFORM)' )

project.target( '../dependency' )
project.target( 'tests' )