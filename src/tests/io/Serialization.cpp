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

#include <gtest/gtest.h>
#include <io/Io.h>
#include <math/Vec2.h>

#define DEFAULT_POD_VALUE( T, V ) template<> T defaultValue( void ) { return V; }

DC_USE_DREEMCHEST

using namespace io;

template<typename T>
T defaultValue( void ) { T value; value.set(); return value; }

DEFAULT_POD_VALUE( u8,   200   )
DEFAULT_POD_VALUE( s8,  -100   )
DEFAULT_POD_VALUE( u16,  60000 )
DEFAULT_POD_VALUE( s16, -30000 )
DEFAULT_POD_VALUE( u32,  6000000 )
DEFAULT_POD_VALUE( s32, -3000000 )
DEFAULT_POD_VALUE( u64,  600000000 )
DEFAULT_POD_VALUE( s64, -300000000 )
DEFAULT_POD_VALUE( f32, -125.00132f )
DEFAULT_POD_VALUE( f64, 121112.3656 )
DEFAULT_POD_VALUE( String, "hello world" );

template<typename T>
struct Value : public Serializable {
            Value( void ) {}

    void    set( void ) { m_value = defaultValue<T>(); }

    bool    operator == ( const Value& other ) const { return m_value == other.m_value; }
    bool    operator != ( const Value& other ) const { return m_value != other.m_value; }

    T       m_value;

    IoBeginSerializer
        IoField( m_value )
    IoEndSerializer
};

template<typename T>
bool arraysEqual( const T& a, const T& b )
{
    if( a.size() != b.size() ) {
        return false;
    }

    for( int i = 0; i < a.size(); i++ ) {
        if( a[i] != b[i] ) {
            return false;
        }
    }

    return true;
}

template<typename T>
struct ArrayOf : public Serializable {
                ArrayOf( void ) {}

    void        set() { int size = 10; while( size-- ) { m_value.push_back( defaultValue<T>() ); } }

    bool        operator == ( const ArrayOf& other ) const { return  arraysEqual( m_value, other.m_value ); }
    bool        operator != ( const ArrayOf& other ) const { return !arraysEqual( m_value, other.m_value ); }

    Array<T>    m_value;

    IoBeginSerializer
        IoArray( m_value )
    IoEndSerializer
};

template<typename T>
struct CompoundValue : public Serializable {

    void            set() { m_value1.set(); m_value2.set(); m_value3.set(); m_array1.set(); m_array2.set(); m_array3.set(); }

    bool            operator == ( const CompoundValue& other ) const {
        return m_value1 == other.m_value1 && m_value2 == other.m_value2 && m_value3 == other.m_value3 && m_array1 == other.m_array1 && m_array2 == other.m_array2 && m_array3 == other.m_array3;
    }
    bool            operator != ( const CompoundValue& other ) const {
        return m_value1 != other.m_value1 && m_value2 != other.m_value2 && m_value3 != other.m_value3 && m_array1 != other.m_array1 && m_array2 != other.m_array2 && m_array3 != other.m_array3;
    }

    ArrayOf<T>   m_array1;
    ArrayOf<T>   m_array2;
    ArrayOf<T>   m_array3;

    Value<T>     m_value1;
    Value<T>     m_value2;
    Value<T>     m_value3;

    IoBeginSerializer
        IoField( m_array1 );
        IoField( m_array2 );
        IoField( m_array3 );
        IoField( m_value1 )
        IoField( m_value2 )
        IoField( m_value3 )
    IoEndSerializer
};

template<typename T>
class Serialization : public testing::Test {
public:

    void SetUp()
    {
        buffer = ByteBuffer::create();
    }
    void TearDown()
    {
        buffer = ByteBufferPtr();
    }

    DiskFileSystem fs;
    ByteBufferPtr  buffer;
    T              source, dest;
};

struct Vector : public Serializable {
    float x, y, z;

    void set() { x = -12.034f, y = 11.023f, z = 1.1f; }

    bool operator == ( const Vector& other ) const { return x == other.x && y == other.y && z == other.z; }
    bool operator != ( const Vector& other ) const { return x != other.x || y != other.y || z != other.z; }

    IoBeginSerializer
        IoField( x )
        IoField( y )
        IoField( z )
    IoEndSerializer
};

