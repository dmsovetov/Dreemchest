examples = Executable( 'Examples', paths = [ '../dreemchest' ], link = [ 'Dreemchest' ] )

if Has( 'platform' ): examples.dirs( '01_Platform/*' )
if Has( 'renderer' ): examples.dirs( '02_Renderer/*' )