[Features]
F_NormalAttribute = inputNormal
F_ColorAttribute  = inputColor
F_AmbientColor	  = ambientColor
F_EmissionColor	  = emissionColor
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
	
	gl_Position     = View.transform * vertex;
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
	vec4 diffuseColor = Material.diffuse;
	vec4 lightColor   = vec4( 0.0, 0.0, 0.0, 1.0 );

#if defined( F_ColorAttribute )
	diffuseColor = diffuseColor * Material.diffuse;
#endif  /*  F_ColorAttribute    */

#if defined( F_NormalAttribute ) && defined( F_PointLight )
	float d    = length( v_LightDir ) / Light.range;
	float att  = 1.0 / (1.0 + 25.0 * d * d);
	lightColor = max( dot( v_Normal, v_LightDir ), 0.0 ) * vec4( Light.color, 1.0 ) * Light.intensity * att;
#endif  /*  F_NormalAttribute    */

#if defined( F_AmbientColor )
	vec4 ambientColor = Scene.ambient;
#else
	vec4 ambientColor = vec4( 0.0, 0.0, 0.0, 1.0 );
#endif  /*  F_AmbientColor    */

	vec4 finalColor = (ambientColor + lightColor) * diffuseColor;
	
#if defined( F_EmissionColor )
	finalColor = finalColor + Material.emission;
#endif	/*	F_EmissionColor	*/

	gl_FragColor = finalColor;
}