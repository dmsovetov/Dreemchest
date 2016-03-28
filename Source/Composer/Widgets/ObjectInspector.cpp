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

#include "ObjectInspector.h"

#include "../Models/PropertyModel.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ** ObjectInspector::s_factory
ObjectInspector::WidgetFactory ObjectInspector::s_factory;

// ** ObjectInspector::ObjectInspector
ObjectInspector::ObjectInspector( QWidget* parent ) : QWidget( parent ), m_layout( NULL ), m_model( NULL ), m_mapper( NULL )
{

}

// ** ObjectInspector::setModel
void ObjectInspector::setModel( PropertyModelQPtr value )
{
    delete m_model;
    m_model = value;
    mapModelToWidgets();
}

// ** ObjectInspector::setMargins
void ObjectInspector::setMargins( int left, int top, int right, int bottom )
{
    m_layout->setContentsMargins( QMargins( left, top, right, bottom ) );
}

// ** ObjectInspector::refresh
void ObjectInspector::refresh( void )
{
    m_mapper->revert();
}

// ** ObjectInspector::mapModelToWidgets
void ObjectInspector::mapModelToWidgets( void )
{
	// Destroy old layout & mapper
	qDeleteLayout( m_layout ); m_layout = NULL;
	delete m_mapper; m_mapper = NULL;

	// Return if the model is not valid
	if( !m_model ) {
		return;
	}

	// Create the mapper.
	m_mapper = new QDataWidgetMapper( this );
	m_mapper->setModel( m_model );

	// Create root layout.
	m_layout = new QFormLayout( this );
    m_layout->setSpacing( 2 );

    // Widget mapper index
    int index = 0;

#if 0
	// Construct property widgets.
	foreach( Introspection::Property* property, m_model->properties() ) {
        // Increase the widget mapper index
        index++;

		// Get the property type & name
		QString widget = property->widget();
		QString name   = property->name();

        // Construct widget by a type name
        QWidget* instance = s_factory.construct( widget );

        if( !instance ) {
			qWarning() << "Property" << name << "has unhnadled widget" << widget;
			continue;            
        }

        // Add widget to layout and mapper
		m_layout->addRow( name, instance );
		m_mapper->addMapping( instance, index - 1 );
	}
#endif

	// Finish construction
	m_mapper->toFirst();
	m_mapper->setSubmitPolicy( QDataWidgetMapper::AutoSubmit );
}

} // namespace Ui

DC_END_COMPOSER