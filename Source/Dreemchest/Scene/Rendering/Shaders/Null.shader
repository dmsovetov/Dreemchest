[VertexShader]
#version 120

uniform mat4 u_vp, u_transform;

void main() {
	gl_Position = u_vp * u_transform * gl_Vertex;
}

[FragmentShader]
#version 120

void main() {
	gl_FragColor = vec4( 1.0, 0.0, 1.0, 1.0 );
}