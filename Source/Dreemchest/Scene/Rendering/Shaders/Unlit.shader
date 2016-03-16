[Features]
F_DIFFUSE_MAP = 1

[VertexShader]
#version 120

uniform mat4 u_vp, u_transform;

#ifdef F_DIFFUSE_MAP
	varying vec2 v_uv0;
#endif  /*  F_DIFFUSE_MAP   */

void main()
{
#ifdef F_DIFFUSE_MAP
	v_uv0 = gl_MultiTexCoord0.xy;
#endif  /*  F_DIFFUSE_MAP   */

	gl_Position = u_vp * u_transform * gl_Vertex;
}

[FragmentShader]
#version 120

#ifdef F_DIFFUSE_MAP
	uniform sampler2D u_tex0;
	varying vec2	  v_uv0;
#endif  /*  F_DIFFUSE_MAP   */

uniform vec4 u_clr0;

void main()
{
	vec4 result  = u_clr0;

#ifdef F_DIFFUSE_MAP
	result *= texture2D( u_tex0, v_uv0 );
#endif  /*  F_DIFFUSE_MAP   */

	gl_FragColor = result;
}