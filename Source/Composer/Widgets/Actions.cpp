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

#include "Actions.h"

DC_BEGIN_COMPOSER

namespace Ui {

// --------------------------------------------------------- BaseAction --------------------------------------------------------- //

// ** BaseAction::BaseAction
BaseAction::BaseAction( QWidget* parent )
    : QAction( parent )
{
    connect( this, SIGNAL(triggered()), this, SLOT(triggered()) );
}

// -------------------------------------------------------- EntityAction -------------------------------------------------------- //

// ** EntityAction::EntityAction
EntityAction::EntityAction( Ecs::EntityWPtr entity, QWidget* parent )
    : BaseAction( parent )
    , m_entity( entity )
{

}

// ----------------------------------------------------- DetachComponentAction -------------------------------------------------- //

// ** DetachComponentAction::DetachComponentAction
DetachComponentAction::DetachComponentAction( Ecs::EntityWPtr entity, TypeIdx component, QWidget* parent )
    : EntityAction( entity, parent )
    , m_component( component )
{

}

// ** DetachComponentAction::triggered
void DetachComponentAction::triggered( void )
{
    m_entity->detachById( m_component );
}

} // namespace Ui

DC_END_COMPOSER