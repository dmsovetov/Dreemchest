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

#ifndef __DC_Composer_Qt_SceneModelPrivate_H__
#define __DC_Composer_Qt_SceneModelPrivate_H__

#include "AbstractTreeModel.h"
#include "../SceneModel.h"

DC_BEGIN_COMPOSER

	class SceneModelPrivate;

	//! Scene model Qt implementation.
	class QSceneModel : public QGenericTreeModel<Scene::SceneObjectWPtr> {
	public:

								//! Constructs QSceneModel instance.
								QSceneModel( SceneModelPrivate* parentSceneModel, Scene::SceneWPtr scene, QObject* parent = NULL );

		//! Removes the scene object at specified index.
		void					remove( const QModelIndex& index );

		//! Returns scene instance.
		Scene::SceneWPtr		scene( void ) const;

	private:

		//! Returns the item flags.
		virtual Qt::ItemFlags	flags( const QModelIndex& index ) const Q_DECL_OVERRIDE;

		//! Returns the model data by index.
		virtual QVariant		data( const QModelIndex& index, int role ) const Q_DECL_OVERRIDE;

		//! Writes new data to model at specified index.
		virtual bool			setData( const QModelIndex& index, const QVariant& value, int role ) Q_DECL_OVERRIDE;

		//! Changes the scene object parent transform.
		virtual bool			moveItem( Item* sourceParent, Item* destinationParent, Item* item, int destinationRow ) const DC_DECL_OVERRIDE;

		//! Handles the drop operation.
		virtual bool			dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) Q_DECL_OVERRIDE;

	#ifdef DC_QT5_ENABLED
		//! Checks if the MIME data can be dropped to an item.
		virtual bool			canDropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) const Q_DECL_OVERRIDE;
	#endif	/*	DC_QT5_ENABLED	*/

		//! Handles scene object addition.
		void					handleSceneObjectAdded( const Scene::Scene::SceneObjectAdded& e );

		//! Handles scene object removal.
		void					handleSceneObjectRemoved( const Scene::Scene::SceneObjectRemoved& e );

	private:

		SceneModelPrivate*		m_parent;	//!< Parent scene model.
		Scene::SceneWPtr		m_scene;	//!< Actual scene.
	};

	//! Scene model interface.
	class SceneModelPrivate : public PrivateInterface<SceneModel, QSceneModel> {
	public:

										//! Constructs SceneModelPrivate instance.
										SceneModelPrivate( Scene::AssetBundleWPtr assets, Scene::SceneWPtr scene );

		//! Returns an acceptable drop action by a set of assets & target scene object
		virtual AssetAction				acceptableAssetAction( const Scene::AssetSet& assets, Scene::SceneObjectWPtr target, const Vec3& point ) const DC_DECL_OVERRIDE;

		//! Handles an asset drop action to a target scene object & world space point.
		virtual bool					performAssetAction( const AssetAction& action ) DC_DECL_OVERRIDE;

		//! Applies material to a static mesh.
		virtual void					applyMaterial( Scene::SceneObjectWPtr target, s32 slot, Scene::MaterialWPtr material ) DC_DECL_OVERRIDE;

		//! Places terrain instance to a scene at specified world space point.
		virtual Scene::SceneObjectWPtr	placeTerrain( Scene::TerrainWPtr terrain, const Vec3& point ) DC_DECL_OVERRIDE;

		//! Places mesh instance to scene at specified world space point.
		virtual Scene::SceneObjectWPtr	placeStaticMesh( Scene::MeshWPtr mesh, const Vec3& point ) DC_DECL_OVERRIDE;

		//! Changes the parent of a scene object to a new one.
		virtual void					changeSceneObjectParent( Scene::SceneObjectWPtr sceneObject, Scene::SceneObjectWPtr parent ) DC_DECL_OVERRIDE;

	private:

		Scene::AssetBundleWPtr			m_assets;	//!< Assets model to use by this scene.
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Qt_SceneModelPrivate_H__	*/