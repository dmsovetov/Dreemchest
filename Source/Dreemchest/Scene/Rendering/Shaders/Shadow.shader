// shadertype=glsl

[VertexShader]
void main()
{
    gl_Position = Shadow.transform * Instance.transform * gl_Vertex;
}

[FragmentShader]

void main()
{
}
