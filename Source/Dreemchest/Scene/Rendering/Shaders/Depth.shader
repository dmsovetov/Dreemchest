[Features]
F_LinearDepth = linearDepth

[VertexShader]
void main()
{
	gl_Position = View.transform * Instance.transform * gl_Vertex;
}

[FragmentShader]
void main()
{
#ifdef F_LinearDepth
	gl_FragColor = vec4( gl_FragCoord.z / gl_FragCoord.w / View.far );
#else
	gl_FragDepth = gl_FragCoord.z;
#endif	/*	F_LinearDepth	*/
}