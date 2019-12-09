#version 150

in  vec4 vPosition;
in  vec3 vNormal;
in  vec2 vTexCoord;
out vec2 texCoord;
out mat4 _modelView;
out vec3 _normal;
out vec3 pos;

uniform mat4 ModelView;
uniform mat4 Projection;

void main()
{
	_modelView = ModelView;
	_normal = vNormal;
	texCoord = vTexCoord;
    
	// Transform vertex  position into eye coordinates
    pos = (ModelView * vPosition).xyz;

    gl_Position =  Projection * ModelView * vPosition;

    
	
	
}
