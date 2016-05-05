// shadertype=glsl

[Features]
F_VertexNormal 		= vertexNormal
F_VertexColor  		= vertexColor
F_AmbientColor	  	= ambientColor
F_EmissionColor	  	= emissionColor
F_DiffuseTexture	= texture0
F_TestTexture		= texture1

[VertexShader]
#if defined( F_VertexColor )
varying vec4 v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_DiffuseTexture )
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

//#if defined( F_TestTexture )
//varying vec2 v_TexCoordTest;
//#endif	/*	F_TestTexture	*/

#if defined( F_VertexNormal )
varying vec3 v_Normal;
#endif  /*  F_VertexNormal    */

void main()
{
	vec4 vertex 	= Instance.transform * gl_Vertex;
	
	gl_Position     = View.transform * vertex;
	gl_PointSize    = 5;
#if defined( F_VertexColor )
	v_Color         = gl_Color;
#endif  /*  F_VertexColor    */

#if defined( F_VertexNormal )
	v_Normal        = gl_Normal;
#endif  /*  F_VertexNormal    */

#if defined( F_DiffuseTexture )
	v_TexCoord0		= gl_MultiTexCoord0.xy;
#endif	/*	F_DiffuseTexture	*/

//#if defined( F_TestTexture )
//	v_TexCoordTest	= gl_MultiTexCoord0.xy;
//#endif	/*	F_TestTexture	*/
}     

[FragmentShader]
#if defined( F_VertexColor )
varying vec4 v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_VertexNormal )
varying vec3 v_Normal;
#endif  /*  F_VertexNormal    */

#if defined( F_DiffuseTexture )
uniform sampler2D u_DiffuseTexture;
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

//#if defined( F_TestTexture )
//uniform sampler2D Texture1;
//varying vec2 v_TexCoordTest;
//#endif	/*	F_TestTexture	*/

void main()
{
	vec4 diffuseColor = Material.diffuse;

#if defined( F_VertexColor )
	diffuseColor = diffuseColor * v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_DiffuseTexture )
	diffuseColor = diffuseColor * texture2D( u_DiffuseTexture, v_TexCoord0 );
#endif	/*	F_DiffuseTexture	*/

//#if defined( F_TestTexture )
//	diffuseColor = diffuseColor + texture2D( Texture1, v_TexCoordTest );
//#endif	/*	F_TestTexture	*/

#if defined( F_AmbientColor )
	vec4 ambientColor = Scene.ambient;
#else
	vec4 ambientColor = vec4( 0.0, 0.0, 0.0, 1.0 );
#endif  /*  F_AmbientColor    */

	vec4 finalColor = ambientColor * diffuseColor;
	
#if defined( F_EmissionColor )
	finalColor = finalColor + Material.emission;
#endif	/*	F_EmissionColor	*/

	gl_FragColor = finalColor;
}