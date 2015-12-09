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

#include "Menu.h"
#include "SignalDelegate.h"

namespace Ui {

// ---------------------------------------------- ToolBar ----------------------------------------------- //

// ** ToolBar::ToolBar
ToolBar::ToolBar( QWidget* parent ) : UserInterface( new QToolBar( parent ) )
{

}

// ** ToolBar::addAction
void ToolBar::addAction( IActionWPtr action )
{
	m_private->addAction( action->privateInterface<QAction>() );
}

// ** ToolBar::removeAction
void ToolBar::removeAction( IActionWPtr action )
{
	m_private->removeAction( action->privateInterface<QAction>() );
}

// ** ToolBar::addSeparator
void ToolBar::addSeparator( void )
{
	m_private->addSeparator();
}

// ----------------------------------------------- Action ----------------------------------------------- //

// ** Action::Action
Action::Action( QWidget* parent, const String& text, ActionCallback callback ) : UserInterface( new QAction( text.c_str(), parent ) ), m_callback( callback )
{
	m_signal = new SignalDelegate( BindSignal( Action::triggered ), m_private.get(), SIGNAL(triggered()) );
}

// ** Action::Action
Action::Action( QAction* action ) : UserInterface( action )
{

}

// ** Action::triggered
void Action::triggered( void )
{
	m_callback( this );
}

// ** Action::isChecked
bool Action::isChecked( void ) const
{
	return m_private->isChecked();
}

// ** Action::setChecked
void Action::setChecked( bool value )
{
	m_private->setChecked( value );
}

// ** Action::setCheckable
void Action::setCheckable( bool value )
{
	m_private->setCheckable( value );
}

// ** Action::setDisabled
void Action::setDisabled( bool value )
{
	m_private->setDisabled( value );
}

// ** Action::isVisible
bool Action::isVisible( void ) const
{
	return m_private->isVisible();
}

// ** Action::setVisible
void Action::setVisible( bool value )
{
	m_private->setVisible( value );
}

// ** Action::text
String Action::text( void ) const
{
	return m_private->text().toStdString();
}

// ** Action::setText
void Action::setText( const String& value )
{
	m_private->setText( value.c_str() );
}

// ** Action::setShortcut
void Action::setShortcut( const String& value )
{
	m_private->setShortcut( QKeySequence( value.c_str() ) );
}

// ** Action::setIcon
void Action::setIcon( const String& value )
{
	m_private->setIcon( QIcon( value.c_str() ) );
}

// ------------------------------------------------ Menu ------------------------------------------------ //

// ** Menu::Menu
Menu::Menu( QWidget *parent ) : UserInterface( new QMenu( parent ) )
{
}

// ** Menu::clear
void Menu::clear( void )
{
	m_private->clear();
}

// ** Menu::size
s32 Menu::size( void ) const
{
	return m_private->actions().size();
}


// ** Menu::addMenu
IMenuWPtr Menu::addMenu( const String& text )
{
	Menu* menu = new Menu( m_private.get() );
	menu->setTitle( text.c_str() );
	m_private->addMenu( menu->privateInterface<QMenu>() );

	m_menues.append( menu );

	return menu;
}

// ** Menu::addAction
IActionWPtr Menu::addAction( const String& text, ActionCallback callback, const String& shortcut, const String& icon, s32 flags )
{
	Action* action = new Action( m_private.get(), text, callback );

	action->setDisabled( flags & ItemDisabled );
	action->setCheckable( flags & (ItemCheckable | ItemChecked) );
	action->setChecked( flags & ItemChecked );
	action->setVisible( (flags & ItemHidden) == 0 );
	action->setIcon( icon );
	action->setShortcut( shortcut );

	m_actions.append( action );

	m_private->QMenu::addAction( action->privateInterface<QAction>() );

	return action;
}

// ** Menu::removeAction
void Menu::removeAction( IActionWPtr action )
{
	s32 index = m_actions.indexOf( action );

	if( index == -1 ) {
		DC_BREAK;
		return;
	}

	m_actions.remove( index );
}

// ** Menu::addSeparator
void Menu::addSeparator( void )
{
	m_private->addSeparator();
}

// ** Menu::setTitle
void Menu::setTitle( const String& value )
{
	m_private->setTitle( value.c_str() );
}

// ** Menu::exec
void Menu::exec( s32 x, s32 y )
{
	m_private->exec( QPoint( x, y ) );
}

} // namespace Ui