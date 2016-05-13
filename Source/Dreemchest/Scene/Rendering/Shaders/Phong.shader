// shadertype=glsl

[Features]
F_VertexNormal 		= vertexNormal
F_VertexColor  		= vertexColor
F_LightType	  		= lightType
F_DiffuseTexture	= texture0
F_ShadowTexture		= texture1
F_ShadowFiltering	= shadowFiltering
F_ClipPlanes		= cbuffer6

[VertexShader]
varying vec3 wsVertex;

#if defined( F_VertexColor )
varying vec4 v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_DiffuseTexture )
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

#if defined( F_ShadowTexture )
varying vec4 lsVertex;
#endif	/*	F_ShadowTexture	*/

#if defined( F_VertexNormal )
varying vec3 wsNormal;
varying vec3 wsLight;
varying vec3 wsLightDir;
#endif  /*  F_VertexNormal    */

void main()
{
	vec4 vertex = Instance.transform * gl_Vertex;
	
	gl_Position     = View.transform * vertex;
	gl_PointSize    = 5;

	wsVertex = vertex.xyz;

#if defined( F_VertexColor )
	v_Color = gl_Color;
#endif  /*  F_VertexColor    */

#if defined( F_VertexNormal )
	wsNormal   = (Instance.transform * vec4( gl_Normal, 0.0)).xyz;
	wsLight	   = Light.position;
	wsLightDir = Light.direction;
#endif  /*  F_VertexNormal    */

#if defined( F_DiffuseTexture )
	v_TexCoord0		= gl_MultiTexCoord0.xy;
#endif	/*	F_DiffuseTexture	*/

#if defined( F_ShadowTexture )
	lsVertex = Shadow.transform * vertex;
#endif	/*	F_ShadowTexture	*/

#if defined( F_ClipPlanes )
	for( int i = 0; i < 6; i++ ) {
		gl_ClipDistance[i] = dot( ClipPlanes.equation[i], vertex );
	}
#endif	/*	F_ClipPlanes	*/
}     

[FragmentShader]
varying vec3 wsVertex;

#if defined( F_VertexColor )
varying vec4 v_Color;
#endif  /*  F_VertexColor    */

#if defined( F_VertexNormal )
varying vec3 wsNormal;
varying vec3 wsLight;
varying vec3 wsLightDir;
#endif  /*  F_VertexNormal    */

#if defined( F_DiffuseTexture )
uniform sampler2D u_DiffuseTexture;
varying vec2 v_TexCoord0;
#endif	/*	F_DiffuseTexture	*/

#if defined( F_ShadowTexture )
uniform sampler2D u_ShadowTexture;
varying vec4 lsVertex;
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

//! Returns a binary depth tesing result
float shadowSampleBinary( sampler2D texture, float currentDepth, vec2 sm )
{
	float storedDepth = texture2D( texture, sm ).x;
	return (currentDepth <= storedDepth) ? 1.0 : 0.0;	
}

//! Returns a PCF depth testing result
float shadowSamplePCF( sampler2D texture, float currentDepth, vec2 sm )
{
	// A shadow factor accumulator
	float shadow = 0.0;

	for( int i = -F_ShadowFiltering * 2; i < F_ShadowFiltering * 2; i++ ) {
		for( int j = -F_ShadowFiltering * 2; j < F_ShadowFiltering * 2; j++ ) {
			shadow += shadowSampleBinary( texture, currentDepth, sm + vec2( i, j ) * Shadow.invSize );
		}
	}
	
	// Compare and return result
	return shadow / ((F_ShadowFiltering * 4 + 1) * (F_ShadowFiltering * 4 + 1));
}

