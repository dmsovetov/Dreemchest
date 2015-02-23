examples = Executable( 'Examples', paths = [ '../dreemchest' ], link = [ 'Dreemchest' ] )

if Has( 'platform' ):
	examples.dirs( '01_Platform/*' )

	if platform == 'iOS':
		examples.linkExternal( Library( 'Foundation', True ), Library( 'UIKit', True ) )
	elif platform == 'MacOS':
		examples.linkExternal( Library( 'Cocoa', True ), Library( 'QuartzCore', True ) )

if Has( 'renderer' ):
	examples.dirs( '02_Renderer/*' )