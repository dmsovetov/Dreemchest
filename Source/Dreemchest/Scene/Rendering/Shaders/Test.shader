[Features]
F_NormalAttribute = inputNormal
F_ColorAttribute  = inputColor
F_AmbientColor	  = ambientColor

[VertexShader]
#ifdef F_ColorAttribute
varying vec4 v_Color;
#endif  /*  F_ColorAttribute    */

#ifdef F_NormalAttribute
varying vec3 v_Normal;
#endif  /*  F_NormalAttribute    */

void main() {
	gl_Position     = Camera.viewProjection * Instance.transform * gl_Vertex;
	gl_PointSize    = 5;
#ifdef F_ColorAttribute
	v_Color         = gl_Color;
#endif  /*  F_ColorAttribute    */

#ifdef F_NormalAttribute
	v_Normal        = gl_Normal;
#endif  /*  F_NormalAttribute    */
}     

[FragmentShader]
#ifdef F_ColorAttribute
varying vec4 v_Color;
#endif  /*  F_ColorAttribute    */

#ifdef F_NormalAttribute
varying vec3 v_Normal;
#endif  /*  F_NormalAttribute    */

void main()
{
#ifdef F_ColorAttribute
	vec4 color = v_Color;
#else
	vec4 color = vec4( 1.0, 1.0, 1.0, 1.0 );
#endif  /*  F_ColorAttribute    */

#ifdef F_NormalAttribute
	color = color * vec4( v_Normal * 0.5 + 0.5, 1.0 );
#endif  /*  F_NormalAttribute    */

#ifdef F_AmbientColor
	color = color + Scene.ambient;
#endif  /*  F_AmbientColor    */

	gl_FragColor = Material.emission + color * Material.diffuse;
}