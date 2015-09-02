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

#ifndef __DC_Scene_Systems_Input2DSystems_H__
#define __DC_Scene_Systems_Input2DSystems_H__

#include "SceneSystem.h"

#include "../Components/Transform.h"
#include "../Components/Input.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Moves scene object transform 2d according to a WASD input scheme
	class WasdInput2DSystem : public SceneSystem2<WasdInput2D, Transform> {
	public:

							//! Constructs a WasdInput2DSystem instance.
							WasdInput2DSystem( Ecs::Entities& entities );

		//! Moves the scene object
		virtual void		process( u32 currentTime, f32 dt, SceneObject& sceneObject, WasdInput2D& input, Transform& transform );

	private:

		platform::Input*	m_input;	//!< The input instance.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Systems_Input2DSystems_H__    */