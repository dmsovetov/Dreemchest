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

#include "Terrain.h"
#include "../Editors/VisualEditor.h"

DC_BEGIN_COMPOSER

// --------------------------------------------------------------------- TerrainTool ------------------------------------------------------------------------ //

// ** TerrainTool::TerrainTool
TerrainTool::TerrainTool( Scene::TerrainWPtr terrain, f32 radius ) : m_radius( radius ), m_terrain( terrain )
{

}

// ** TerrainTool::radius
f32 TerrainTool::radius( void ) const
{
	return m_radius;
}

// ** TerrainTool::setRadius
void TerrainTool::setRadius( f32 value )
{
	m_radius = value;
}

// ** TerrainTool::terrain
Scene::TerrainWPtr TerrainTool::terrain( void ) const
{
	return m_terrain;
}

// ----------------------------------------------------------------- TerrainHeightmapSystem ----------------------------------------------------------------- //

// ** TerrainHeightmapSystem::TerrainHeightmapSystem
TerrainHeightmapSystem::TerrainHeightmapSystem( Scene::SceneObjectWPtr tool, Editors::CursorWPtr cursor )
	: GenericEntitySystem( "TerrainHeightmapSystem" ), m_cursor( cursor ), m_tool( tool )
{
}

// ** TerrainHeightmapSystem::process
void TerrainHeightmapSystem::process( u32 currentTime, f32 dt, Ecs::Entity& entity, Editors::TerrainChunk& chunk, Scene::StaticMesh& mesh, Scene::Transform& transform )
{
	// Find the intersection point of ray & terrain
	Vec3 point = findTerrainIntersection( chunk.terrain(), m_cursor->ray() );

	// Place tool to intersection point
	m_tool->get<Scene::Transform>()->setPosition( point );

	// No mouse buttons pressed - tool is not working
	if( !m_cursor->buttons().is( Ui::LeftMouseButton ) ) {
		return;
	}

	// Get terrain heightmap.
	Scene::TerrainWPtr terrain   = chunk.terrain();
	Scene::Heightmap&  heightmap = terrain->heightmap();

	// Get tool settings.
	f32  radius   = m_tool->get<TerrainTool>()->radius();
	Vec3 position = m_tool->get<Scene::Transform>()->position();

	// Affect the heightmap by tool.
	for( u32 z = 0; z <= heightmap.size(); z++ ) {
		for( u32 x = 0; x <= heightmap.size(); x++ ) {
			// Get the heightmap vertex position.
			Vec3 vertex = Vec3( x, terrain->heightAtVertex( x, z ), z );

			// Calculate distance to a center of a tool
			f32 distance = (position - vertex).length();

			if( distance >= radius ) {
				continue;
			}

			// Calculate the smoothing value
			//f32 smooth = sinf( 1.0f - distance / radius );
			f32 smooth = 1.0f - distance / radius;

			// Caclulate new height value
			f32 height = vertex.y + 1.0f * smooth;

			// Update the height
			heightmap.setHeight( x, z, height / terrain->maxHeight() * heightmap.maxValue() );
		}
	}

	// Rebuilt chunks
	u32 chunkIdx = 0;

	for( u32 i = 0; i < terrain->chunkCount(); i++ ) {
		for( u32 j = 0; j < terrain->chunkCount(); j++ ) {
			// Get the chunk buffers
			Scene::Terrain::VertexBuffer vertices = terrain->chunkVertexBuffer( i, j );

			// Set chunk mesh
			Scene::Mesh::VertexBuffer vb;
			vb.resize( vertices.size() );

			for( u32 v = 0, n = ( u32 )vertices.size(); v < n; v++ ) {
				vb[v].position = vertices[v].position + Vec3( i * terrain->kChunkSize, 0, j * terrain->kChunkSize );
				vb[v].normal = vertices[v].normal;
				vb[v].uv[0] = vertices[v].uv;
			}

			mesh.mesh()->setVertexBuffer( chunkIdx, vb );

			// Increase the chunk index
			chunkIdx++;
		}
	}

	// Rebuild terrain mesh.
	mesh.mesh()->invalidateRenderable();
}

// ** TerrainHeightmapSystem::findTerrainIntersection
Vec3 TerrainHeightmapSystem::findTerrainIntersection( Scene::TerrainWPtr terrain, const Ray& ray, f32 epsilon ) const
{
	f32 dt		= 0.5f;
	f32 scalar	= 0.5f;
	f32 step	= 0.25f;
	Vec3 point;

	do {
		// Get the point on ray
		point = ray.origin() + ray.direction() * 1000.0f * scalar;

		// Get height at point
		f32 height = terrain->height( point.x, point.z );

		// Check the height difference, if it's less than epsilon - intersection point is found.
		if( fabs( height - point.y ) <= epsilon ) {
			break;
		}

		// Select the next segment of a ray based on height difference
		if( height < point.y ) {
			f32 newScalar = scalar + step;
			dt = newScalar - scalar;
			scalar = newScalar;
		}
		else if( height > point.y ) {
			f32 newScalar = scalar - step;
			dt = scalar - newScalar;
			scalar = newScalar;
		}

		step *= 0.5f;
	} while( dt >= 0.00001f );

	return point;
}

// -------------------------------------------------------------- TerrainToolPass -------------------------------------------------------------- //

// ** TerrainToolPass::render
void TerrainToolPass::render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const TerrainTool& tool, const Scene::Transform& transform )
{
	Renderer::Renderer2DWPtr renderer = context->renderer();

	renderer->pushTransform( transform.matrix() );
	{
		Vec3 prev;

		for( s32 i = 0, n = 64; i <= n; i++ ) {
			f32  angle = radians( i * (360.0f / n) );
			Vec3 side  = (Vec3::axisX() * cosf( angle ) + Vec3::axisZ() * sinf( angle )) * tool.radius();
			Vec3 end   = side;

			f32 height = tool.terrain()->height( side.x + transform.position().x, side.z + transform.position().z );

			end += Vec3( 0.0f, height - transform.position().y, 0.0f );

			if( i != 0 ) {
				renderer->line( prev, end, Rgba( 0.0f, 1.0f, 0.0f, height == -1.0f ? 0.0f : 1.0f ) );
			}

			prev = end;
		}
	}
	renderer->popTransform();
}

DC_END_COMPOSER