#version 150

struct Material {
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

struct PointLight {
    vec3 position;
    
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

#define NR_POINT_LIGHTS 3

in  vec3 pos;
in  vec2 texCoord;
in  vec3 _normal;
in  mat4 _modelView;
out vec4 fColor;

uniform sampler2D texture1;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

vec4 CalcPointLight(PointLight light);

void main() 
{ 
   vec4 result = vec4(0, 0, 0, 1);
   for(int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i]);

   fColor = result;
} 

vec4 CalcPointLight(PointLight light) {
   vec3 L = normalize( light.position.xyz - pos );
   vec3 E = normalize( -pos );
   vec3 H = normalize( L + E );

   // Transform vertex normal into eye coordinates
   vec3 N = normalize( _modelView * vec4(_normal, 0.0) ).xyz;

   // Compute terms in the illumination equation
    vec4 ambient = light.ambient;

    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd*light.diffuse * material.diffuse;
	
    float Ks = pow( max(dot(N, H), 0.0), material.shininess );
    vec4  specular = Ks * light.specular * material.specular;
    
    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

	vec4 color = ambient + specular;
	color.a = 1.0;

    return (color + (diffuse + (1 - diffuse) * ambient) * texture2D(texture1, texCoord));
}