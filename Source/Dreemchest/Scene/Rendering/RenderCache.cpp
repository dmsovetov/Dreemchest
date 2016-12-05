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

#include "RenderCache.h"
#include "Rvm/RenderingContext.h"
#include "../Assets/Mesh.h"
#include "../Assets/Material.h"
#include "../Assets/Image.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** TestRenderCache::TestRenderCache
TestRenderCache::TestRenderCache( Assets::AssetsWPtr assets, RenderingContextWPtr context )
    : m_assets( assets )
    , m_context( context )
{
}

// ** TestRenderCache::create
RenderCachePtr TestRenderCache::create( Assets::AssetsWPtr assets, RenderingContextWPtr context )
{
    return DC_NEW TestRenderCache( assets, context );
}

// ** TestRenderCache::requestInputLayout
RenderResource TestRenderCache::requestInputLayout( const VertexFormat& format )
{
    InputLayouts::iterator i = m_inputLayouts.find( format );

    if( i != m_inputLayouts.end() ) {
        return i->second;
    }

    RenderResource id = m_context->requestInputLayout( format );
    m_inputLayouts[format] = id;
    return id;
}

// ** TestRenderCache::requestVertexBuffer
RenderResource TestRenderCache::requestVertexBuffer( const MeshHandle& mesh )
{
    RenderResources::iterator i = m_vertexBuffers.find( mesh.asset().uniqueId() );

    if( i != m_vertexBuffers.end() ) {
        return i->second;
    }

    NIMBLE_BREAK_IF( mesh->chunkCount() == 0, "could not cache an empty mesh" );

    const Mesh::VertexBuffer& vertices = mesh->vertexBuffer();
    VertexFormat vertexFormat( VertexFormat::Normal | VertexFormat::Uv0 | VertexFormat::Uv1 );

    RenderResource id = m_context->requestVertexBuffer( &vertices[0], vertices.size() * vertexFormat.vertexSize() );
    m_vertexBuffers[mesh.asset().uniqueId()] = id;

    LogVerbose( "renderCache", "vertex buffer with %d vertices created\n", vertices.size() );

    return id;
}

// ** TestRenderCache::requestIndexBuffer
RenderResource TestRenderCache::requestIndexBuffer( const MeshHandle& mesh )
{
    RenderResources::iterator i = m_indexBuffers.find( mesh.asset().uniqueId() );

    if( i != m_indexBuffers.end() ) {
        return i->second;
    }

    NIMBLE_BREAK_IF( mesh->chunkCount() == 0, "could not cache an empty mesh" );

    const Mesh::IndexBuffer& indices = mesh->indexBuffer();

    RenderResource id = m_context->requestIndexBuffer( &indices[0], indices.size() * sizeof( u16 ) );
    m_indexBuffers[mesh.asset().uniqueId()] = id;

    LogVerbose( "renderCache", "index buffer with %d indices created\n", indices.size() );

    return id;
}

// ** TestRenderCache::requestMesh
const TestRenderCache::RenderableNode* TestRenderCache::requestMesh( const MeshHandle& asset )
{
    // Return a NULL pointer for invalid meshes
    if( !asset.isValid() ) {
        return NULL;
    }

    NIMBLE_ABORT_IF( !asset.isLoaded(), "a mesh asset was not loaded" );

    // Get an asset unique id
    const Assets::AssetId& assetId = asset.asset().uniqueId();

    // First lookup a cached material
    RenderableNodeCache::iterator i = m_renderable.find( assetId );
    
    if( i != m_renderable.end() ) {
        return i->second.get();
    }

    // Create a new render node
    RenderableNode* node = DC_NEW RenderableNode;
    node->offset = 0;
    node->count  = asset->indexBuffer().size();
    node->states.bindVertexBuffer( requestVertexBuffer( asset ) );
    node->states.bindIndexBuffer( requestIndexBuffer( asset ) );
    node->states.bindInputLayout( requestInputLayout( asset->vertexFormat() ) );

    // Associate this material node with an asset identifier
    m_renderable[assetId] = node;

    LogVerbose( "renderCache", "renderable created from '%s'\n", asset.asset().name().c_str() );

    return node;
}

// ** TestRenderCache::createRenderable
const TestRenderCache::RenderableNode* TestRenderCache::createRenderable( const void* vertices, s32 count, const VertexFormat& vertexFormat )
{
    RenderResource vertexBuffer = m_context->requestVertexBuffer( vertices, count * vertexFormat.vertexSize() );

    RenderableNode* node = DC_NEW RenderableNode;
    node->offset = 0;
    node->count  = count;
    node->states.bindVertexBuffer( vertexBuffer );
    node->states.bindInputLayout( requestInputLayout( vertexFormat ) );

    return node;
}

// ** TestRenderCache::requestMaterial
const TestRenderCache::MaterialNode* TestRenderCache::requestMaterial( const MaterialHandle& asset )
{
    // Return a NULL pointer for invalid materials
    if( !asset.isValid() ) {
        return NULL;
    }

    NIMBLE_ABORT_IF( !asset.isLoaded(), "a material asset was not loaded" );

    // Get an asset unique id
    const Assets::AssetId& assetId = asset.asset().uniqueId();

    // First lookup a cached material
    MaterialNodeCache::iterator i = m_materials.find( assetId );
    
    if( i != m_materials.end() ) {
        return i->second.get();
    }

    // Create a new material node
    MaterialNode* node = DC_NEW MaterialNode;
    node->data.diffuse      = asset->color( Material::Diffuse );
    node->data.specular     = asset->color( Material::Specular );
    node->data.emission     = asset->color( Material::Emission );
    node->data.rim.color    = Rgb( 1.0f, 0.0f, 0.0f );
    node->data.rim.start    = 0.25f;
    node->data.rim.end      = 1.0f;
    node->data.rim.factor   = 0.5f;
    node->constantBuffer    = m_context->requestConstantBuffer( &node->data, sizeof( RenderScene::CBuffer::Material ), RenderScene::CBuffer::Material::Layout );

    // Now setup a material state block
    node->states.bindConstantBuffer( node->constantBuffer, RenderState::MaterialConstants );
    for( s32 i = 0; i < Material::TotalMaterialLayers; i++ ) {
        RenderResource id = requestTexture( asset->texture( static_cast<Material::Layer>( i ) ) );
        if( id ) {
            node->states.bindTexture( id, static_cast<RenderState::TextureSampler>( RenderState::Texture0 + i ) );
        }
    }

    // Associate this material node with an asset identifier
    m_materials[assetId] = node;

    LogVerbose( "renderCache", "material '%s' created\n", asset.asset().name().c_str() );

    return node;
}

// ** TestRenderCache::requestTexture
RenderResource TestRenderCache::requestTexture( const ImageHandle& image )
{
    if( !image.isValid() ) {
        return 0;
    }

    NIMBLE_ABORT_IF( !image.isLoaded(), "an image asset was not loaded" );

    RenderResources::iterator i = m_textures.find( image.asset().uniqueId() );

    if( i != m_textures.end() ) {
        return i->second;
    }

    const Image& data = *image;
    RenderResource id = m_context->requestTexture( &data.mipLevel( 0 )[0], data.width(), data.height(), data.bytesPerPixel() == 3 ? Renderer::PixelRgb8 : Renderer::PixelRgba8 );
    m_textures[image.asset().uniqueId()] = id;

    LogVerbose( "renderCache", "texture created for '%s'\n", image.asset().name().c_str() );

    return id;
}

} // namespace Scene

DC_END_DREEMCHEST
