[Features]
F_NormalAttribute 	= inputNormal
F_ColorAttribute  	= inputColor
F_AmbientColor	  	= ambientColor
F_EmissionColor	  	= emissionColor
F_DiffuseTexture	= texture0

[VertexShader]
#if defined( F_ColorAttribute )
varying vec4 v_Color;
#endif  /*  F_ColorAttribute    */

#if defined( F_DiffuseTexture )
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

#if defined( F_NormalAttribute )
varying vec3 v_Normal;
#endif  /*  F_NormalAttribute    */

void main()
{
	vec4 vertex 	= Instance.transform * gl_Vertex;
	
	gl_Position     = View.transform * vertex;
	gl_PointSize    = 5;
#if defined( F_ColorAttribute )
	v_Color         = gl_Color;
#endif  /*  F_ColorAttribute    */

#if defined( F_NormalAttribute )
	v_Normal        = gl_Normal;
#endif  /*  F_NormalAttribute    */

#if defined( F_DiffuseTexture )
	v_TexCoord0		= gl_MultiTexCoord0.xy;
#endif	/*	F_DiffuseTexture	*/
}     

[FragmentShader]
#if defined( F_ColorAttribute )
varying vec4 v_Color;
#endif  /*  F_ColorAttribute    */

#if defined( F_NormalAttribute )
varying vec3 v_Normal;
#endif  /*  F_NormalAttribute    */

#if defined( F_DiffuseTexture )
uniform sampler2D u_DiffuseTexture;
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

void main()
{
	vec4 diffuseColor = Material.diffuse;

#if defined( F_ColorAttribute )
	diffuseColor = diffuseColor * v_Color;
#endif  /*  F_ColorAttribute    */

#if defined( F_DiffuseTexture )
	diffuseColor = diffuseColor * texture2D( u_DiffuseTexture, v_TexCoord0 );
#endif	/*	F_DiffuseTexture	*/

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