struct Mesh : public Serializable {
    String name;
    Vector position;
    Vector scale;

    void set() { name = "hello"; position.set(); scale.set(); }

    bool operator == ( const Mesh& other ) const { return position == other.position && scale == other.scale && name == other.name; }
    bool operator != ( const Mesh& other ) const { return position != other.position || scale != other.scale || name != other.name; }

    IoBeginSerializer
        IoField( name )
        IoField( position )
        IoField( scale )
    IoEndSerializer
};

struct SceneSettings : public Serializable {
    Vector fogColor;
    String name;

    void set() { name = "sceneSettings", fogColor.set(); }

    bool operator == ( const SceneSettings& other ) const { return fogColor == other.fogColor && name == other.name; }
    bool operator != ( const SceneSettings& other ) const { return fogColor != other.fogColor || name != other.name; }

    IoBeginSerializer
        IoField( fogColor )
        IoField( name )
    IoEndSerializer
};

struct Scene : public Serializable {
    String      name;
    SceneSettings settings;
    Array<Mesh> staticMeshes;
    Array<Mesh> dynamicMeshes;

    void set() {
        name = "scene", settings.set(), staticMeshes.resize( 50 ), dynamicMeshes.resize( 20 );
        for( int i = 0; i < staticMeshes.size(); i++ ) staticMeshes[i].set();
        for( int i = 0; i < dynamicMeshes.size(); i++ ) dynamicMeshes[i].set();
    }

    bool operator == ( const Scene& other ) const { return name == other.name && settings == other.settings &&  arraysEqual( dynamicMeshes, other.dynamicMeshes ) &&  arraysEqual( staticMeshes, other.staticMeshes ); }
    bool operator != ( const Scene& other ) const { return name != other.name || settings != other.settings || !arraysEqual( dynamicMeshes, other.dynamicMeshes ) || !arraysEqual( staticMeshes, other.staticMeshes ); }

    IoBeginSerializer
        IoField( settings )
        IoField( name )
        IoArray( staticMeshes )
        IoArray( dynamicMeshes )
    IoEndSerializer
};

typedef testing::Types<
    // POD types
      Value<u8>
    , Value<s8>
    , Value<u16>
    , Value<s16>
    , Value<u32>
    , Value<s32>
    , Value<u64>
    , Value<s64>
    , Value<f32>
    , Value<f64>
    , Value<String>

    // Arrays of POD types
    , ArrayOf<u8>
    , ArrayOf<s8>
    , ArrayOf<u16>
    , ArrayOf<s16>
    , ArrayOf<u32>
    , ArrayOf<s32>
    , ArrayOf<u64>
    , ArrayOf<s64>
    , ArrayOf<f32>
    , ArrayOf<f64>
    , ArrayOf<String>

    // Compound values
    , CompoundValue<u8>
    , CompoundValue<s8>
    , CompoundValue<u16>
    , CompoundValue<s16>
    , CompoundValue<u32>
    , CompoundValue<s32>
    , CompoundValue<u64>
    , CompoundValue<s64>
    , CompoundValue<f32>
    , CompoundValue<f64>
    , CompoundValue<String>

    // Complex objects
    , Vector
    , Mesh
    , SceneSettings
    , Scene

    // Array of complex objects
    , ArrayOf<Vector>
    , ArrayOf<Mesh>
    , ArrayOf<SceneSettings>
    , ArrayOf<Scene>

    // Compound values of objects
    , CompoundValue<Vector>
    , CompoundValue<Mesh>
    , CompoundValue<SceneSettings>
    , CompoundValue<Scene>
> SerializableTypes;

TYPED_TEST_CASE(Serialization, SerializableTypes);

TYPED_TEST(Serialization, Binary)
{
    this->source.set();

    {
        int pos = this->buffer->position();

        Storage storage( StorageBinary, this->buffer );
        ASSERT_EQ(true, storage);
        this->source.write( storage );
        ASSERT_NE(pos, this->buffer->position());
    }

    {
        this->buffer->setPosition(0);
        Storage storage( StorageBinary, this->buffer );
        ASSERT_EQ(true, storage);
        ASSERT_NE(this->source, this->dest);
        this->dest.read( storage );
        ASSERT_EQ(this->source, this->dest);
    }
}