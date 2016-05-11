// shadertype=glsl

[Features]
F_VertexColor = vertexColor

[VertexShader]
#if defined( F_VertexColor )
	varying vec4 v_Color;
#endif  /*  F_VertexColor    */

void main()
{
	gl_Position  = View.transform * gl_Vertex;

#if defined( F_VertexColor )
	v_Color = gl_Color;
#endif  /*  F_VertexColor    */
}     

[FragmentShader]
#if defined( F_VertexColor )
	varying vec4 v_Color;
#endif  /*  F_VertexColor    */

void main()
{
	vec4 diffuseColor = vec4( 1.0, 1.0, 1.0, 1.0 );

#if defined( F_VertexColor )
	diffuseColor *= v_Color;
#endif  /*  F_VertexColor    */

	gl_FragColor = diffuseColor;
}