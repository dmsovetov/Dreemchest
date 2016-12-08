// shadertype=glsl

[VertexShader]
void main()
{
    gl_Position = View.transform * Instance.transform * gl_Vertex;
}

[FragmentShader]
void main()
{
    gl_FragColor = vec4( 1.0, 0.0, 1.0, 1.0 );
}
