items = ['sound', 'threads', 'platform', 'renderer']
Tests = Executable( 'Tests', sources = [ '.' ], link = [ 'Dreemchest' ], paths = [ '../' ] )

for item in items:
    if Has( item ): Tests.dirs( item )