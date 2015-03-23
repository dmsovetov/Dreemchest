items = ['sound', 'threads', 'platform', 'renderer', 'io', 'network']
Tests = Executable( 'Tests', sources = [ '.' ], paths = [ '../dreemchest', '../modules' ] )

Tests.linkExternal( Library( 'gtest', True ), Library( 'jsoncpp', True ) )

for item in items:
    if Has( item ):
		Tests.link( 'd' + item )
		Tests.dirs( item )