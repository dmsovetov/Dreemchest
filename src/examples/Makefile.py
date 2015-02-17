examples = Executable( 'Examples', paths = [ '../dreemchest' ], link = [ 'Dreemchest' ] )

if Has( 'platform' ): examples.dirs( 'platform/*' )
if Has( 'renderer' ): examples.dirs( 'renderer/*' )