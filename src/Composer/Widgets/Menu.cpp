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

DC_BEGIN_COMPOSER

namespace Ui {

// ---------------------------------------------- ToolBar ----------------------------------------------- //

// ** ToolBar::ToolBar
ToolBar::ToolBar( QWidget* parent ) : QToolBar( parent )
{

}

// ** ToolBar::addAction
void ToolBar::addAction( ActionQPtr action )
{
	QToolButton* button = new QToolButton( this );
	button->setDefaultAction( action );
	button->setMaximumSize( QSize( 24, 24 ) );

	addWidget( button );
}

// ** ToolBar::addAction
ActionQPtr ToolBar::addAction( const String& text, ActionCallback callback, const String& shortcut, const String& icon, s32 flags )
{
	ActionQPtr action = Action::create( text, callback, shortcut, icon, flags );
	m_group.append( action );

	addAction( action );

	return action;
}

// ** ToolBar::beginActionGroup
void ToolBar::beginActionGroup( void )
{
	m_group.clear();
}

// ** ToolBar::endActionGroup
void ToolBar::endActionGroup( void )
{
	DC_BREAK_IF( m_group.empty() );

	QActionGroup* group = new QActionGroup( this );

	foreach( ActionQPtr action, m_group ) {
		group->addAction( action );
	}

    m_group.clear();
}

// ----------------------------------------------- Action ----------------------------------------------- //

// ** Action::Action
Action::Action( QWidget* parent, const String& text, ActionCallback callback ) : QAction( text.c_str(), parent ), m_callback( callback )
{
    connect(this, SIGNAL(triggered()), this, SLOT(triggered()));
}

// ** Action::triggered
void Action::triggered( void )
{
	m_callback( this );
}

// ** Action::setShortcut
void Action::setShortcut( const String& value )
{
	QAction::setShortcut( QKeySequence( value.c_str() ) );
}

// ** Action::setIcon
void Action::setIcon( const String& value )
{
	QAction::setIcon( QIcon( value.c_str() ) );
}

// ** Action::create
ActionQPtr Action::create( const String& text, ActionCallback callback, const String& shortcut, const String& icon, s32 flags )
{
	Action* action = new Action( NULL, text, callback );

	action->setDisabled( flags & ItemDisabled );
	action->setCheckable( flags & (ItemCheckable | ItemChecked) );
	action->setChecked( flags & ItemChecked );
	action->setVisible( (flags & ItemHidden) == 0 );
	action->setIcon( icon );
	action->setShortcut( shortcut );

	return action;
}

// ------------------------------------------------ Menu ------------------------------------------------ //

// ** Menu::Menu
Menu::Menu( QWidget *parent ) : QMenu( parent )
{
}

// ** Menu::size
s32 Menu::size( void ) const
{
	return actions().size();
}

// ** Menu::addMenu
MenuQPtr Menu::addMenu( const String& text )
{
	Menu* menu = new Menu( this );
	menu->setTitle( text.c_str() );
	QMenu::addMenu( menu );

	return menu;
}

// ** Menu::addAction
ActionQPtr Menu::addAction( const String& text, ActionCallback callback, const String& shortcut, const String& icon, s32 flags )
{
	ActionQPtr action = Action::create( text, callback, shortcut, icon, flags );
	QMenu::addAction( action );
	return action;
}

} // namespace Ui

DC_END_COMPOSER