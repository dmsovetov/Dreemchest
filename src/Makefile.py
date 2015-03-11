Project( externals = '../externals' )

Include( 'dreemchest' )
Include( 'examples' )

if Library( 'gtest', False ):
	Include( 'tests' )