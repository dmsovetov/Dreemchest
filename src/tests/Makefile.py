Tests = Executable( 'Tests', sources = [ '.' ], paths = [ '../dreemchest' ] )
Tests.link( 'Dreemchest' )
Tests.linkExternal( Library( 'gtest', True ), Library( 'jsoncpp', True ) )