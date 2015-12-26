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

#ifndef __DC_Composer_RotationTool_H__
#define __DC_Composer_RotationTool_H__

#include "Gizmo.h"
#include "../Editors/VisualEditor.h"

DC_BEGIN_COMPOSER

	//! Rotation tool component.
	class RotationTool: public Ecs::Component<RotationTool> {
	public:

							//! Construct RotationTool instance.
							RotationTool( f32 scalingFactor = 0.175f );

		//! Returns the scaling factor of a gizmo.
		f32					scalingFactor( void ) const;

		//! Returns gizmo data.
		const Gizmo&		gizmo( void ) const;
		Gizmo&				gizmo( void );

	private:

		f32					m_scalingFactor;	//!< The gizmo scaling factor.
		Gizmo				m_gizmo;			//!< Actual gizmo state.
	};

	//! Rotates transform by a rotation tool.
	class RotationToolSystem : public Ecs::GenericEntitySystem<RotationTool, Scene::Transform> {
	public:

									//! Constructs RotationToolSystem instance
									RotationToolSystem( Scene::SpectatorCameraWPtr camera );

		//! Sets cursor position.
		void						setCursor( u32 x, u32 y, u8 buttons );

		//! Processes the single rotation tool.
		virtual void				process( u32 currentTime, f32 dt, Ecs::Entity& entity, RotationTool& tool, Scene::Transform& transform ) DC_DECL_OVERRIDE;

	private:

		//! Returns the selection from a mouse ray.
		Gizmo::Transform			findTransformByRay( RotationTool& tool, const Vec3& position, Plane& plane, Vec3& intersection, f32 scale, const Ray& ray ) const;

	private:

		Vec2						m_cursor;	//!< Last known cursor position.
		Scene::SpectatorCameraWPtr	m_camera;	//!< Camera instance used for ray casting.
		FlagSet8					m_buttons;	//!< Mouse buttons state.
	};

	//! Renders active rotation tools.
	class RotationToolPass : public Scene::RenderPass<RotationTool> {
	public:

						//! Constructs RotationToolPass instance.
						RotationToolPass( Ecs::EcsWPtr ecs )
							: RenderPass( ecs, "RotationToolPass" ) {}

	protected:

		//! Renders the mesh of a single translation tool.
		virtual void	render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const RotationTool& tool, const Scene::Transform& transform ) DC_DECL_OVERRIDE;
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_RotationTool_H__	*/