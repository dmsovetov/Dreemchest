[VertexShader]
#version 120

uniform mat4 u_vp, u_transform, u_inverseTransform;
uniform vec4 u_lightPosition;
uniform vec4 u_lightColor;

#ifdef USE_DIFFUSE_MAP
	varying vec2 v_uv0;
#endif

varying vec3 v_normal;
varying vec4 v_lightDir;
varying vec4 v_lightColor;

void main()
{
#ifdef USE_DIFFUSE_MAP
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

#ifdef USE_DIFFUSE_MAP
	uniform sampler2D u_tex0;
	varying vec2	  v_uv0;
#endif

varying vec3 v_normal;
varying vec4 v_lightDir;
varying vec4 v_lightColor;

uniform vec4 u_clr0;
uniform vec4 u_lightPosition;
uniform vec4 u_lightIntensity;

void main() {
	vec4 diffuse  = u_clr0;

#ifdef USE_DIFFUSE_MAP
	diffuse *= texture2D( u_tex0, v_uv0 );
#endif

	vec3 normal = normalize( v_normal );
	vec3 light  = normalize( v_lightDir.xyz );
	float r     = length(v_lightDir.xyz) / u_lightPosition.w;
	float att   = 1.0 / (1.0 + 25.0 * r * r);
	vec4 result = diffuse * att * v_lightColor.a * v_lightColor * max( dot( normal, light ), 0.0 );

	gl_FragColor = vec4( result.rgb, diffuse.a );	
}