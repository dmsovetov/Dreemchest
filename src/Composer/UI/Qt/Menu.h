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

#include "../IMenu.h"
#include "UserInterface.h"

namespace Ui {

	//! Menu action Qt implementation
	class Action : public UserInterface<IAction, QAction> {
	public:

										//! Constructs Action instance.
										Action( QWidget* parent, const String& text, ActionCallback callback );
								
		//! Returns the checked state of a menu action.
		virtual bool					isChecked( void ) const;
	
		//! Sets the checked state of a menu action.
		virtual void					setChecked( bool value );

		//! Sets the checkable state of a menu action.
		virtual void					setCheckable( bool value );

		//! Sets the disabled state of a menu action.
		virtual void					setDisabled( bool value );
	
		//! Returns the visibility state of a menu action.
		virtual bool					isVisible( void ) const;
	
		//! Sets the visibility state of a menu action.
		virtual void					setVisible( bool value );
	
		//! Returns the menu action text.
		virtual String					text( void ) const;
	
		//! Sets the menu action text.
		virtual void					setText( const String& value );
	
		//! Sets the menu action shortcut.
		virtual void					setShortcut( const String& value );
	
		//! Sets the menu action icon.
		virtual void					setIcon( const String& value );
	
	private:

		//! Triggered callback.
		void							triggered( void );
	
	private:

		SignalDelegatePtr				m_signal;	//!< Signal delegate.
		ActionCallback					m_callback;	//!< Menu action callback.
	};

	//! Tool bar Qt implementation.
	class ToolBar : public UserInterface<IToolBar, QToolBar> {
	public:

									//! Constructs ToolBar instance.
									ToolBar( QWidget* parent );

		//! Adds an action to a toolbar.
		virtual void				addAction( IActionWPtr action );
	
		//! Removes an action from a toolbar.
		virtual void				removeAction( IActionWPtr action );
	
		//! Adds a separator to a toolbar.
		virtual void				addSeparator( void );
	};

	//! Menu Qt implementation.
	class Menu : public UserInterface<IMenu, QMenu> {
	public:

									//! Constructs Menu instance.
									Menu( QWidget* parent );

		//! Clears this menu.
		virtual void				clear( void );

		//! Adds new submenu.
		virtual IMenuWPtr			addMenu( const String& text );

		//! Adds new action to a menu.
		virtual IActionWPtr			addAction( const String& text, ActionCallback callback, const String& shortcut = String(), const String& icon = String(), s32 flags = 0 );
	
		//! Removes action from a menu.
		virtual void				removeAction( IActionWPtr action );

		//! Adds a separator item to a menu.
		virtual void				addSeparator( void );

		//! Sets the menu title.
		virtual void				setTitle( const String& value );

		//! Shows the menu at specified coordinates.
		virtual void				exec( s32 x, s32 y );
	
	private:

		QVector<IMenuPtr>			m_menues;	//!< All nested menues reside here.
		QVector<IActionPtr>			m_actions;	//!< All added actions reside here.
	};

} // namespace Ui

#endif	/*	!__DC_Composer_Menu_H__	*/