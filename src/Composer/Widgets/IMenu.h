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

#ifndef __DC_Composer_IMenu_H__
#define __DC_Composer_IMenu_H__

#include "../Composer.h"

#define BindAction( target ) std::bind( &target, this, std::placeholders::_1 )
#define BindSignal( target ) std::bind( &target, this )

DC_BEGIN_COMPOSER

namespace Ui {

	//! Menu action flags
	enum ActionFlags {
		  ItemDisabled	= BIT( 0 )	//!< Menu action is disabled.
		, ItemCheckable	= BIT( 1 )	//!< Menu action is checkable.
		, ItemChecked	= BIT( 2 )	//!< Menu action is checked.
		, ItemHidden	= BIT( 3 )	//!< Menu action is hidden.
	};

	//! Menu action interface
	class IAction : public IInterface {
	public:

		//! Returns the checked state of a menu action.
		virtual bool				isChecked( void ) const				= 0;
	
		//! Sets the checked state of a menu action.
		virtual void				setChecked( bool value )			= 0;
	
		//! Returns the visibility state of a menu action.
		virtual bool				isVisible( void ) const				= 0;
	
		//! Sets the visibility state of a menu action.
		virtual void				setVisible( bool value )			= 0;

		//! Sets the disabled state of a menu action.
		virtual void				setDisabled( bool value )			= 0;
	
		//! Returns the menu action text.
		virtual String				text( void ) const					= 0;
	
		//! Sets the menu action text.
		virtual void				setText( const String& value )		= 0;
	
		//! Sets the menu action shortcut.
		virtual void				setShortcut( const String& value )	= 0;
	
		//! Sets the menu action icon.
		virtual void				setIcon( const String& value )		= 0;
	};

	//! Toolbar interface.
	class IToolBar : public IInterface {
	public:

		//! Adds an action to a tool bar.
		virtual void				addAction( IActionWPtr action )		= 0;

		//! Creates new action owned by this tool bar.
		virtual IActionWPtr			addAction( const String& text, ActionCallback callback, const String& shortcut = String(), const String& icon = String(), s32 flags = 0 ) = 0;

		//! Begins the action group construction.
		virtual void				beginActionGroup( void ) = 0;

		//! Ends the action group construction.
		virtual void				endActionGroup( void ) = 0;

		//! Removes an action from a tool bar.
		virtual void				removeAction( IActionWPtr action )	= 0;
	
		//! Adds a separator to a tool bar.
		virtual void				addSeparator( void )				= 0;
	};

	//! Menu interface
	class IMenu : public IInterface {
	public:

		//! Clears this menu.
		virtual void				clear( void ) = 0;

		//! Returns the total number of items inside the menu.
		virtual s32					size( void ) const = 0;

		//! Adds new submenu.
		virtual IMenuWPtr			addMenu( const String& text ) = 0;

		//! Adds new action to a menu.
		virtual IActionWPtr			addAction( const String& text, ActionCallback callback, const String& shortcut = String(), const String& icon = String(), s32 flags = 0 ) = 0;
	
		//! Removes action from a menu.
		virtual void				removeAction( IActionWPtr action ) = 0;

		//! Adds a separator item to a menu.
		virtual void				addSeparator( void ) = 0;

		//! Sets the menu title.
		virtual void				setTitle( const String& value ) = 0;

		//! Shows the menu at specified coordinates.
		virtual void				exec( s32 x, s32 y ) = 0;
	};

} // namespace Ui

DC_END_COMPOSER

#endif	/*	!__DC_Composer_IMenu_H__	*/