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

#ifndef __DC_Composer_SceneModel_H__
#define __DC_Composer_SceneModel_H__

#include "../Composer.h"

DC_BEGIN_COMPOSER

	//! Scene model interface.
	class SceneModel : public IInterface {
	public:

		//! Available asset drop action.
		struct AssetAction {
			//! Asset drop action types.
			enum Type {
				  Invalid				//!< The drop action is invalid.
				, PlaceMesh				//!< Mesh asset can be dropped.
				, AssignMaterial		//!< Material can be assigned.
			};

										//! Constructs AssetAction instance.
										AssetAction( Type type, Scene::AssetSet assets, Scene::SceneObjectWPtr sceneObject, const Vec3& point = Vec3( 0, 0, 0 ) )
											: type( type ), sceneObject( sceneObject ), assets( assets ), point( point ) {}

										//! Constructs AssetAction instance from type.
										AssetAction( Type type )
											: type( type ) {}

										//! Returns true if this is a valid action.
										operator bool( void ) const { return type != Invalid; }

			Type						type;			//!< Drop action type.
			Scene::SceneObjectWPtr		sceneObject;	//!< Target scene object.
			Scene::AssetSet				assets;			//!< Asset set that should be used.
			Vec3						point;			//!< Target world space point.
		};

		//! Returns an acceptable drop action by a set of assets & target scene object
		virtual AssetAction				acceptableAssetAction( const Scene::AssetSet& assets, Scene::SceneObjectWPtr target, const Vec3& point = Vec3( 0, 0, 0 ) ) const = 0;

		//! Handles an asset drop action to a target scene object & world space point.
		virtual bool					performAssetAction( const AssetAction& action ) = 0;

		//! Applies material to a static mesh.
		virtual void					applyMaterial( Scene::SceneObjectWPtr target, s32 slot, Scene::MaterialWPtr material ) = 0;

		//! Places mesh instance to scene at specified world space point.
		virtual Scene::SceneObjectWPtr	placeStaticMesh( Scene::MeshWPtr mesh, const Vec3& point ) = 0;

		//! Places terrain instance to a scene at specified world space point.
		virtual Scene::SceneObjectWPtr	placeTerrain( Scene::TerrainWPtr terrain, const Vec3& point ) = 0;

		//! Changes the parent of a scene object to a new one.
		virtual void					changeSceneObjectParent( Scene::SceneObjectWPtr sceneObject, Scene::SceneObjectWPtr parent ) = 0;
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_SceneModel_H__	*/