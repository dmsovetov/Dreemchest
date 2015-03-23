SOURCE_FILES = [
  'serialization/BinaryStorage.cpp'
, 'serialization/BinaryStorage.h'
, 'serialization/Serializable.cpp'
, 'serialization/Serializable.h'
, 'serialization/Storage.cpp'
, 'serialization/Storage.h'
, 'serialization/TextStorage.cpp'
, 'serialization/TextStorage.h'
]

jsoncpp = Library( 'jsoncpp', False )

if jsoncpp:
	SOURCE_FILES = SOURCE_FILES + [
	  'serialization/JsonStorage.cpp'
	, 'serialization/JsonStorage.h'
	]
	
zlib = Library( 'zlib', False )
if zlib:
	SOURCE_FILES = SOURCE_FILES + [
	  'processors/ZlibBufferCompressor.cpp'
	, 'processors/ZlibBufferCompressor.h'
	]

io = StaticLibrary( 'dIO', sources = [ '.', 'streams' ] + SOURCE_FILES, defines = [ 'DC_BUILD_LIBRARY' ] )

if jsoncpp: io.linkExternal( jsoncpp )
if zlib:	io.linkExternal( zlib )