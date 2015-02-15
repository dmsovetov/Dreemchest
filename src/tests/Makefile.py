items = ['sound', 'threads', 'platform']
Tests = Executable( 'Tests', sources = [ '.' ], link = [ 'Core', 'Dependency' ], paths = [ '../' ] )
for item in items:
    if Has( item ): Tests.dirs( item )
Tests.frameworks( 'Cocoa', 'OpenGL', 'QuartzCore', 'OpenAL' )