items = ['sound', 'threads', 'platform', 'renderer']
Tests = Executable( 'Tests', sources = [ '.' ], link = [ 'Dreemchest' ], paths = [ '../dreemchest' ] )

Tests.linkExternal( Library( 'gtest', True ) )

for item in items:
    if Has( item ): Tests.dirs( item )