/*********************************************************
Version 1.000

Code provided by Michael Hemsley and Anthony Dick
for the course 
COMP SCI 3014/7090 Computer Graphics
School of Computer Science
University of Adelaide

Permission is granted for anyone to copy, use, modify, or distribute this
program and accompanying programs and documents for any purpose, provided
this copyright notice is retained and prominently displayed, along with
a note saying that the original programs are available from the aforementioned 
course web page. 

The programs and documents are distributed without any warranty, express or
implied.  As the programs were written for research purposes only, they have
not been tested to the degree that would be advisable in any important
application.  All use of these programs is entirely at the user's own risk.
*********************************************************/

#version 150

// manyAttributes.fp
// An example of using interpolated values from the previous stage

in vec2 st;
in vec3 color;
in vec3 normal;
in vec4 vertex;
in vec3 tangent;
in float crossP;
in vec3 lightDir;
uniform sampler2D texMap;


out vec4 fragColour;

uniform vec4 point_light_pos;
uniform vec3 directional_light_direction;

uniform vec3 point_light_ambient;     // Light ambient RGBA values
uniform vec3 point_light_diffuse;     // Light diffuse RGBA values  
uniform vec3 point_light_specular;    // Light specular RGBA values

uniform vec3 directional_light_ambient;     // Light ambient RGBA values
uniform vec3 directional_light_diffuse;     // Light diffuse RGBA values  
uniform vec3 directional_light_specular;    // Light specular RGBA values

uniform vec3 mtl_ambient;  // Ambient surface colour
uniform vec3 mtl_diffuse;  // Diffuse surface colour
uniform vec3 mtl_specular; // Specular surface colour

uniform float on_point;
uniform float on_directional;
uniform float on_emission;
uniform float on_wireframe;

uniform float shininess;



// This is the same function that appeared in the vertex shader for
// per-vertex lighting. Now it is being calculated for each fragment
// individually.
vec3 phongPointDiffuseAmbientLight(in vec4 position, in vec3 norm)
{
    vec3 s = normalize(vec3(point_light_pos - position));
    vec3 v = normalize(-position.xyz);
    vec3 r = reflect( -s, norm );
    
    vec3 ambient = point_light_ambient * mtl_ambient;
	
    // The diffuse component
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = point_light_diffuse * mtl_diffuse * sDotN;

    // The specular component
    vec3 spec = vec3(0.0);
    if ( sDotN > 0.0 )
		spec = point_light_specular * mtl_specular *
            pow( max( dot(r,v), 0.0 ), shininess );

    return diffuse + ambient ;// + spec+ ;
}

#define PI 3.141592653589793

float wardSpecular(
  vec3 lightDirection,
  vec3 viewDirection,
  vec3 surfaceNormal,
  vec3 fiberParallel,
  vec3 fiberPerpendicular,
  float shinyParallel,
  float shinyPerpendicular) {

  float NdotL = dot(surfaceNormal, lightDirection);
  float NdotR = dot(surfaceNormal, viewDirection);

  if(NdotL < 0.0 || NdotR < 0.0) {
    return 0.0;
  }

  vec3 H = normalize(lightDirection + viewDirection);

  float NdotH = dot(surfaceNormal, H);
  float XdotH = dot(fiberParallel, H);
  float YdotH = dot(fiberPerpendicular, H);

  float coeff = sqrt(NdotL/NdotR) / (4.0 * PI * shinyParallel * shinyPerpendicular); 
  float theta = (pow(XdotH/shinyParallel, 2.0) + pow(YdotH/shinyPerpendicular, 2.0)) / (1.0 + NdotH);

  return coeff * exp(-2.0 * theta);
}

// This is the same function that appeared in the vertex shader for
// per-vertex lighting. Now it is being calculated for each fragment
// individually.
vec3 phongPointSpecLight(in vec4 position, in vec3 norm)
{
    vec3 s = normalize(vec3(point_light_pos - position));
    vec3 v = normalize(-position.xyz);
    vec3 r = reflect( -s, norm );
    
    vec3 ambient = point_light_ambient * mtl_ambient;
	
    // The diffuse component
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = point_light_diffuse * mtl_diffuse * sDotN;

    // The specular component
    vec3 spec = vec3(0.0);
    if ( sDotN > 0.0 )
		spec = point_light_specular * mtl_specular *
            pow( max( dot(r,v), 0.0 ), shininess );

    return spec;
}

// This is the same function that appeared in the vertex shader for
// per-vertex lighting. Now it is being calculated for each fragment
// individually.
vec3 phongDirectionalLight(in vec4 position, in vec3 norm)
{
    vec3 s = normalize(vec3(lightDir));
    vec3 v = normalize(-position.xyz);
    vec3 r = reflect( -s, norm );
    
    vec3 ambient = directional_light_ambient * mtl_ambient;
	
    // The diffuse component
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = directional_light_diffuse * mtl_diffuse * sDotN;

    // The specular component
    vec3 spec = vec3(0.0);
    if ( sDotN > 0.0 )
		spec = directional_light_specular * mtl_specular *
            pow( max( dot(r,v), 0.0 ), shininess );

    return ambient + diffuse;// + spec;
    //return diffuse;
}

vec3 emissionLight(in vec4 position, in vec3 norm)
{
    vec3 emission=vec3(1.0f);

    return emission;
}


vec3 nn;
float ws;
void main(void) {
	normal;
	vertex;
	//nn = normalize(cross(normalize(tangent),cross(normalize(normal),normalize(tangent))));
	//vec2 st2 = vec2(1,1) - vec2(-1,1)*st; 
		// We now just have to set the already smoothed colour as our frag colour
	fragColour = vec4(phongPointDiffuseAmbientLight(vertex, normalize(normal)),1.0f)*on_point;
	fragColour += vec4(phongDirectionalLight(vertex, normalize(normal)),1.0f)*(1.0 - on_directional)*(1.0 - on_wireframe)*(1.0 * on_emission);
	fragColour += vec4(emissionLight(vertex, normalize(normal)),1.0f)*on_emission;
	fragColour += vec4(phongPointSpecLight(vertex, normalize(normal)),1.0f)*on_point;
	
        fragColour *= pow(vec4(color,1.0f)*texture(texMap, st),vec4(vec3(2.2f),1.0f));
	ws = wardSpecular(normalize(vec3(directional_light_direction)),
	normalize(-vertex.xyz),
	normalize(normal),
	normalize(tangent),
	normalize(cross(tangent,normal)),
	0.14,
	1.0)*mtl_specular.x;
	fragColour += vec4(vec3(ws,ws,ws),1.0);
	fragColour = pow(fragColour,vec4(vec3(1.0f/2.2f),1.0f));
	if (on_wireframe > 0.0f){
		fragColour = vec4(color,1.0f);
		
	}
	//fragColour = vec4(0.5f*vec3(tangent + vec3(1.0)),1.0f);
	//fragColour = vec4(vec3(ws,ws,ws),1.0);
	
}
