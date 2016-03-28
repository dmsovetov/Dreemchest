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

#ifndef __DC_Composer_EntityInspector_H__
#define __DC_Composer_EntityInspector_H__

#include "../Composer.h"

DC_BEGIN_COMPOSER

namespace Ui {

    //! Displays entity as a list of it's components.
    class EntityInspector : public QWidget {

        Q_OBJECT

    public:

                                    //! Constructs EntityInspector instance.
                                    EntityInspector( QWidget* parent = NULL );

        //! Binds an entity to this inspector
        void                        bind( Ecs::EntityWPtr entity );

        //! Returns true if entity has unsaved changes.
        bool                        hasChanges( void ) const;

        //! Returns bound entity.
        Ecs::EntityWPtr             entity( void ) const;

    public Q_SLOTS:

        //! Saves entity to a storage.
        void                        save( void );

    private:

        //! Rebuilds entity inspector layout.
        void                        refresh( void );

        //! Sets the 'has changes' flag and setups corresponding controlls.
        void                        setHasChanges( bool value );

        //! Saves entity state.
        Archive                     saveState( void ) const;

        //! Restores entity state.
        void                        restoreState( const Archive& state );

        //! Builds sorted component list.
        Ecs::ComponentWeakList      buildComponentList( void ) const;

        //! Creates component editing menu.
        QMenu*                      createComponentMenu( TypeIdx component ) const;

    private Q_SLOTS:

        //! Handles property changed signal of a property model.
        void                        propertyChanged( void );

        //! Resets entity to a previous saved state.
        void                        reset( void );

        //! Detaches component.
        void                        detachComponent( void );

        //! Attaches a new component to an entity.
        void                        attachComponent( const QString& name );

    private:

        class HorizontalSeparator;
        class Header;

        //! Container type to store all object inspectors that were created for entity components.
        typedef QVector<ObjectInspectorQPtr> ObjectInspectors;

        QVBoxLayout*                m_layout;       //!< Vertical layout for component inspectors.
        QPushButton*                m_save;         //!< Save button instance.
        QPushButton*                m_reset;        //!< Reset button instance.
        QPushButton*                m_add;          //!< Add component button instance.
        //ComponentMenu*            m_components;   //!< Component menu used to attach components.
        ObjectInspectors            m_inspectors;   //!< Component object inspectors.
        Ecs::EntityWPtr             m_entity;       //!< Bound entity instance.
        Archive                     m_previous;     //!< Previos state of en entity.
        bool                        m_hasChanges;   //!< Indicates that entity has unsaved changes.
    };

} // namespace Ui

DC_END_COMPOSER

#endif  /*  !__DC_Composer_EntityInspector_H__    */