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

#ifndef __DC_Composer_MeshImporter_H__
#define __DC_Composer_MeshImporter_H__

#include "AssetImporter.h"

#ifdef HAVE_FBX
	#include <fbxsdk.h>
#endif	/*	HAVE_FBX	*/

DC_BEGIN_COMPOSER

namespace Importers {

	//! Base class for all mesh importers.
	class MeshImporter : public AssetImporter {
	public:

		//! Writes imported mesh nodes to a file.
		virtual bool		import( IFileSystemWPtr fs, const Asset& asset, const io::Path& path ) DC_DECL_OVERRIDE;

	protected:

		//! Imports mesh nodes from a file.
		virtual bool		importNodes( IFileSystemWPtr fs, const Asset& asset, const io::Path& path ) = 0;

	protected:

		//! Mesh vertex format
		struct Vertex {
			enum { MaxTexCoords = 2 };
			Vec3			position;			//!< Vertex position.
			Vec3			normal;				//!< Vertex normal.
			Vec2			uv[MaxTexCoords];	//!< Vertex uv.
		};

		//! Vertex comparison functor.
		struct VertexCompare {
			//! Compares two vertices
			bool			operator()( const Vertex& a, const Vertex& b ) const;
		};

		//! Alias the mesh indexer type.
		typedef MeshIndexer<Vertex, VertexCompare> MeshNode;

		Array<MeshNode>		m_nodes;	//!< Imported mesh nodes.
	};

#ifdef HAVE_FBX

	//! Imports the FBX mesh.
	class MeshImporterFBX : public MeshImporter {
	public:

							MeshImporterFBX( void );
		virtual				~MeshImporterFBX( void ) DC_DECL_OVERRIDE;

	private:

		//! Imports mesh nodes from FBX file.
		virtual bool		importNodes( IFileSystemWPtr fs, const Asset& asset, const io::Path& path ) DC_DECL_OVERRIDE;

		//! Imports the FBX scene node.
		void				importNode( FbxNode* node );

		//! Imports the FBX scene node attributes.
		void				importAttributes( FbxNode* node );

		//! Imports the mesh from FBX scene node.
		void				importMesh( FbxNode* node, FbxMesh* mesh );

	private:

		static FbxManager*	s_manager;	//!< FBX manager instance.
		static FbxImporter*	s_importer;	//!< FBX scene importer instance.
		FbxScene*			m_scene;	//!< Imported FBX scene.
	};

#endif	/*	HAVE_FBX	*/

} // namespace Importers

DC_END_COMPOSER

#endif	/*	!__DC_Composer_MeshImporter_H__	*/