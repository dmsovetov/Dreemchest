// shadertype=glsl

[Features]
F_VertexColor  		= vertexColor
F_DiffuseTexture	= texture0

[VertexShader]
#if defined( F_VertexColor )
	varying vec4 v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_DiffuseTexture )
	varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

void main()
{
	gl_Position  = View.transform * gl_Vertex;

#if defined( F_VertexColor )
	v_Color = gl_Color;
#endif  /*  F_VertexColor    */

#if defined( F_DiffuseTexture )
	v_TexCoord0 = gl_MultiTexCoord0.xy;
#endif	/*	F_DiffuseTexture	*/
}     

[FragmentShader]
#if defined( F_VertexColor )
	varying vec4 v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_DiffuseTexture )
	uniform sampler2D	u_DiffuseTexture;
	varying vec2		v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

void main()
{
	vec4 diffuseColor = Material.diffuse;

#if defined( F_VertexColor )
	diffuseColor *= v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_DiffuseTexture )
	diffuseColor *= texture2D( u_DiffuseTexture, v_TexCoord0 );
#endif	/*	F_DiffuseTexture	*/

	gl_FragColor = diffuseColor;
}