Tests = Executable( 'Tests', sources = [ '.' ], libs = [ 'Dreemchest' ], paths = [ '../dreemchest' ] )
Tests.linkExternal( Library( 'gtest', True ), Library( 'jsoncpp', True ) )