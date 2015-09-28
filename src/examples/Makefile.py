SOURCES = [
	  '01_HelloWorld'
	, '02_CreatingWindows'
	, '03_WindowEvents'
	, '04_RendererInitialization'
	, '05_Files'
	, '06_BasicNetworking'
	, '07_TCPSockets'
	, '08_NetworkHandlers'
]

for name in SOURCES:
	Exe = Executable( name, paths = [ '../dreemchest' ], sources = [ name + '.cpp' ] )
	Exe.link( 'Dreemchest' )