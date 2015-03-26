/**************************************************************************
 
 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
**************************************************************************/

#ifndef __DC_Io_Io_H__
#define __DC_Io_Io_H__

#include "../Dreemchest.h"

#ifndef DC_UTILS_INCLUDED
	#include <utils/Utils.h>
#endif

DC_BEGIN_DREEMCHEST

namespace io {

    DECLARE_LOG( log )

    class Path;
    class Serializable;
    class Storage;
	class Serializable;

	class FileSystem;
		class DiskFileSystem;
		class Archive;

	class Stream;
		class FileStream;
		class ByteBuffer;
		class PackedStream;

    //! Available stream open modes.
    enum StreamMode {
        BinaryReadStream,     //!< Open binary stream for reading.
        BinaryWriteStream,    //!< Open binary stream for writing.
    };

    //! Available text data storages.
    enum StorageType {
        StorageBinary,
        StorageJson,
        StorageYaml,
        StorageXml,
    };

    //! Stream ptr type.
    typedef StrongPtr<Stream>       StreamPtr;

    //! File stream ptr type.
    typedef StrongPtr<FileStream>   FileStreamPtr;

    //! Byte buffer ptr type.
    typedef StrongPtr<ByteBuffer>   ByteBufferPtr;

    //! Archive ptr type.
    typedef StrongPtr<Archive>      ArchivePtr;

	//! Serializable strong ptr.
	typedef StrongPtr<Serializable>	SerializablePtr;

    typedef Map<Path, ArchivePtr>	Archives;
    typedef Set<Path>				PathSet;
	typedef List<SerializablePtr>	Serializables;

    namespace detail {
        typedef StrongPtr<class FieldSerializer>    FieldSerializerPtr;
        typedef List<FieldSerializerPtr>            FieldSerializers;
    }

} // namespace io

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "streams/FileStream.h"
    #include "streams/ByteBuffer.h"
	#include "serialization/BinarySerializer.h"
    #include "serialization/BinaryStorage.h"

    #ifdef HAVE_JSONCPP
   //     #include "serialization/JsonStorage.h"
    #endif

    #include "serialization/Serializable.h"
	#include "FileSystem.h"
	#include "Archive.h"
    #include "DiskFileSystem.h"
#endif

#endif /*   !defined( __DC_Io_Io_H__ )   */