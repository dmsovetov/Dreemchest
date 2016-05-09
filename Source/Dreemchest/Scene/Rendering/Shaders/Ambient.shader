// shadertype=glsl

[Features]
F_VertexNormal 		= vertexNormal
F_VertexColor  		= vertexColor
F_AmbientColor	  	= ambientColor
F_EmissionColor	  	= emissionColor
F_DiffuseTexture	= texture0
F_RimLight			= rimLight

[VertexShader]
varying vec4 wsVertex;

#if defined( F_VertexColor )
varying vec4 v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_DiffuseTexture )
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

#if defined( F_VertexNormal )
varying vec3 wsNormal;
#endif  /*  F_VertexNormal    */

void main()
{
	wsVertex = Instance.transform * gl_Vertex;
	
	gl_Position  = View.transform * wsVertex;
	gl_PointSize = 5;

#if defined( F_VertexColor )
	v_Color = gl_Color;
#endif  /*  F_VertexColor    */

#if defined( F_VertexNormal )
	wsNormal = (Instance.transform * vec4( gl_Normal, 0.0)).xyz;
#endif  /*  F_VertexNormal    */

#if defined( F_DiffuseTexture )
	v_TexCoord0 = gl_MultiTexCoord0.xy;
#endif	/*	F_DiffuseTexture	*/
}     

[FragmentShader]
varying vec4 wsVertex;

#if defined( F_VertexColor )
varying vec4 v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_VertexNormal )
varying vec3 wsNormal;
#endif  /*  F_VertexNormal    */

#if defined( F_DiffuseTexture )
uniform sampler2D u_DiffuseTexture;
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

//! Computes a rim light factor
float rim( vec3 point, vec3 normal, vec3 eye, float start, float end )
{
	vec3 dir = normalize( eye - point );
	return smoothstep( start, end, 1.0 - max( dot( normal, dir ), 0.0 ) );	
}

void main()
{
	vec4 diffuseColor = Material.diffuse;

#if defined( F_VertexColor )
	diffuseColor *= v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_DiffuseTexture )
	diffuseColor *= texture2D( u_DiffuseTexture, v_TexCoord0 );
#endif	/*	F_DiffuseTexture	*/

#if defined( F_AmbientColor )
	vec4 ambientColor = Scene.ambient;
#else
	vec4 ambientColor = vec4( 0.0, 0.0, 0.0, 1.0 );
#endif  /*  F_AmbientColor    */

	vec4 finalColor = ambientColor * diffuseColor;
	
#if defined( F_EmissionColor )
	finalColor += Material.emission;
#endif	/*	F_EmissionColor	*/

#if defined( F_RimLight ) && defined( F_VertexNormal )
	finalColor += vec4( Material.rim.color, 1.0 ) * rim( wsVertex.xyz, wsNormal, View.position, Material.rim.start, Material.rim.end ) * Material.rim.factor;
#endif	/*	F_RimLight	*/

	gl_FragColor = finalColor;
}