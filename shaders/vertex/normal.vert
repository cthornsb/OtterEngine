varying vec3 vertex_color;
void main(){
	gl_Position = ftransform(); // Transform coordinates to clip space
	vertex_color = vec3(gl_Normal);
}
