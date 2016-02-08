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

#include "TerrainEditing.h"
#include "../../Editors/VisualEditor.h"

DC_BEGIN_COMPOSER

// --------------------------------------------------------------------- TerrainTool ------------------------------------------------------------------------ //

// ** TerrainTool::TerrainTool
TerrainTool::TerrainTool( Scene::TerrainHandle terrain, f32 radius, f32 strength, f32 exp ) : m_radius( radius ), m_terrain( terrain ), m_strength( strength ), m_type( Raise ), m_exp( exp )
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

// ** TerrainTool::strength
f32 TerrainTool::strength( void ) const
{
	return m_strength;
}

// ** TerrainTool::setStrength
void TerrainTool::setStrength( f32 value )
{
	m_strength = value;
}

// ** TerrainTool::exp
f32 TerrainTool::exp( void ) const
{
    return m_exp;
}

// ** TerrainTool::setExp
void TerrainTool::setExp( f32 value )
{
    m_exp = value;
}

// ** TerrainTool::terrain
Scene::TerrainHandle TerrainTool::terrain( void ) const
{
	return m_terrain;
}

// ** TerrainTool::influence
f32 TerrainTool::influence( f32 distance ) const
{
	if( distance >= m_radius ) {
		return 0.0f;
	}

    DC_BREAK_IF( m_strength < 0.0f || m_strength > 1.0f );
    f32 factor = distance / m_radius;
	return sinf( 1.0f - powf( factor, m_exp ) ) * m_strength;
}

// ** TerrainTool::type
TerrainTool::Type TerrainTool::type( void ) const
{
	return m_type;
}

// ** TerrainTool::setType
void TerrainTool::setType( Type value )
{
	m_type = value;
}

// ----------------------------------------------------------------- TerrainHeightmapSystem ----------------------------------------------------------------- //

// ** TerrainHeightmapSystem::TerrainHeightmapSystem
TerrainHeightmapSystem::TerrainHeightmapSystem( Scene::SceneObjectWPtr tool, Scene::ViewportWPtr viewport )
	: GenericTouchSystem( viewport ), m_tool( tool )
{
}

// ** TerrainHeightmapSystem::touchMovedEvent
void TerrainHeightmapSystem::touchMovedEvent( Scene::Viewport::TouchMoved& e, Ecs::Entity& entity, Editors::TerrainChunk& chunk, Scene::StaticMesh& mesh, Scene::Transform& transform )
{
    if( !m_tool->isEnabled<TerrainTool>() ) {
        return;
    }

	// Find the intersection point of ray & terrain
	Vec3 point = chunk.terrain()->rayMarch( m_viewport->ray() );

	// Place tool to intersection point
	m_tool->get<Scene::Transform>()->setPosition( point );

	// Get tool settings.
	f32  radius   = m_tool->get<TerrainTool>()->radius();
	Vec3 position = transform.matrix().inversed() * m_tool->get<Scene::Transform>()->position();

	// No mouse buttons pressed - tool is not working
	if( !m_viewport->flags().is( Ui::MouseButtons::Left ) ) {
		return;
	}

	// Check that intersection point is inside the chunk
	if( !isPointInside( position, radius, chunk ) ) {
		return;
	}

	// Get terrain heightmap.
    Scene::TerrainHandle terrain = chunk.terrain();
    Assets::AssetWriteLock<Scene::Terrain> locked = terrain.writeLock();
    Scene::Heightmap& heightmap = locked->heightmap();

	point = transform.parent()->matrix().inversed() * point;

	// Calculate the affected heightmap rect.
	s32 cr       = ceil( radius );
	s32 cx       = point.x;
	s32 cz       = point.z;
    f32 strength = m_tool->get<TerrainTool>()->strength();

	// Affect the heightmap by tool.
	for( s32 z = cz - cr; z <= cz + cr; z++ ) {
		for( s32 x = cx - cr; x <= cx + cr; x++ ) {
			// Ensure this is a valid vertex coodinates.
            if( !terrain->hasVertex( x, z ) ) {
                continue;
            }

			// Get the heightmap vertex position.
			Vec3 vertex = Vec3( ( f32 )x, terrain->heightAtVertex( x, z ), ( f32 )z );

			// Calculate distance to a center of a tool
			f32 distance = (point - vertex).length();

			// Get the influence
			f32 influence = m_tool->get<TerrainTool>()->influence( distance );

			if( influence <= 0.0f ) {
				continue;
			}

            DC_BREAK_IF( influence < 0.0f || influence > 1.0f );

			// Caclulate new height value
            f32 height = 0.0f;

            switch( m_tool->get<TerrainTool>()->type() ) {
            case TerrainTool::Lower:    height = applyLowering( vertex, x, z, terrain, influence, strength );   break;
            case TerrainTool::Raise:    height = applyRaising( vertex, x, z, terrain, influence, strength );    break;
            case TerrainTool::Level:    height = applyLeveling( vertex, x, z, terrain, influence, strength );   break;
            case TerrainTool::Flatten:  height = applyFlattening( vertex, x, z, terrain, influence, strength ); break;
            case TerrainTool::Smooth:   height = applySmoothing( vertex, x, z, terrain, influence, strength );  break;
            }

			// Update the height
			heightmap.setHeight( x, z, max2( height, 0.0f ) / terrain->maxHeight() * heightmap.maxValue() );
		}
	}

#if 0
	// Get the chunk buffers
	Scene::Terrain::VertexBuffer vertices = terrain->chunkVertexBuffer( chunk.x(), chunk.z() );

	// Set chunk mesh
	Scene::Mesh::VertexBuffer vb;
	vb.resize( vertices.size() );

	for( u32 v = 0, n = ( u32 )vertices.size(); v < n; v++ ) {
		vb[v].position = vertices[v].position;
		vb[v].normal = vertices[v].normal;
		vb[v].uv[0] = vertices[v].uv;
	}

	mesh.mesh()->setVertexBuffer( 0, vb );
	mesh.mesh()->updateBounds();

	// Rebuild terrain mesh.
	mesh.mesh()->invalidateRenderable();
#else
    DC_NOT_IMPLEMENTED
#endif
}

