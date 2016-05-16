// shadertype=glsl

[Features]
F_VertexColor	= vertexColor
F_TexCoord		= vertexTexCoord0
F_Texture		= texture0
F_Material		= cbuffer3

[VertexShader]
#if defined( F_VertexColor )
	varying vec4 v_Color;
#endif  //	F_VertexColor

#if defined( F_TexCoord )
	varying vec2 v_TexCoord0;
#endif	//	F_TexCoord

void main()
{
	gl_Position  = View.transform * gl_Vertex;

#if defined( F_VertexColor )
	v_Color = gl_Color;
#endif  //	F_VertexColor

#if defined( F_TexCoord )
	v_TexCoord0 = gl_MultiTexCoord0.xy;
#endif	//	F_TexCoord
}     

[FragmentShader]
#if defined( F_VertexColor )
	varying vec4 v_Color;
#endif  //	F_VertexColor

#if defined( F_TexCoord )
	varying vec2 v_TexCoord0;
#endif	//	F_TexCoord

#if defined( F_Texture )
	uniform sampler2D Texture0;
#endif	//	F_Texture

void main()
{
	vec4 diffuseColor = vec4( 1.0, 1.0, 1.0, 1.0 );

#if defined( F_Material )
	diffuseColor *= Material.diffuse;
#endif	//	F_Material

#if defined( F_VertexColor )
	diffuseColor *= v_Color;
#endif  //	F_VertexColor

#if defined( F_Texture ) && defined( F_TexCoord )
	diffuseColor *= texture( Texture0, v_TexCoord0 );
#endif	//	F_TexCoord

	gl_FragColor = diffuseColor;
}