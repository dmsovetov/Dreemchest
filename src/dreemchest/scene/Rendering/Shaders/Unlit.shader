[VertexShader]
#version 140

uniform mat4 u_vp, u_transform;
varying vec3 v_normal;

#ifdef USE_DIFFUSE_MAP
	varying vec2 v_uv0;
#endif

void main()
{
#ifdef USE_DIFFUSE_MAP
	v_uv0 = gl_MultiTexCoord0.xy;
#endif

	gl_Position = u_vp * u_transform * gl_Vertex;
}

[FragmentShader]
#version 140

#ifdef USE_DIFFUSE_MAP
	uniform sampler2D u_tex0;
	varying vec2	  v_uv0;
#endif

uniform vec4 u_clr0;

void main()
{
	vec4 result  = u_clr0;

#ifdef USE_DIFFUSE_MAP
	result *= texture2D( u_tex0, v_uv0 );
#endif

	gl_FragColor = result;
}