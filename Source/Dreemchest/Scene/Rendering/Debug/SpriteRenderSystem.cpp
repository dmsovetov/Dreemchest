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

// ** SpriteRenderSystem::SpriteRenderSystem
SpriteRenderSystem::SpriteRenderSystem( RenderingContext& context, RenderScene& renderScene, f32 scaleFactor )
    : RenderSystemBase( context, renderScene, renderScene.scene()->ecs()->requestIndex( "", Ecs::Aspect::all<Camera, Transform>() ) )
	, m_vertexFormat( VertexFormat::Position | VertexFormat::Color | VertexFormat::Uv0 )
	, m_scaleFactor( scaleFactor )
{
	// Allocate and initialize an index buffer
	m_indices = allocateTrianleIndexBuffer( MaxSpritesInBatch * 2 );

	// Request an input layout for a 2D vertex format
	m_inputLayout  = context.requestInputLayout( m_vertexFormat );

	// Request a vertex buffer used for rendering
	m_vertexBuffer = context.requestVertexBuffer( NULL, MaxSpritesInBatch * m_vertexFormat.vertexSize() );

	// Request an index buffer and upload data
	m_indexBuffer = context.requestIndexBuffer( m_indices, MaxSpritesInBatch * 6 * sizeof( u16 ) );
}

// ** SpriteRenderSystem::emitRenderOperations
void SpriteRenderSystem::emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform )
{
	// Get all sprites that reside in scene
	const RenderScene::Sprites& sprites = m_renderScene.sprites();

	// Begin batch by allocating a chunk of vertex data
	void* vertices = frame.allocate( m_vertexFormat.vertexSize() * MaxSpritesInBatch * 2 );

	// A total number of emitted vertices
	s32 vertexCount = 0;

	// Process each sprite an write it to a dynamic mesh vertex buffer
	for( s32 i = 0, n = sprites.count(); i < n; i++ ) {
		// Get a sprite by index
		const RenderScene::SpriteNode& sprite = sprites[i];
		
		// Write the sprite inside into a vertex buffer
		emitSpriteVertices( vertices, vertexCount, *sprite.matrix, sprite.sprite->width(), sprite.sprite->height(), sprite.sprite->color() );

		// All sprites contain 4 vertices, so increase a vertex count by this amount
		vertexCount += 4;
	}

	// Upload vertex data to a GPU buffer
	commands.uploadVertexBuffer( m_vertexBuffer, vertices, vertexCount * m_vertexFormat.vertexSize() );

	// Setup a sprite render state
	StateScope state = stateStack.newScope();
	state->bindVertexBuffer( m_vertexBuffer );
	state->bindIndexBuffer( m_indexBuffer );
	state->bindInputLayout( m_inputLayout );

	// Emit a draw indexed command to render sprites
	commands.drawIndexed( 0, Renderer::PrimTriangles, stateStack.states(), 0, vertexCount * 3 );
}

// ** SpriteRenderSystem::emitSpriteVertices
void SpriteRenderSystem::emitSpriteVertices( void* vertices, s32 offset, const Matrix4& transform, s32 width, s32 height, const Rgba& color ) const
{
	// A sprite size multiplier
	f32 scaleFactor = m_scaleFactor * 0.5f;

	// A set of sprite vertices in a local space
	Vec4 localSpaceVertex[] = {
		  { -width * scaleFactor, -height * scaleFactor, 0.0f, 1.0f }
		, {  width * scaleFactor, -height * scaleFactor, 0.0f, 1.0f }
		, {  width * scaleFactor,  height * scaleFactor, 0.0f, 1.0f }
		, { -width * scaleFactor,  height * scaleFactor, 0.0f, 1.0f }
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
		m_vertexFormat.setVertexAttribute( VertexFormat::Uv0, uv[i], vertices, offset + i );
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