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

#include "ObjectInspectorPrivate.h"

#include "AssetTree.h"

#include "../../Models/PropertyModel.h"
#include "../../Models/EnumModels.h"

DC_BEGIN_COMPOSER

namespace Ui {

void qDeleteLayout( QLayout* layout )
{
	if( !layout ) {
		return;
	}

	QLayoutItem* child = NULL;

	while( (child = layout->takeAt(0)) != 0 ) {
		delete child->widget();
		delete child;
	}

	delete layout;
}

// ** QObjectInspector::QObjectInspector
QObjectInspector::QObjectInspector( QWidget* parent ) : QWidget( parent ), m_layout( NULL ), m_mapper( NULL )
{

}

// ** QObjectInspector::setModel
void QObjectInspector::setModel( PropertyModelWPtr value )
{
	// Set the model
	m_model = value;

	// Destroy old layout & mapper
	qDeleteLayout( m_layout ); m_layout = NULL;
	delete m_mapper; m_mapper = NULL;

	// Return if the model is not valid
	if( value.isNull() ) {
		return;
	}

	// Create the mapper.
	m_mapper = new QDataWidgetMapper( this );
	m_mapper->setModel( m_model.data() );

	// Create root layout.
	m_layout = new QFormLayout( this );

	// Construct property widgets.
	for( s32 i = 0, n = m_model->propertyCount(); i < n; i++ ) {
		// Get the property type & name
		QString type = m_model->propertyType( i );
		QString name = m_model->propertyName( i );

		// Construct the widget according to the type
		QWidget* widget = NULL;

		if( type == "Scene::ImageWPtr" ) {
			widget = new QAssetSelector( Scene::Asset::Image );
			connect( widget, SIGNAL(valueChanged()), m_mapper, SLOT(submit()) );
		}
		else if( type == "Scene::RenderingMode" ) {
			widget = new QEnumComboBox<RenderingModeModel>();
		}
		else if( type == "Scene::Material::Model" ) {
			widget = new QEnumComboBox<LightingModel>();
		}
		else {
			qWarning() << "Property" << name << "has unhnadled type" << type;
			continue;
		}

		// Add widget
		m_layout->addRow( name, widget );
		m_mapper->addMapping( widget, i );
	}

	// Finish construction
	m_mapper->toFirst();
	m_mapper->setSubmitPolicy( QDataWidgetMapper::AutoSubmit );
}

// ** ObjectInspectorPrivate::ObjectInspectorPrivate
ObjectInspectorPrivate::ObjectInspectorPrivate( QWidget* parent ) : PrivateInterface( new QObjectInspector( parent ) )
{

}

// ** ObjectInspectorPrivate::setModel
void ObjectInspectorPrivate::setModel( PropertyModelWPtr value )
{
	m_private->setModel( value );
}

} // namespace Ui

DC_END_COMPOSER