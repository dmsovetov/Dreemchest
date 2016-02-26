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

#include "RenderingContext.h"

#include "Rvm.h"
#include "ShaderCache.h"

#include "../Assets/Mesh.h"
#include "../Assets/Image.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// Open Renderer namespace here
using namespace Renderer;

// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext( RvmPtr rvm, ShaderCachePtr shaders, HalPtr hal, Renderer2DPtr renderer )
	: m_rvm( rvm ), m_shaders( shaders ), m_hal( hal ), m_renderer( renderer )
{
	m_rvm->setRenderingContext( this );
}

// ** RenderingContext::create
RenderingContextPtr RenderingContext::create( const HalPtr& hal )
{
	// Create rendering context.
	Renderer2DPtr  renderer2d = Renderer2D::create( hal, 1024 );
	RvmPtr		   rvm( DC_NEW Rvm( hal, 8000 ) );
	ShaderCachePtr shaders( DC_NEW ShaderCache( "D:\\BFG9000\\externals\\src\\dreemchest\\src\\dreemchest\\scene\\Rendering\\Shaders\\", hal ) );

	return RenderingContextPtr( DC_NEW RenderingContext( rvm, shaders, hal, renderer2d ) );
}

// ** RenderingContext::renderable
const RenderingContext::Renderable& RenderingContext::renderable( u32 index ) const
{
	DC_ABORT_IF( index == 0, "invalid renderable index" );
	return m_renderables[index - 1];
}

// ** RenderingContext::texture
const Renderer::TexturePtr& RenderingContext::texture( u32 index ) const
{
	DC_ABORT_IF( index == 0, "invalid texture index" );
	return m_textures[index - 1];
}

// ** RenderingContext::rvm
RvmPtr RenderingContext::rvm( void ) const
{
	return m_rvm;
}

// ** RenderingContext::shaders
ShaderCachePtr RenderingContext::shaders( void ) const
{
	return m_shaders;
}

// ** RenderingContext::hal
HalPtr RenderingContext::hal( void ) const
{
	return m_hal;
}

// ** RenderingContext::renderer
Renderer2DPtr RenderingContext::renderer( void ) const
{
	return m_renderer;
}

// ** RenderingContext::uploadRenderable
const RenderingAssetId& RenderingContext::uploadRenderable( MeshWPtr mesh, s32 chunk )
{
	DC_ABORT_IF( !mesh.valid(), "invalid mesh" );

	// Get chunk mesh buffers
	const Mesh::VertexBuffer& vertices = mesh->vertexBuffer( chunk );
	const Mesh::IndexBuffer&  indices  = mesh->indexBuffer( chunk );

	u32 vertexCount = ( u32 )vertices.size();
	u32 indexCount = ( u32 )indices.size();

	// Create GPU buffers.
	VertexDeclarationPtr vertexFormat = m_hal->createVertexDeclaration( "P3:N:T0:T1" );
	VertexBufferPtr	     vertexBuffer = m_hal->createVertexBuffer( vertexFormat, vertexCount );
	IndexBufferPtr	     indexBuffer  = m_hal->createIndexBuffer( indexCount );

	// Upload the vertex data
	Mesh::Vertex* vertex = vertexBuffer->lock<Mesh::Vertex>();

	for( u32 i = 0; i < vertexCount; i++ ) {
		vertex->position = vertices[i].position;
		vertex->normal = vertices[i].normal;

		for( u32 j = 0; j < Mesh::Vertex::MaxTexCoords; j++ ) {
			vertex->uv[j] = vertices[i].uv[j];
		}
		
		vertex++;	
	}

	vertexBuffer->unlock();

	// Upload the index data
	u16* index = indexBuffer->lock();
	memcpy( index, &indices[0], indices.size() * sizeof( u16 ) );
	indexBuffer->unlock();

	// Create the renderable.
	m_renderables.push_back( Renderable( PrimTriangles, vertexBuffer, indexBuffer ) );

	// Set the rendering asset id.
	mesh->setChunkId( chunk, m_renderables.size() );

	return mesh->chunkId( chunk );
}

// ** RenderingContext::uploadRenderable
const RenderingAssetId& RenderingContext::uploadTexture( ImageWPtr image )
{
	DC_ABORT_IF( !image.valid(), "invalid image" );

	Renderer::Texture2DPtr texture = m_hal->createTexture2D( image->width(), image->height(), image->bytesPerPixel() == 3 ? Renderer::PixelRgb8 : Renderer::PixelRgba8 );
	texture->setData( 0, &image->mipLevel( 0 )[0] );

	// Add texture
	m_textures.push_back( texture );

	// Set the rendering asset id.
	image->setId( m_textures.size() );

	return image->id();
}

} // namespace Scene

DC_END_DREEMCHEST