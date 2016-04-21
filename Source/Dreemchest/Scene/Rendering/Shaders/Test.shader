[Features]
F_NormalAttribute = inputNormal
F_ColorAttribute  = inputColor
F_AmbientColor	  = ambientColor
F_PointLight	  = pointLight

[VertexShader]
#if defined( F_ColorAttribute )
varying vec4 v_Color;
#endif  /*  F_ColorAttribute    */

#if defined( F_NormalAttribute ) && defined( F_PointLight )
varying vec3 v_Normal;
varying vec3 v_LightDir;
#endif  /*  F_NormalAttribute    */

void main()
{
	vec4 vertex 	= Instance.transform * gl_Vertex;
	
	gl_Position     = Camera.viewProjection * vertex;
	gl_PointSize    = 5;
#if defined( F_ColorAttribute )
	v_Color         = gl_Color;
#endif  /*  F_ColorAttribute    */

#if defined( F_NormalAttribute ) && defined( F_PointLight )
	v_Normal        = gl_Normal;
	v_LightDir		= Light.position - vertex.xyz;
#endif  /*  F_NormalAttribute    */
}     

[FragmentShader]
#if defined( F_ColorAttribute )
varying vec4 v_Color;
#endif  /*  F_ColorAttribute    */

#if defined( F_NormalAttribute ) && defined( F_PointLight )
varying vec3 v_Normal;
varying vec3 v_LightDir;
#endif  /*  F_NormalAttribute    */

void main()
{
#if defined( F_ColorAttribute )
	vec4 color = v_Color;
#else
	vec4 color = vec4( 1.0, 1.0, 1.0, 1.0 );
#endif  /*  F_ColorAttribute    */

#if defined( F_NormalAttribute ) && defined( F_PointLight )
	float d   = length( v_LightDir ) / Light.radius;
	float att = 1.0 / (1.0 + 25.0 * d * d);
	color 	  = color * max( dot( v_Normal, v_LightDir ), 0.0 ) * vec4( Light.color, 1.0 ) * Light.intensity * att;
#endif  /*  F_NormalAttribute    */

#if defined( F_AmbientColor )
	color = color + Scene.ambient;
#endif  /*  F_AmbientColor    */

	gl_FragColor = Material.emission + color * Material.diffuse;
}