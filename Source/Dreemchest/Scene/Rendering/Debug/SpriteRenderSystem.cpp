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

#include "SpriteRenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// -------------------------------------------------------------- SpriteRenderSystem -------------------------------------------------------------- //

// ** SpriteRenderSystem::SpriteRenderSystem
SpriteRenderSystem::SpriteRenderSystem( RenderingContext& context, RenderScene& renderScene )
    : RenderSystem( context, renderScene )
    , m_vertexFormat( VertexFormat::Position | VertexFormat::Color | VertexFormat::TexCoord0 )
{
    // Allocate and initialize an index buffer
    m_indices = allocateTrianleIndexBuffer( MaxSpritesInBatch * 2 );

    // Request an input layout for a 2D vertex format
    m_inputLayout  = context.requestInputLayout( m_vertexFormat );

    // Request a vertex buffer used for rendering
    m_vertexBuffer = context.requestVertexBuffer( NULL, MaxSpritesInBatch * m_vertexFormat.vertexSize() );

    // Request an index buffer and upload data
    m_indexBuffer = context.requestIndexBuffer( m_indices, MaxSpritesInBatch * 6 * sizeof( u16 ) );

    // Create a sprite shader
    m_spriteShader = m_context.deprecatedRequestShader( "../../Source/Dreemchest/Scene/Rendering/Shaders/Default.shader" );
}

// ** SpriteRenderSystem::emitRenderOperations
void SpriteRenderSystem::emitRenderOperations( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const SpriteRenderer& spriteRenderer )
{
    // Push a shader sprite rendering state
    StateScope state = stateStack.newScope();
    state->bindVertexBuffer( m_vertexBuffer );
    state->bindIndexBuffer( m_indexBuffer );
    state->bindInputLayout( m_inputLayout );
    state->bindProgram( m_spriteShader );

    // Get all sprites that reside in scene
    const RenderScene::Sprites& sprites = m_renderScene.sprites();

    // Process each sprite and submit them in batches for rendering
    const StateBlock* activeStates = NULL;
    s32                        spriteCount  = 0;

    for( s32 i = 0, n = sprites.count(); i < n; i++, spriteCount++ ) {
        // Get a sprite by index
        const RenderScene::SpriteNode& sprite = sprites[i];

        // Go to a next sprite if this one has a same material state block as a previous one
        if( sprite.material.states == activeStates && spriteCount < MaxSpritesInBatch ) {
            continue;
        }

        // Render states for this sprite differ from a previous one - submit a batch
        emitSpriteBatch( frame, commands, stateStack, sprites, i - spriteCount, spriteCount, spriteRenderer.scaleFactor() );
            
        // Save this state block and reset a sprite count
        activeStates = sprite.material.states;
        spriteCount  = 0;
    }
    
    // Flush a last batch
    emitSpriteBatch( frame, commands, stateStack, sprites, sprites.count() - spriteCount, spriteCount, spriteRenderer.scaleFactor() );
}

// ** SpriteRenderSystem::emitSpriteBatch
void SpriteRenderSystem::emitSpriteBatch( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const RenderScene::Sprites& sprites, s32 first, s32 count, f32 scaleFactor )
{
    // Nothing to render - just skip
    if( count == 0 ) {
        return;
    }

    // Begin batch by allocating a chunk of vertex data
    void* vertices = frame.allocate( m_vertexFormat.vertexSize() * count * 2 );

    // A total number of emitted vertices
    s32 vertexCount = 0;

    // Process each sprite and write them to a dynamic mesh vertex buffer
    for( s32 i = first; i < first + count; i++ ) {
        // Get a sprite by index
        const RenderScene::SpriteNode& sprite = sprites[i];

        // Write the sprite inside into a vertex buffer
        emitSpriteVertices( vertices, vertexCount, *sprite.matrix, sprite.sprite->width(), sprite.sprite->height(), sprite.sprite->color(), scaleFactor );

        // All sprites contain 4 vertices, so increase a vertex count by this amount
        vertexCount += 4;
    }

    // Push a material state for this batch
    StateScope state = stateStack.push( sprites[first].material.states );

    // Upload vertex data to a GPU buffer and emit a draw indexed command
    commands.uploadVertexBuffer( m_vertexBuffer, vertices, vertexCount * m_vertexFormat.vertexSize() );
    commands.drawIndexed( 0, Renderer::PrimTriangles, 0, vertexCount * 3, stateStack );
}

// ** SpriteRenderSystem::emitSpriteVertices
void SpriteRenderSystem::emitSpriteVertices( void* vertices, s32 offset, const Matrix4& transform, s32 width, s32 height, const Rgba& color, f32 scaleFactor ) const
{
    // A set of sprite vertices in a local space
    Vec4 localSpaceVertex[] = {
          { -width * scaleFactor * 0.5f, -height * scaleFactor * 0.5f, 0.0f, 1.0f }
        , {  width * scaleFactor * 0.5f, -height * scaleFactor * 0.5f, 0.0f, 1.0f }
        , {  width * scaleFactor * 0.5f,  height * scaleFactor * 0.5f, 0.0f, 1.0f }
        , { -width * scaleFactor * 0.5f,  height * scaleFactor * 0.5f, 0.0f, 1.0f }
    };

    // A set of sprite texture coordinates
    Vec2 uv[] = {
          { 0.0f, 0.0f }
        , { 1.0f, 0.0f }
        , { 1.0f, 1.0f }
        , { 0.0f, 1.0f }
    };

    // Write sprite vertex attributes to an output stream
    for( s32 i = 0; i < 4; i++ ) {
        m_vertexFormat.setVertexAttribute( VertexFormat::Position, Vec3( transform * localSpaceVertex[i] ), vertices, offset + i );
        m_vertexFormat.setVertexAttribute( VertexFormat::Color, color.toInteger(), vertices, offset + i );
        m_vertexFormat.setVertexAttribute( VertexFormat::TexCoord0, uv[i], vertices, offset + i );
    }
}

// ** SpriteRenderSystem::allocateTrianleIndexBuffer
u16* SpriteRenderSystem::allocateTrianleIndexBuffer( s32 triangleCount ) const
{
    // Calculate a total number of indices
    s32 count = triangleCount * 3;

    // Allocate an index buffer
    u16* indices = DC_NEW u16[count];

    // Initialize an index buffer
    for( s32 i = 0; i < count / 6; i++ ) {
        indices[i * 6 + 0] = 0 + i * 4;
        indices[i * 6 + 1] = 1 + i * 4;
        indices[i * 6 + 2] = 2 + i * 4;

        indices[i * 6 + 3] = 0 + i * 4;
        indices[i * 6 + 4] = 2 + i * 4;
        indices[i * 6 + 5] = 3 + i * 4;
    }

    return indices;
}

} // namespace Scene

DC_END_DREEMCHEST
