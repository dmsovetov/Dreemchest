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

#include "Spatial.h"

#include "../Components/Rendering.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------------------------------ Spatial::Result ------------------------------------------------------------------ //

// ** Spatial::Result::Result
Spatial::Result::Result( void )
{
    memset( ray.point, 0, sizeof( ray.point ) );
    ray.time = -1.0f;
}

// ** Spatial::Result::Result
Spatial::Result::Result( SceneObjectWPtr sceneObject, const Vec3& point, f32 time )
    : sceneObject( sceneObject )
{
    ray.time = time;

    for( s32 i = 0; i < 3; i++ ) {
        ray.point[i] = point[i];
    }
}

// ---------------------------------------------------------------------- Spatial ---------------------------------------------------------------------- //

// ** Spatial::Spatial
Spatial::Spatial( SceneWPtr scene )
    : m_scene( scene )
    , m_meshes( scene->ecs()->requestIndex( "Static Meshes", Ecs::Aspect::all<StaticMesh>() ) )
{
}

// ** Spatial::queryRay
Spatial::Results Spatial::queryRay( const Ray& ray, const FlagSet8& flags ) const
{
	// Get entity set from index
	const Ecs::EntitySet& entities = m_meshes->entities();

	// Resulting array
	Results results;

	// Iterate over all meshes in scene an store all that are intersected by a ray.
	for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
		// Get the world space bounding box of a mesh.
		const Bounds& bounds = (*i)->get<StaticMesh>()->worldSpaceBounds();

		// Check for intersection.
		Result result( *i, Vec3::zero(), -1.0f );

		if( ray.intersects( bounds, result.ray.point, &result.ray.time ) ) {
			results.push_back( result );
		}
	}

	// Sort results
	if( flags.is( QueryBackToFront ) ) {
		std::sort( results.begin(), results.end(), Spatial::rayTracingResultGreater );
	} else {
		std::sort( results.begin(), results.end(), Spatial::rayTracingResultLess );
	}

	// Remove all elements except the first one if single result requested
	if( !results.empty() && flags.is( QuerySingle ) ) {
		results.erase( results.begin() + 1, results.end() );
	}

	return results;
}

// ** Spatial::rayTracingResultGreater
bool Spatial::rayTracingResultLess( const Result& a, const Result& b )
{
    return a.ray.time < b.ray.time;
}

// ** Spatial::rayTracingResultGreater
bool Spatial::rayTracingResultGreater( const Result& a, const Result& b )
{
    return a.ray.time > b.ray.time;
}

} // namespace Scene

DC_END_DREEMCHEST