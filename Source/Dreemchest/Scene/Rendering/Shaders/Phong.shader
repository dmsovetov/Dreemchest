[Features]
F_DIFFUSE_MAP       = 1
F_AMBIENT_OCCLUSION = 16
F_EMISSION_MAP      = 32
F_DIFFUSE_DETAIL    = 64
F_PRIMARY_UV        = 512

[VertexShader]
#version 120

uniform mat4 u_vp, u_transform, u_inverseTransform;
uniform vec4 u_lightPosition;
uniform vec4 u_lightColor;

#ifdef F_PRIMARY_UV
	varying vec2 v_uv0;
#endif  /*  F_PRIMARY_UV   */

varying vec3 v_normal;
varying vec4 v_lightDir;
varying vec4 v_lightColor;

void main()
{
#ifdef F_PRIMARY_UV
	v_uv0 = gl_MultiTexCoord0.xy;
#endif

	vec4 vertex = u_transform * gl_Vertex;
	mat3 normal = transpose( mat3( u_inverseTransform ) );

	v_normal = normalize(normal * gl_Normal);
	v_lightDir = u_lightPosition - vertex;
	v_lightColor = u_lightColor;
	gl_Position = u_vp * vertex;
}

[FragmentShader]
#version 120

#ifdef F_PRIMARY_UV
	varying vec2 v_uv0;
#endif  /*  F_PRIMARY_UV   */

varying vec3 v_normal;
varying vec4 v_lightDir;
varying vec4 v_lightColor;

#ifdef F_DIFFUSE_MAP
	uniform sampler2D u_diffuseTexture;
#endif  /*  F_DIFFUSE_MAP   */

#ifdef F_AMBIENT_OCCLUSION
    uniform sampler2D u_ambientOcclusionTexture;
#endif  /*  F_AMBIENT_OCCLUSION */

#ifdef F_EMISSION_MAP
    uniform sampler2D u_emissionTexture;
#endif  /*  F_EMISSION_MAP */

#ifdef F_DIFFUSE_DETAIL
    uniform sampler2D u_diffuseDetailTexture;
#endif  /*  F_DIFFUSE_DETAIL */

uniform vec4 u_diffuseColor;
uniform vec4 u_emissionColor;
uniform vec4 u_ambientColor;

uniform vec4 u_lightPosition;

void main() {
	vec4 diffuse = u_diffuseColor;

#ifdef F_DIFFUSE_MAP
	diffuse *= texture2D( u_diffuseTexture, v_uv0 );
#endif

	vec3 normal = normalize( v_normal );
	vec3 light  = normalize( v_lightDir.xyz );
	float r     = length( v_lightDir.xyz ) / u_lightPosition.w;
	float att   = 1.0 / (1.0 + 25.0 * r * r);
	vec4 result = diffuse * att * v_lightColor.a * v_lightColor * max( dot( normal, light ), 0.0 );

	gl_FragColor = vec4( result.rgb, diffuse.a );

    //gl_FragColor = vec4( normal, 1.0f );
    //gl_FragColor = vec4( v_lightColor.rgb, 1.0 );
}