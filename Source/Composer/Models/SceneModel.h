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

#include "AbstractTreeModel.h"

DC_BEGIN_COMPOSER

    //! Scene model used by UI views and scene editor.
    class SceneModel : public GenericTreeModel<Scene::SceneObjectWPtr> {
    public:

        //! Available asset drop action.
        struct AssetAction {
            //! Asset drop action types.
            enum Type {
                  Invalid                //!< The drop action is invalid.
                , PlaceMesh                //!< Mesh asset can be dropped.
                , AssignMaterial        //!< Material can be assigned.
            };

                                        //! Constructs AssetAction instance.
                                        AssetAction( Type type, Assets::AssetSet assets, Scene::SceneObjectWPtr sceneObject, const Vec3& point = Vec3( 0, 0, 0 ) )
                                            : type( type ), sceneObject( sceneObject ), assets( assets ), point( point ) {}

                                        //! Constructs AssetAction instance from type.
                                        AssetAction( Type type )
                                            : type( type ) {}

                                        //! Returns true if this is a valid action.
                                        operator bool( void ) const { return type != Invalid; }

            Type                        type;            //!< Drop action type.
            Scene::SceneObjectWPtr        sceneObject;    //!< Target scene object.
            Assets::AssetSet            assets;            //!< Asset set that should be used.
            Vec3                        point;            //!< Target world space point.
        };

                                        //! Constructs SceneModel instance.
                                        SceneModel( Assets::Assets& assets, Scene::SceneWPtr scene, QObject* parent );

        //! Returns scene instance.
        Scene::SceneWPtr                scene( void ) const;

        //! Returns an acceptable drop action by a set of assets & target scene object
        AssetAction                        acceptableAssetAction( const Assets::AssetSet& assets, Scene::SceneObjectWPtr target, const Vec3& point ) const;

        //! Handles an asset drop action to a target scene object & world space point.
        bool                            performAssetAction( const AssetAction& action );

        //! Applies material to a static mesh.
        void                            applyMaterial( Scene::SceneObjectWPtr target, s32 slot, Scene::MaterialHandle material );

        //! Places terrain instance to a scene at specified world space point.
        Scene::SceneObjectWPtr            placeTerrain( Scene::TerrainHandle terrain, const Vec3& point );

        //! Places mesh instance to scene at specified world space point.
        Scene::SceneObjectWPtr            placeStaticMesh( Scene::MeshHandle mesh, const Vec3& point );

        //! Changes the parent of a scene object to a new one.
        void                            changeSceneObjectParent( Scene::SceneObjectWPtr sceneObject, Scene::SceneObjectWPtr parent ) const;

        //! Removes the scene object at specified index.
        void                            remove( const QModelIndex& index );

    private:

        //! Returns the item flags.
        virtual Qt::ItemFlags            flags( const QModelIndex& index ) const Q_DECL_OVERRIDE;

        //! Returns the model data by index.
        virtual QVariant                data( const QModelIndex& index, int role ) const Q_DECL_OVERRIDE;

        //! Writes new data to model at specified index.
        virtual bool                    setData( const QModelIndex& index, const QVariant& value, int role ) Q_DECL_OVERRIDE;

        //! Changes the scene object parent transform.
        virtual bool                    moveItem( Item* sourceParent, Item* destinationParent, Item* item, int destinationRow ) const Q_DECL_OVERRIDE;

        //! Handles the drop operation.
        virtual bool                    dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) Q_DECL_OVERRIDE;

    #ifdef DC_QT5_ENABLED
        //! Checks if the MIME data can be dropped to an item.
        virtual bool                    canDropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) const Q_DECL_OVERRIDE;
    #endif    /*    DC_QT5_ENABLED    */

        //! Handles scene object addition.
        void                            handleSceneObjectAdded( const Scene::Scene::SceneObjectAdded& e );

        //! Handles scene object removal.
        void                            handleSceneObjectRemoved( const Scene::Scene::SceneObjectRemoved& e );

    private:

        Assets::Assets&                    m_assets;    //!< Assets use by this scene.
        Scene::SceneWPtr                m_scene;    //!< Actual scene.
    };

DC_END_COMPOSER

#endif    /*    !__DC_Composer_SceneModel_H__    */
