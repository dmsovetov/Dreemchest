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

#ifndef __DC_Scene_Systems_CullingSystems_H__
#define __DC_Scene_Systems_CullingSystems_H__

#include "../Scene.h"

#include "../Components/Transform.h"
#include "../Components/Rendering.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Performs frustum culling for all active cameras.
	class FrustumCullingSystem : public Ecs::EntityWithComponentsSystem2<StaticMesh, Transform> {
	public:

							//! Constructs FrustumCullingSystem instance.
							FrustumCullingSystem( const Ecs::IndexPtr& cameras )
								: EntityWithComponentsSystem2( "FrustumCulling" ), m_cameras( cameras ) {}

	protected:

		//! Calculates frustums for all cameras.
		virtual bool		begin( u32 currentTime );

		//! Perform culling of a single mesh.
		virtual void		process( u32 currentTime, f32 dt, Ecs::Entity& entity, StaticMesh& staticMesh, Transform& transform );

	private:

		Ecs::IndexPtr		m_cameras;	//!< All cameras used for culling.
		Array<PlaneClipper>	m_frustums;	//!< Active frustums.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Systems_CullingSystems_H__    */