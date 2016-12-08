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

#ifndef __DC_Composer_Menu_H__
#define __DC_Composer_Menu_H__

#include "../Composer.h"

#define BindAction( target ) std::bind( &target, this, std::placeholders::_1 )
#define BindSignal( target ) std::bind( &target, this )

DC_BEGIN_COMPOSER

namespace Ui {

    //! Menu action flags
    enum ActionFlags {
          ItemDisabled    = BIT( 0 )    //!< Menu action is disabled.
        , ItemCheckable    = BIT( 1 )   //!< Menu action is checkable.
        , ItemChecked    = BIT( 2 )     //!< Menu action is checked.
        , ItemHidden    = BIT( 3 )      //!< Menu action is hidden.
    };

    //! Qt action extension.
    class Action : public QAction {

        Q_OBJECT

    public:

                                    //! Constructs Action instance.
                                    Action( QWidget* parent, const String& text, ActionCallback callback );

        //! Sets the menu action shortcut.
        void                        setShortcut( const String& value );
    
        //! Sets the menu action icon.
        void                        setIcon( const String& value );

        //! Creates new Action instance.
        static ActionQPtr            create( const String& text, ActionCallback callback, const String& shortcut = String(), const String& icon = String(), s32 flags = 0 );
    
    private Q_SLOTS:

        //! Action triggered slot to run an action callback.
        void                        triggered( void );
    
    private:

        ActionCallback                m_callback;    //!< Menu action callback.
    };

    //! Qt tool bar extension.
    class ToolBar : public QToolBar {
    public:

                                    //! Constructs ToolBar instance.
                                    ToolBar( QWidget* parent );

        //! Adds an action to a tool bar.
        void                        addAction( ActionQPtr action );

        //! Creates new action owned by this tool bar.
        ActionQPtr                    addAction( const String& text, ActionCallback callback, const String& shortcut = String(), const String& icon = String(), s32 flags = 0 );

        //! Begins the action group construction.
        void                        beginActionGroup( void );

        //! Ends the action group construction.
        void                        endActionGroup( void );

    private:

        QVector<ActionQPtr>            m_group;    //!< The action group being constructed.
    };

    //! Qt menu extension.
    class Menu : public QMenu {
    public:

                                    //! Constructs Menu instance.
                                    Menu( QWidget* parent );

        //! Returns the total number of items inside the menu.
        s32                            size( void ) const;

        //! Adds new submenu.
        MenuQPtr                    addMenu( const String& text );

        //! Adds new action to a menu.
        ActionQPtr                    addAction( const String& text, ActionCallback callback, const String& shortcut = String(), const String& icon = String(), s32 flags = 0 );
    };

} // namespace Ui

DC_END_COMPOSER

#endif    /*    !__DC_Composer_Menu_H__    */
