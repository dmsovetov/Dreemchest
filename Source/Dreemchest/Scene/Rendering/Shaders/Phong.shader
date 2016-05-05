// shadertype=glsl

[Features]
F_VertexNormal 		= vertexNormal
F_VertexColor  		= vertexColor
F_LightType	  		= lightType
F_DiffuseTexture	= texture0

[VertexShader]
#if defined( F_VertexColor )
varying vec4 v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_DiffuseTexture )
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

#if defined( F_VertexNormal )
varying vec3 v_Normal;
varying vec3 v_LightPos;
varying vec3 v_LightDir;
varying vec3 v_VertexPos;
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
	v_LightPos		= Light.position;
	v_LightDir		= Light.direction;
	v_VertexPos		= vertex.xyz;
#endif  /*  F_VertexNormal    */

#if defined( F_DiffuseTexture )
	v_TexCoord0		= gl_MultiTexCoord0.xy;
#endif	/*	F_DiffuseTexture	*/
}     

[FragmentShader]
#if defined( F_VertexColor )
varying vec4 v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_VertexNormal )
varying vec3 v_Normal;
varying vec3 v_LightPos;
varying vec3 v_LightDir;
varying vec3 v_VertexPos;
#endif  /*  F_VertexNormal    */

#if defined( F_DiffuseTexture )
uniform sampler2D u_DiffuseTexture;
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

vec4 phongLightIntensity( vec3 point, vec3 light, vec3 direction, vec3 normal, float range, float intensity, float cutoff )
{
#if F_LightType == 1
	vec3  dir  = light - point;
	float d    = length( dir ) / range;
	float att  = 1.0 / (1.0 + 25.0 * d * d);
#elif F_LightType == 2
	vec3  dir  = normalize( light - point );
	float d    = length( dir ) / range;
	float spot = max( dot( dir, direction ), 0.0 );
	float att  = 1.0 / (1.0 + 25.0 * d * d) * (1.0 - (1.0 - spot) * 1.0 / (1.0 - cutoff));
#elif F_LightType == 3
	vec3  dir  = direction;
	float att  = 1.0;
#endif	/*	F_LightType == 1	*/

	return max( dot( normal, normalize( dir ) ), 0.0 ) * intensity * att;
}

void main()
{
	vec4 diffuseColor = Material.diffuse;
	vec4 lightColor   = vec4( 0.0, 0.0, 0.0, 1.0 );

#if defined( F_VertexColor )
	diffuseColor = diffuseColor * v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_DiffuseTexture )
	diffuseColor = diffuseColor * texture2D( u_DiffuseTexture, v_TexCoord0 );
#endif	/*	F_DiffuseTexture	*/

#if defined( F_VertexNormal )
	lightColor = phongLightIntensity( v_VertexPos, v_LightPos, v_LightDir, normalize( v_Normal ), Light.range, Light.intensity, Light.cutoff ) * vec4( Light.color, 1.0 );
#endif  /*  F_VertexNormal    */

	vec4 finalColor = lightColor * diffuseColor;

	gl_FragColor = finalColor;
}