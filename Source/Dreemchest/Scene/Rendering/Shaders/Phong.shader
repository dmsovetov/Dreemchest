[Features]
F_NormalAttribute 	= inputNormal
F_ColorAttribute  	= inputColor
F_PointLight	  	= pointLight
F_DiffuseTexture	= texture0

[VertexShader]
#if defined( F_ColorAttribute )
varying vec4 v_Color;
#endif  /*  F_ColorAttribute    */

#if defined( F_DiffuseTexture )
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

#if defined( F_NormalAttribute ) && defined( F_PointLight )
varying vec3 v_Normal;
varying vec3 v_LightPos;
varying vec3 v_VertexPos;
#endif  /*  F_NormalAttribute    */

void main()
{
	vec4 vertex 	= Instance.transform * gl_Vertex;
	
	gl_Position     = View.transform * vertex;
	gl_PointSize    = 5;
#if defined( F_ColorAttribute )
	v_Color         = gl_Color;
#endif  /*  F_ColorAttribute    */

#if defined( F_NormalAttribute ) && defined( F_PointLight )
	v_Normal        = gl_Normal;
	v_LightPos		= Light.position;
	v_VertexPos		= vertex.xyz;
#endif  /*  F_NormalAttribute    */

#if defined( F_DiffuseTexture )
	v_TexCoord0		= gl_MultiTexCoord0.xy;
#endif	/*	F_DiffuseTexture	*/
}     

[FragmentShader]
#if defined( F_ColorAttribute )
varying vec4 v_Color;
#endif  /*  F_ColorAttribute    */

#if defined( F_NormalAttribute ) && defined( F_PointLight )
varying vec3 v_Normal;
varying vec3 v_LightPos;
varying vec3 v_VertexPos;
#endif  /*  F_NormalAttribute    */

#if defined( F_DiffuseTexture )
uniform sampler2D u_DiffuseTexture;
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

vec4 phongLightIntensity( vec3 point, vec3 light, vec3 normal, float range, float intensity )
{
	vec3  dir  = light - point;
	float d    = length( dir ) / range;
	float att  = 1.0 / (1.0 + 25.0 * d * d);
	return max( dot( normal, normalize( dir ) ), 0.0 ) * intensity * att;
}

void main()
{
	vec4 diffuseColor = Material.diffuse;
	vec4 lightColor   = vec4( 0.0, 0.0, 0.0, 1.0 );

#if defined( F_ColorAttribute )
	diffuseColor = diffuseColor * v_Color;
#endif  /*  F_ColorAttribute    */

#if defined( F_DiffuseTexture )
	diffuseColor = diffuseColor * texture2D( u_DiffuseTexture, v_TexCoord0 );
#endif	/*	F_DiffuseTexture	*/

#if defined( F_NormalAttribute ) && defined( F_PointLight )
	lightColor = phongLightIntensity( v_VertexPos, v_LightPos, normalize( v_Normal ), Light.range, Light.intensity ) * vec4( Light.color, 1.0 );
#endif  /*  F_NormalAttribute    */

	vec4 finalColor = lightColor * diffuseColor;

	gl_FragColor = finalColor;
}