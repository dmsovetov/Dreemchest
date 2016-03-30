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

#include "PropertyInspector.h"

#include "../Models/PropertyModel.h"
#include "../Models/EnumerationModel.h"

#include "Properties/VectorEdit.h"
#include "Properties/StringEdit.h"
#include "Properties/QuatEdit.h"
#include "Properties/EnumEdit.h"

#define DEV_MINIMUM_LABEL_SIZE  (50)

DC_BEGIN_COMPOSER

namespace Ui {

// ----------------------------------------------------------------------------- PropertyInspector::ItemDelegate ----------------------------------------------------------------------------- //

//! Item delegate type used by QDataWidgetMapper to bind properties to widgets.
class PropertyInspector::ItemDelegate : public QItemDelegate {
protected:

    //! Updates widget data with value stored inside an object property.
    virtual void	setEditorData( QWidget* editor, const QModelIndex& index ) const Q_DECL_OVERRIDE;

    //! Updates property value with data received from a widget.
    virtual void	setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const Q_DECL_OVERRIDE;
};

// ** PropertyInspector::ItemDelegate::setEditorData
void PropertyInspector::ItemDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
    QVariant   value        = index.data( Qt::EditRole );
    QByteArray userProperty = editor->metaObject()->userProperty().name();

    if( userProperty.isEmpty() ) {
        LogError( "propertyInspector", "widget %s has no user proprty to write data\n", editor->metaObject()->className() );
        return;
    }

    if( !value.isValid() ) {
        LogError( "propertyInspector", "property model returned an invalid value\n" );
        return;
    }

    editor->setProperty( userProperty, value );
}

// ** PropertyInspector::ItemDelegate::setModelData
void PropertyInspector::ItemDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
    QByteArray userProperty = editor->metaObject()->userProperty().name();

    if( userProperty.isEmpty() ) {
        LogError( "propertyInspector", "widget %s has no user proprty to read data\n", editor->metaObject()->className() );
        return;
    }

    model->setData( index, editor->property( userProperty ), Qt::EditRole );
}

// ------------------------------------------------------------------------------------ PropertyInspector ------------------------------------------------------------------------------------ //

// ** PropertyInspector::s_factory
PropertyInspector::WidgetFactory PropertyInspector::s_factory;

// ** PropertyInspector::PropertyInspector
PropertyInspector::PropertyInspector( QWidget* parent )
    : QWidget( parent )
    , m_layout( NULL )
    , m_model( NULL )
    , m_mapper( NULL )
{

}

// ** PropertyInspector::setModel
void PropertyInspector::setModel( PropertyModelQPtr value )
{
    delete m_model;
    m_model = value;
    mapModelToWidgets();
}

// ** PropertyInspector::setMargins
void PropertyInspector::setMargins( int left, int top, int right, int bottom )
{
    m_layout->setContentsMargins( QMargins( left, top, right, bottom ) );
}

// ** PropertyInspector::refresh
void PropertyInspector::refresh( void )
{
    m_mapper->revert();
}

// ** PropertyInspector::mapModelToWidgets
void PropertyInspector::mapModelToWidgets( void )
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
    m_mapper->setItemDelegate( new ItemDelegate );

	// Create root layout.
	m_layout = new QFormLayout( this );
    m_layout->setSpacing( 2 );

    // Widget mapper index
    int index = 0;

	// Construct property widgets.
	foreach( PropertyModel::Property* property, m_model->properties() ) {
        // Increase the widget mapper index
        index++;

        // Get the property info
        const Reflection::PropertyInfo& info = property->info();

		// Get the property type & name
		QString name = property->name();
        name[0] = name[0].toUpper();

        QWidget* widget = NULL;
        const Reflection::MetaObject* metaObject = property->metaObject();

        if( metaObject && metaObject->isEnum() ) {
            // Construct combo box with enumeration model
            widget = new EnumEdit( metaObject->isEnum() );
        } else {
            // Construct widget by a type name
            widget = s_factory.construct( property->type() );
        }

        if( !widget ) {
            LogWarning( "propertyInspector", "no widget registered for properties of type '%s'\n", property->type()->name() );
            continue;
        }

        // Connect to a widget's valueChanged signal
        connect( widget, SIGNAL(valueChanged(const Variant&)), m_mapper, SLOT(submit()) );

        // Add widget to layout and mapper
		m_layout->addRow( name, widget );
		m_mapper->addMapping( widget, index - 1 );

        // Set the minimum size for a label widget
        QWidget* label = m_layout->labelForField( widget );
        label->setMinimumWidth( DEV_MINIMUM_LABEL_SIZE );

        // Set the property tooltip
        if( info.description ) {
            label->setToolTip( info.description );
        }
	}

	// Finish construction
	m_mapper->toFirst();
	m_mapper->setSubmitPolicy( QDataWidgetMapper::AutoSubmit );
}

} // namespace Ui

DC_END_COMPOSER