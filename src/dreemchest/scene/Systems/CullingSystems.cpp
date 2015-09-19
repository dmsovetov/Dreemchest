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

#include "CullingSystems.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** FrustumCullingSystem::begin
bool FrustumCullingSystem::begin( u32 currentTime )
{
	// Clear frustums
	m_frustums.clear();

	// Extract frustums from active cameras.
	const Ecs::EntitySet& cameras = m_cameras->entities();

	u8 cameraId = 0;

	for( Ecs::EntitySet::const_iterator i = cameras.begin(), end = cameras.end(); i != end; ++i ) {
		Camera*	   camera		   = (*i)->get<Camera>();
		Transform* cameraTransform = (*i)->get<Transform>();
		Frustum	   frustum;

		if( !camera->view().valid() ) {
			continue;
		}

		camera->setId( cameraId++ );

		Matrix4 clip = camera->calculateClipSpace( cameraTransform->matrix() );
		Camera::calculateFrustum( clip, frustum.m_planes );
		frustum.m_id = camera->id();

		m_frustums.push_back( frustum );
	}

	return EntityWithComponentsSystem2::begin( currentTime );
}

// ** FrustumCullingSystem::process
void FrustumCullingSystem::process( u32 currentTime, f32 dt, Ecs::Entity& entity, StaticMesh& staticMesh, Transform& transform )
{
	Bounds bounds = staticMesh.bounds() * transform.matrix();

	for( u32 i = 0, n = m_frustums.size(); i < n; i++ ) {
		const Frustum& frustum = m_frustums[i];
		bool		   inside  = true;

		for( u32 j = 0; j < 6; j++ ) {
			if( frustum.m_planes[j].isBehind( bounds ) ) {
				inside = false;
				break;
			}
		}

		staticMesh.setVisibilityMask( BIT( frustum.m_id ), inside );
	}
}

} // namespace Scene

DC_END_DREEMCHEST