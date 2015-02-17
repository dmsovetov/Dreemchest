examples = Executable( 'Examples', paths = [ '../dreemchest' ], link = [ 'Dreemchest' ] )

if Has( 'platform' ): examples.dirs( 'platform/*' )