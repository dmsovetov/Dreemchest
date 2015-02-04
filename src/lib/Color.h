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

#ifndef		__DC_Color_H__
#define		__DC_Color_H__

DC_BEGIN_DREEMCHEST

	// ** enum GlobalColor
	enum GlobalColor {
		Transparent,
		Black,
		White,
		Red,
		Green,
		Blue,
        Cyan,
        Magenta,
        Yellow,
	};

	// ** class Rgba
	class Rgba {
	public:

		f32             r, g, b, a;

	public:

                        Rgba( void );
                        Rgba( u32 r, u32 g, u32 b, u32 a = 255 );
                        Rgba( f32 r, f32 g, f32 b, f32 a = 1.0f );
                        Rgba( const Rgba& other );
                        Rgba( u32 rgba );
						Rgba( GlobalColor color );

		bool            operator == ( const Rgba& other ) const;
		bool            operator != ( const Rgba& other ) const;
		f32             operator [] ( u32 index ) const;
		f32&            operator [] ( u32 index );
		Rgba            operator *  ( f32 scalar ) const;
        const Rgba&     operator *= ( f32 scalar );
		Rgba            operator *  ( const Rgba& other ) const;
        const Rgba&     operator *= ( const Rgba& other );
		Rgba            operator +  ( const Rgba& other ) const;
        const Rgba&     operator += ( const Rgba& other );

        u32             toInt( void ) const;
        void            setRgba( f32 r, f32 g, f32 b, f32 a );

		static Rgba     lerp( const Rgba& a, const Rgba& b, f32 scalar );
		static Rgba     random( void );
	};

    // ** Rgba::Rgba
    inline Rgba::Rgba( void ) : r( 1.0f ), g( 1.0f ), b( 1.0f ), a( 1.0f )
    {
    }

    inline Rgba::Rgba( u32 R, u32 G, u32 B, u32 A )
    {
        r = R / 255.0f; g = G / 255.0f; b = B / 255.0f; a = A / 255.0f;
    }

    inline Rgba::Rgba( f32 R, f32 G, f32 B, f32 A )
    {
        r = R;
        g = G;
        b = B;
        a = A;
    }

    inline Rgba::Rgba( const Rgba& other )
    {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
    }

    inline Rgba::Rgba( GlobalColor color )
    {
        switch( color ) {
		case Transparent:	setRgba( 0.0f, 0.0f, 0.0f, 0.0f ); break;
        case Black:			setRgba( 0.0f, 0.0f, 0.0f, 1.0f ); break;
		case White:			setRgba( 1.0f, 1.0f, 1.0f, 1.0f ); break;
		case Red:			setRgba( 1.0f, 0.0f, 0.0f, 1.0f ); break;
		case Green:			setRgba( 0.0f, 1.0f, 0.0f, 1.0f ); break;
		case Blue:			setRgba( 0.0f, 0.0f, 1.0f, 1.0f ); break;
        case Cyan:			setRgba( 0.0f, 1.0f, 1.0f, 1.0f ); break;
        case Magenta:		setRgba( 1.0f, 0.0f, 1.0f, 1.0f ); break;
        case Yellow:		setRgba( 1.0f, 1.0f, 0.0f, 1.0f ); break;
		}
    }

    // ** Rgba::toInt
    inline unsigned int Rgba::toInt( void ) const
    {
        unsigned int c = 0;

        c = c | (( u32 )( u32(r * 255) & 0xff ) << 24);
        c = c | (( u32 )( u32(g * 255) & 0xff ) << 16);
        c = c | (( u32 )( u32(b * 255) & 0xff ) << 8 );
        c = c | (( u32 )( u32(a * 255) & 0xff ) << 0 );

        return c;
    }

    // ** Rgba::setRgba
    inline void Rgba::setRgba( f32 r, f32 g, f32 b, f32 a )
    {
        this->r = r; this->g = g; this->b = b; this->a = a;
    }

    // ** Rgba::operator ==
    inline bool Rgba::operator == ( const Rgba& other ) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    // Rgba::operator !=
    inline bool Rgba::operator != ( const Rgba& other ) const
    {
        return r != other.r || g != other.g || b != other.b || a != other.a;
    }

    // ** Rgba::operator []
    inline f32 Rgba::operator [] ( u32 index ) const
    {
        switch( index ) {
        case 0: return r;
        case 1: return g;
        case 2: return b;
        case 3: return a;
        }

        return 0;
    }

    // ** Rgba::operator []
    inline f32& Rgba::operator [] ( u32 index )
    {
        switch( index ) {
        case 0: return r;
        case 1: return g;
        case 2: return b;
        case 3: return a;
        }

        return r;
    }

    // ** Rgba::operator *
    inline Rgba Rgba::operator * ( float scalar ) const
    {
        return Rgba( r * scalar, g * scalar, b * scalar, a * scalar );
    }

    // ** Rgba::operator *
    inline Rgba Rgba::operator * ( const Rgba& c ) const
    {
        return Rgba( r * c.r, g * c.g, b * c.b, a * c.a );
    }

    // ** Rgba::operator +
    inline Rgba Rgba::operator + ( const Rgba& c ) const
    {
        return Rgba( r + c.r, g + c.g, b + c.b, a + c.a );
    }

    // ** Rgba::lerp
    inline Rgba Rgba::lerp( const Rgba& a, const Rgba& b, float scalar )
    {
        Rgba result;
        
        result.r = (1.0f - scalar) * a.r + scalar * b.r;
        result.g = (1.0f - scalar) * a.g + scalar * b.g;
        result.b = (1.0f - scalar) * a.b + scalar * b.b;
        result.a = (1.0f - scalar) * a.a + scalar * b.a;

        return result;
    }

    // ** Rgba::random
    inline Rgba Rgba::random( void )
    {
        return Rgba( ( u32 )(rand() % 255), (rand() % 255), (rand() % 255), 255 );
    }

    // ** class Rgb
    class Rgb {
    public:

                    Rgb( void );
                    Rgb( GlobalColor color );
                    Rgb( f32 r, f32 g, f32 b );
                    Rgb( const f32* rgb );
                    Rgb( const Rgb& other );

        Rgb operator + ( const Rgb& other ) const;
        Rgb operator * ( const Rgb& other ) const;
        Rgb operator * ( float scalar ) const;

    public:

        f32       r, g, b;
    };

    // ** Rgb::Rgb
    inline Rgb::Rgb( void )
    {
        r = g = b = 1.0f;
    }

    // ** Rgb::Rgb
    inline Rgb::Rgb( GlobalColor color )
    {
        switch( color ) {
        case Transparent:	r = 0.0f, g = 0.0f, b = 0.0f; break;
        case Black:			r = 0.0f, g = 0.0f, b = 0.0f; break;
        case White:			r = 1.0f, g = 1.0f, b = 1.0f; break;
        case Red:			r = 1.0f, g = 0.0f, b = 0.0f; break;
        case Green:			r = 0.0f, g = 1.0f, b = 0.0f; break;
        case Blue:			r = 0.0f, g = 0.0f, b = 1.0f; break;
        case Cyan:			r = 0.0f, g = 1.0f, b = 1.0f; break;
        case Magenta:		r = 1.0f, g = 0.0f, b = 1.0f; break;
        case Yellow:		r = 1.0f, g = 1.0f, b = 0.0f; break;
		}
    }

    // ** Rgb::Rgb
    inline Rgb::Rgb( float r, float g, float b )
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    // ** Rgb::Rgb
    inline Rgb::Rgb( const float *rgb )
    {
        r = rgb[0];
        g = rgb[1];
        b = rgb[2];
    }

    // ** Rgb::Rgb
    inline Rgb::Rgb( const Rgb& other )
    {
        r = other.r;
        g = other.g;
        b = other.b;
    }

    // ** Rgb::operator +
    inline Rgb Rgb::operator + ( const Rgb& other ) const
    {
        return Rgb( r + other.r, g + other.g, b + other.b );
    }

    // ** Rgb::operator *
    inline Rgb Rgb::operator * ( const Rgb& other ) const
    {
        return Rgb( r * other.r, g * other.g, b * other.b );
    }
    
    // ** Rgb::operator *
    inline Rgb Rgb::operator * ( float scalar ) const
    {
        return Rgb( r * scalar, g * scalar, b * scalar );
    }

DC_END_DREEMCHEST

#endif		/*	!__DC_Color_H__	*/