[Features]
F_DIFFUSE_MAP       = 1
F_AMBIENT_OCCLUSION = 16
F_EMISSION_MAP      = 32
F_DIFFUSE_DETAIL    = 64
F_PRIMARY_UV        = 512

[VertexShader]
#version 120

uniform mat4 u_vp, u_transform;

#ifdef F_PRIMARY_UV
	varying vec2 v_uv0;
#endif  /*  F_PRIMARY_UV   */

void main()
{
#ifdef F_PRIMARY_UV
	v_uv0 = gl_MultiTexCoord0.xy;
#endif  /*  F_PRIMARY_UV   */

	gl_Position = u_vp * u_transform * gl_Vertex;
}

[FragmentShader]
#version 120

#ifdef F_PRIMARY_UV
	varying vec2 v_uv0;
#endif  /*  F_PRIMARY_UV   */

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

void main()
{
    // Calculate the diffuse term
	vec4 diffuse = u_diffuseColor;

#ifdef F_DIFFUSE_MAP
	diffuse *= texture2D( u_diffuseTexture, v_uv0 );
#endif  /*  F_DIFFUSE_MAP   */

#ifdef F_DIFFUSE_DETAIL
    diffuse *= texture2D( u_diffuseDetailTexture, v_uv0 );
#endif  /*  F_DIFFUSE_DETAIL    */

    // Calculate ambient term
    vec4 ambient = u_ambientColor * diffuse;

#ifdef F_AMBIENT_OCCLUSION
    ambient *= texture2D( u_ambientOcclusionTexture, v_uv0 );
#endif  /*  F_AMBIENT_OCCLUSION */

    // Calculate emission term
    vec4 emission = u_emissionColor;

#ifdef F_EMISSION_MAP
    emission *= texture2D( u_emissionTexture, v_uv0 );
#endif  /*  F_AMBIENT_OCCLUSION */

	gl_FragColor = vec4( (diffuse + ambient + emission).rgb, diffuse.a );
}