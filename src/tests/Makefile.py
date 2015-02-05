items = ['sound', 'threads', 'platform']
Tests = Executable( 'Tests', sources = [ '.' ], libs = [ 'Core', 'Dependency' ], include = [ '../' ] )
for item in items:
    if Has( item ): Tests.dirs( item )
Tests.frameworks( 'Cocoa', 'OpenGL', 'QuartzCore', 'OpenAL' )