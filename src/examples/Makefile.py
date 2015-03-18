examples = Executable( 'Examples', paths = [ '../dreemchest', '../modules' ], link = [ 'Dreemchest' ] )

if Has( 'platform' ):
	examples.dirs( '01_Platform/*' )
	examples.dirs( '03_IO/*' )

	if iOS:
		examples.linkExternal( Library( 'Foundation', True ), Library( 'UIKit', True ) )
	elif MacOS:
		examples.linkExternal( Library( 'Cocoa', True ), Library( 'QuartzCore', True ) )

if Has( 'renderer' ):
	examples.dirs( '02_Renderer/*' )