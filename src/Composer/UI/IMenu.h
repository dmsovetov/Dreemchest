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

#define BindAction( target ) std::bind( &target, std::placeholders::_1 )
#define BindSignal( target ) std::bind( &target, this )

namespace Ui {

	//! Menu action flags
	enum ActionFlags {
		  ItemDisabled	= BIT( 0 )	//!< Menu action is disabled.
		, ItemCheckable	= BIT( 1 )	//!< Menu action is checkable.
		, ItemChecked	= BIT( 2 )	//!< Menu action is checked.
		, ItemHidden	= BIT( 3 )	//!< Menu action is hidden.
	};

	//! Public UI interface class.
	class IInterface : public RefCounted {
	public:

		virtual						~IInterface( void ) {}

		//! Returns the raw private implementation pointer.
		virtual void*				ptr( void ) const = 0;

		//! Returns the private interface.
		template<typename T>
		T*							privateInterface( void ) const { return reinterpret_cast<T*>( ptr() ); }
	};

	//! Menu action interface
	class IAction : public IInterface {
	public:

		virtual						~IAction( void ) {}

		//! Returns the checked state of a menu action.
		virtual bool				isChecked( void ) const				= 0;
	
		//! Sets the checked state of a menu action.
		virtual void				setChecked( bool value )			= 0;
	
		//! Returns the visibility state of a menu action.
		virtual bool				isVisible( void ) const				= 0;
	
		//! Sets the visibility state of a menu action.
		virtual void				setVisible( bool value )			= 0;
	
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

		virtual						~IToolBar( void ) {}

		//! Adds an action to a toolbar.
		virtual void				addAction( IActionWPtr action )		= 0;
	
		//! Removes an action from a toolbar.
		virtual void				removeAction( IActionWPtr action )	= 0;
	
		//! Adds a separator to a toolbar.
		virtual void				addSeparator( void )				= 0;
	};

	//! Menu interface
	class IMenu : public IInterface {
	public:

		virtual						~IMenu( void ) {}
	
		//! Adds new action to a menu.
		virtual IActionWPtr			addAction( const String& text, ActionCallback callback, const String& shortcut = String(), const String& icon = String(), s32 flags = 0 ) = 0;
	
		//! Removes action from a menu.
		virtual void				removeAction( IActionWPtr action ) = 0;

		//! Adds a separator item to a menu.
		virtual void				addSeparator( void ) = 0;

		//! Sets the menu title.
		virtual void				setTitle( const String& value ) = 0;
	};

} // namespace Ui

#endif	/*	!__DC_Composer_IMenu_H__	*/