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

#include "DebugPasses.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** OverdrawPass::setup
void OverdrawPass::setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection )
{
	// Set additive blending for all render passes
	rvm.setRasterization( AllRenderModesBit, Rvm::AdditiveRasterization );

	// Set the default shader
	rvm.setDefaultShader( shaders.shaderById( ShaderCache::ConstantColor ) );

	// Set constant color register
	rvm.setRegister( Rvm::ConstantColor, Vec4( 0.1f, 0.0f, 0.0f ) );
}

// ** VertexNormalsPass::setup
void VertexNormalsPass::setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection )
{
	// Set the default shader
	rvm.setDefaultShader( shaders.shaderById( ShaderCache::Normals ) );
}

// ** WireframePass::setup
void WireframePass::setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection )
{
	Rvm::RasterizationOptions additiveWithDepthWrite = Rvm::AdditiveRasterization.overrideDepthMask( true );

	// Set additive blending for all render passes
	rvm.setRasterization( AllRenderModesBit, additiveWithDepthWrite );

	// Set wireframe polygon mode
	rvm.setDefaultPolygonMode( Renderer::PolygonWire );

	// Set the default shader
	rvm.setDefaultShader( shaders.shaderById( ShaderCache::ConstantColor ) );

	// Set constant color register
	rvm.setRegister( Rvm::ConstantColor, Vec4( 0.1f, 0.1f, 0.0f ) );
}

// ** MeshBoundsPass::render
void MeshBoundsPass::render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const StaticMesh& staticMesh, const Transform& transform )
{
	Renderer::Renderer2DPtr renderer = context->renderer();

	renderer->wireBox( staticMesh.worldSpaceBounds(), Rgba( 1.0f, 1.0f, 0.0f, 0.25f ) );
}

// ** LightBoundsPass::render
void LightBoundsPass::render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const Light& light, const Transform& transform )
{
	Renderer::Renderer2DPtr renderer = context->renderer();

	renderer->wireSphere( transform.worldSpacePosition(), light.range(), light.color() );
}

// ** ParticlesBoundsPass::render
void ParticlesBoundsPass::render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const Particles& particles, const Transform& transform )
{
	Renderer::Renderer2DPtr renderer = context->renderer();

	renderer->wireBox( particles.instance()->bounds(), Rgba( 1.0f, 1.0f, 0.0f, 0.25f ) );
}

// -------------------------------------------------------- GridPass -------------------------------------------------------- //

// ** GridPass::render
void GridPass::render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const Grid& grid, const Transform& transform )
{
	Renderer::Renderer2DPtr renderer = context->renderer();

	renderer->pushTransform( transform.matrix() );

	// Get grid features
	FlagSet8 features = grid.features();

	// Render grid cells
	if( features.is( Grid::Cells ) ) {
		// Render with a cell size
		renderGrid( renderer, grid.cellSize() * 1.0f, grid.size(), Rgba( 0.0f, 0.0f, 0.0f, 0.05f ) );

		// Render with a cell x5 size
		renderGrid( renderer, grid.cellSize() * 5.0f, grid.size(), Rgba( 0.0f, 0.0f, 0.0f, 0.1f ) );
	}

	// Render axes
	if( features.is( Grid::Axes ) ) {
		renderCross( renderer, grid.size(), Rgba( 1.0f, 1.0f, 1.0f, 0.2f ) );
	}

	// Render basis
	if( features.is( Grid::Basis ) ) {
		renderer->line( Vec3( 0.0f, 0.0f, 0.0f ), Vec3( 1.0f, 0.0f, 0.0f ), Rgba::fromBytes( 240,  68,  31 ) );
		renderer->line( Vec3( 0.0f, 0.0f, 0.0f ), Vec3( 0.0f, 1.0f, 0.0f ), Rgba::fromBytes( 187, 237,  82 ) );
		renderer->line( Vec3( 0.0f, 0.0f, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ), Rgba::fromBytes(  61, 133, 236 ) );
	}

	// Render outline
	if( features.is( Grid::Outline ) ) {
		f32 s = grid.size() * 0.5f;
		renderer->wireBox( Bounds( Vec3( -s, 0.0f, -s ), Vec3( s, 0.0f, s ) ), Rgba( 1.0f, 1.0f, 0.0f, 0.2f ) );
	}

	renderer->popTransform();
}

// ** GridPass::renderGrid
void GridPass::renderGrid( Renderer::Renderer2DWPtr renderer, f32 cellSize, f32 size, const Rgba& color ) const
{
	s32 n = static_cast<s32>( floor( size / cellSize ) / 2.0f );

	for( s32 i = -n; i <= n; i++ ) {
		renderer->line( Vec3( i * cellSize, 0.0f, -cellSize * n ), Vec3( i * cellSize, 0.0f, cellSize * n ), color );
	}

	for( s32 i = -n; i <= n; i++ ) {
		renderer->line( Vec3( -cellSize * n, 0.0f, i * cellSize ), Vec3( cellSize * n, 0.0f, i * cellSize ), color );
	}
}

// ** GridPass::renderCross
void GridPass::renderCross( Renderer::Renderer2DWPtr renderer, f32 size, const Rgba& color ) const
{
	f32 s = size * 0.5f;

	renderer->line( Vec3( -s, 0.0f, 0.0f ), Vec3( s, 0.0f, 0.0f ), color );
	renderer->line( Vec3( 0.0f, 0.0f, -s ), Vec3( 0.0f, 0.0f, s ), color );
}

} // namespace Scene

DC_END_DREEMCHEST