float nrand( vec2 n ) {
	return fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

vec2 rot2d( vec2 p, float a ) {
	vec2 sc = vec2(sin(a),cos(a));
	return vec2( dot( p, vec2(sc.y, -sc.x) ), dot( p, sc.xy ) );
}

//! Returns a poisson-filtered depth testing result
float shadowSamplePoisson( sampler2D texture, float currentDepth, vec2 sm, vec3 fragCoord )
{
	const int NUM_TAPS = 12;
	float radius = 4.0 * Shadow.invSize;

	vec2 poisson[NUM_TAPS];
	poisson[0]  = vec2( -0.326, -0.406 );
	poisson[1]  = vec2( -0.840, -0.074 );
	poisson[2]  = vec2( -0.696,  0.457 );
	poisson[3]  = vec2( -0.203,  0.621 );
	poisson[4]  = vec2(  0.962, -0.195 );
	poisson[5]  = vec2(  0.473, -0.480 );
	poisson[6]  = vec2(  0.519,  0.767 );
	poisson[7]  = vec2(  0.185, -0.893 );
	poisson[8]  = vec2(  0.507,  0.064 );
	poisson[9]  = vec2(  0.896,  0.412 );
	poisson[10] = vec2( -0.322, -0.933 );
	poisson[11] = vec2( -0.792, -0.598 );

	vec4 sum = vec4( 0.0 );
	float rnd  = nrand( fragCoord.xy );
	vec4 basis = vec4( rot2d( vec2( 1.0, 0.0 ), rnd ), rot2d( vec2( 0.0, 1.0 ), rnd ) );

	sum += shadowSampleBinary( texture, currentDepth, sm + radius * vec2( dot( poisson[ 0], basis.xz ), dot( poisson[ 0], basis.yw ) ) );
	sum += shadowSampleBinary( texture, currentDepth, sm + radius * vec2( dot( poisson[ 1], basis.xz ), dot( poisson[ 1], basis.yw ) ) );
	sum += shadowSampleBinary( texture, currentDepth, sm + radius * vec2( dot( poisson[ 2], basis.xz ), dot( poisson[ 2], basis.yw ) ) );
	sum += shadowSampleBinary( texture, currentDepth, sm + radius * vec2( dot( poisson[ 3], basis.xz ), dot( poisson[ 3], basis.yw ) ) );
	sum += shadowSampleBinary( texture, currentDepth, sm + radius * vec2( dot( poisson[ 4], basis.xz ), dot( poisson[ 4], basis.yw ) ) );
	sum += shadowSampleBinary( texture, currentDepth, sm + radius * vec2( dot( poisson[ 5], basis.xz ), dot( poisson[ 5], basis.yw ) ) );
	sum += shadowSampleBinary( texture, currentDepth, sm + radius * vec2( dot( poisson[ 6], basis.xz ), dot( poisson[ 6], basis.yw ) ) );
	sum += shadowSampleBinary( texture, currentDepth, sm + radius * vec2( dot( poisson[ 7], basis.xz ), dot( poisson[ 7], basis.yw ) ) );
	sum += shadowSampleBinary( texture, currentDepth, sm + radius * vec2( dot( poisson[ 8], basis.xz ), dot( poisson[ 8], basis.yw ) ) );
	sum += shadowSampleBinary( texture, currentDepth, sm + radius * vec2( dot( poisson[ 9], basis.xz ), dot( poisson[ 9], basis.yw ) ) );
	sum += shadowSampleBinary( texture, currentDepth, sm + radius * vec2( dot( poisson[10], basis.xz ), dot( poisson[10], basis.yw ) ) );
	sum += shadowSampleBinary( texture, currentDepth, sm + radius * vec2( dot( poisson[11], basis.xz ), dot( poisson[11], basis.yw ) ) );

	return sum / NUM_TAPS;
}

//! Performs a projected texture lookup and returns a depth testing result
float shadowFactor( sampler2D texture, vec4 lightSpaceCoord, float bias, vec3 fragCoord )
{
	// Perform a perspective divide and map to [0, 1] range
	lightSpaceCoord = lightSpaceCoord / lightSpaceCoord.w * 0.5 + 0.5;

	// Extract current and stored depths
	float currentDepth = lightSpaceCoord.z * bias;

#if F_ShadowFiltering
//	return shadowSamplePoisson( texture, currentDepth, lightSpaceCoord.xy, fragCoord );
	return shadowSamplePCF( texture, currentDepth, lightSpaceCoord.xy );
#else
	return shadowSampleBinary( texture, currentDepth, lightSpaceCoord.xy );
#endif	/*	F_ShadowFiltering	*/
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
	float attenuation = distanceAttenuation( wsVertex, wsLight, Light.range, 1.0, 0.0, 25.0 );
	float intensity   = pointLightIntensity( wsVertex, wsLight, normalize( wsNormal ) ) * attenuation;

	#elif F_LightType == 2
	float attenuation = distanceAttenuation( wsVertex, wsLight, Light.range, 1.0, 0.0, 25.0 );
	float intensity   = spotLightIntensity( wsVertex, wsLight, normalize( wsNormal ), wsLightDir, Light.cutoff ) * attenuation;

	#elif F_LightType == 3
	float intensity   = directionalLightIntensity( wsLightDir, normalize( wsNormal ) );
	#endif	/*	F_LightType == 1	*/

	lightColor = vec4( Light.color, 1.0 ) * Light.intensity * intensity;
#endif  /*  F_VertexNormal    */

#if F_ShadowTexture
	lightColor *= shadowFactor( u_ShadowTexture, lsVertex, 0.999999, wsVertex/*gl_FragCoord*/ );
#endif	/*	F_ShadowTexture	*/

	vec4 finalColor = lightColor * diffuseColor;

	gl_FragColor = finalColor;
}