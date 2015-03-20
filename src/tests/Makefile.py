items = ['sound', 'threads', 'platform', 'renderer', 'io', 'network']
Tests = Executable( 'Tests', sources = [ '.' ], link = [ 'Dreemchest' ], paths = [ '../dreemchest', '../modules' ] )

Tests.linkExternal( Library( 'gtest', True ), Library( 'jsoncpp', True ) )

for item in items:
    if Has( item ): Tests.dirs( item )