// ** TerrainHeightmapSystem::isPointInside
bool TerrainHeightmapSystem::isPointInside( const Vec3& point, f32 radius, const Editors::TerrainChunk& chunk ) const
{
	if( point.x < -radius || point.x > Scene::Terrain::kChunkSize + radius ) return false;
	if( point.y < -radius || point.y > Scene::Terrain::kChunkSize + radius ) return false;

	return true;
}

// ** TerrainHeightmapSystem::applyLowering
f32 TerrainHeightmapSystem::applyLowering( const Vec3& vertex, s32 x, s32 z, Scene::TerrainHandle terrain, f32 influence, f32 strength ) const
{
    return vertex.y - influence;
}

// ** TerrainHeightmapSystem::applyRaising
f32 TerrainHeightmapSystem::applyRaising( const Vec3& vertex, s32 x, s32 z, Scene::TerrainHandle terrain, f32 influence, f32 strength ) const
{
    return vertex.y + influence;
}

// ** TerrainHeightmapSystem::applyFlattening
f32 TerrainHeightmapSystem::applyFlattening( const Vec3& vertex, s32 x, s32 z, Scene::TerrainHandle terrain, f32 influence, f32 strength ) const
{
    // Calculate avarage terrain height in affected radius.
    f32 average = calculateAverageHeight( terrain, x, z, 10 );

    // Caclulate new height.
    f32 sign   = average > vertex.y ? 1.0f : -1.0f;
    f32 result = vertex.y + influence * sign;

    // Clamp height value.
    if( (sign < 0.0f && result < average) || (sign > 0.0f && result > average) ) {
        result = average;
    }

    return average;
}

// ** TerrainHeightmapSystem::applyLeveling
f32 TerrainHeightmapSystem::applyLeveling( const Vec3& vertex, s32 x, s32 z, Scene::TerrainHandle terrain, f32 influence, f32 strength ) const
{
    // Calculate target height.
    f32 height = terrain->maxHeight() * strength;

    // Caclulate new height.
    f32 result = vertex.y + influence * (height > vertex.y ? 1.0f : -1.0f);

    // Clamp height.
    result = min2( height, result );

    return result;
}

// ** TerrainHeightmapSystem::applySmoothing
f32 TerrainHeightmapSystem::applySmoothing( const Vec3& vertex, s32 x, s32 z, Scene::TerrainHandle terrain, f32 influence, f32 strength ) const
{
    // Calculate avarage terrain height around the vertex.
    f32 average = calculateAverageHeight( terrain, x, z, 1 );

    // Now interpolate current vertex height with an avarage value
    f32 result = average * influence + vertex.y * (1.0f - influence);
    return result;
}

// ** TerrainHeightmapSystem::calculateAverageHeight
f32 TerrainHeightmapSystem::calculateAverageHeight( Scene::TerrainHandle terrain, s32 x, s32 z, s32 radius ) const
{
    f32 average = 0.0f;
    s32 count   = 0;

	for( s32 j = z - radius; j <= z + radius; j++ ) {
		for( s32 i = x - radius; i <= x + radius; i++ ) {
			if( !terrain->hasVertex( i, j ) ) {
                continue;
            }

            average += terrain->heightAtVertex( i, j );
			count   += 1;
		}
	}

	average /= count;

    return average;
}

// -------------------------------------------------------------- TerrainToolPass -------------------------------------------------------------- //

// ** TerrainToolPass::render
void TerrainToolPass::render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const TerrainTool& tool, const Scene::Transform& transform )
{
	Renderer::Renderer2DWPtr renderer = context->renderer();

	s32 r = ceil( tool.radius() );
	s32 x = transform.position().x;
	s32 z = transform.position().z;

	Scene::TerrainHandle terrain = tool.terrain();

	for( s32 j = z - r; j < z + r; j++ ) {
		for( s32 i = x - r; i < x + r; i++ ) {
			// Ensure this is a valid vertex coodinates.
            if( !terrain->hasVertex( i, j ) ) {
                continue;
            }

			f32 height = tool.terrain()->height( i, j );
			Vec3 point = Vec3( ( f32 )i, height, ( f32 )j );

			// Get the influence
			f32 influence = tool.influence( (point - transform.position()).length() );

			if( influence <= 0.0f ) {
				continue;
			}

			renderer->box( Bounds( point - Vec3(5.0f,0.1f,5.0f) * influence, point + Vec3(5.0f,0.1f,5.0f) * influence ), Rgba( 1.0f, 1.0f, 0.0f ) );
		}	
	}
}

DC_END_COMPOSER