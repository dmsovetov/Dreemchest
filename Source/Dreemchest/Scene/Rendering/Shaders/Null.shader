// shadertype=glsl
[Features]
F_View     = cbuffer1
F_Instance = cbuffer2

[VertexShader]
void main()
{
    vec4 vertex = gl_Vertex;

#if defined( F_Instance )
    vertex = Instance.transform * vertex;
#endif    /*    F_Instance    */

#if defined( F_View )
    vertex = View.transform * vertex;
#endif    /*    F_View    */

    gl_Position = vertex;
}

[FragmentShader]
void main()
{
    gl_FragColor = vec4( 1.0, 0.0, 1.0, 1.0 );
}
