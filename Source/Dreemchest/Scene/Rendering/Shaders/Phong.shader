// shadertype=glsl

[Features]
F_VertexNormal 		= vertexNormal
F_VertexColor  		= vertexColor
F_LightType	  		= lightType
F_DiffuseTexture	= texture0
F_ShadowTexture		= texture1

[VertexShader]
#if defined( F_VertexColor )
varying vec4 v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_DiffuseTexture )
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

#if defined( F_ShadowTexture )
varying vec4 v_TexCoordShadow;
#endif	/*	F_ShadowTexture	*/

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
	v_Normal        = (Instance.transform * vec4( gl_Normal, 0.0)).xyz;
	v_LightPos		= Light.position;
	v_LightDir		= Light.direction;
	v_VertexPos		= vertex.xyz;
#endif  /*  F_VertexNormal    */

#if defined( F_DiffuseTexture )
	v_TexCoord0		= gl_MultiTexCoord0.xy;
#endif	/*	F_DiffuseTexture	*/

#if defined( F_ShadowTexture )
	v_TexCoordShadow = Shadow.transform * vertex;
#endif	/*	F_ShadowTexture	*/
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

#if defined( F_ShadowTexture )
uniform sampler2D u_ShadowTexture;
varying vec4 v_TexCoordShadow;
#endif	/*	F_ShadowTexture	*/

//! Computes a distance attenuation of a light source between point 'a' and 'b'
float distanceAttenuation( vec3 a, vec3 b, float range, float constant, float linear, float quadratic )
{
	float d = length( a - b ) / range;
	return 1.0 / (constant + linear * d + quadratic * d * d);
}

//! Computes a point light intensity at specified point
float pointLightIntensity( vec3 point, vec3 light, vec3 normal )
{
	vec3 dir = normalize( light - point );
	return max( dot( normal, dir ), 0.0 );
}

//! Computes a spot light intensity at specified point
float spotLightIntensity( vec3 point, vec3 light, vec3 normal, vec3 direction, float cutoff )
{
	vec3  dir	  = normalize( light - point );
	float lambert = max( dot( normal, dir ), 0.0 );
	float spot	  = max( dot( direction, dir ), 0.0 );
	float falloff = 1.0 - (1.0 - spot) * 1.0 / (1.0 - cutoff);

	return lambert * falloff;
}

//! Computes a directional light intensity at specified point
float directionalLightIntensity( vec3 light, vec3 normal )
{
	return max( dot( normal, light ), 0.0 );
}

//! Converts a clip space coordinates to a texture space.
float shadowFactor( sampler2D texture, vec4 lightSpaceCoord, float bias )
{
	float currentDepth = lightSpaceCoord.z / lightSpaceCoord.w;
	float storedDepth  = texture2D( texture, lightSpaceCoord.xy / lightSpaceCoord.w * 0.5 + 0.5 ).x;

	return (currentDepth <= (storedDepth + bias)) ? 1.0 : 0.0;
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
	#if F_LightType == 1
	float attenuation = distanceAttenuation( v_VertexPos, v_LightPos, Light.range, 1.0, 0.0, 25.0 );
	float intensity   = pointLightIntensity( v_VertexPos, v_LightPos, normalize( v_Normal ) ) * attenuation;

	#elif F_LightType == 2
	float attenuation = distanceAttenuation( v_VertexPos, v_LightPos, Light.range, 1.0, 0.0, 25.0 );
	float intensity   = spotLightIntensity( v_VertexPos, v_LightPos, normalize( v_Normal ), v_LightDir, Light.cutoff ) * attenuation;

	#elif F_LightType == 3
	float intensity   = directionalLightIntensity( v_LightDir, normalize( v_Normal ) );
	#endif	/*	F_LightType == 1	*/

	lightColor = vec4( Light.color, 1.0 ) * Light.intensity * intensity;
#endif  /*  F_VertexNormal    */

#if F_ShadowTexture
	lightColor *= shadowFactor( u_ShadowTexture, v_TexCoordShadow, 0.0 );
#endif	/*	F_ShadowTexture	*/

	vec4 finalColor = lightColor * diffuseColor;

	gl_FragColor = finalColor;
}