examples = Executable( 'Examples', paths = [ '../dreemchest', '../modules' ] )
examples.link( 'Dreemchest' )
if Has( 'platform' ):
	examples.dirs( '01_Platform/*' )
	examples.dirs( '03_IO/*' )
#	examples.link( 'dPlatform' )

	if iOS:
		examples.linkExternal( Library( 'Foundation', True ), Library( 'UIKit', True ) )
	elif MacOS:
		examples.linkExternal( Library( 'Cocoa', True ), Library( 'QuartzCore', True ) )

if Has( 'renderer' ):
	examples.dirs( '02_Renderer/*' )
#	examples.link( 'dRenderer' )
	
if Has( 'network' ):
	examples.dirs( '04_Networking/*' )
#	examples.link( 'dNetwork', 'dThreads' )