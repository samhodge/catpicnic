

#version 150

// "Seascape" by Alexander Alekseev aka TDM - 2014
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.




int NUM_STEPS = 8;

float PI	 	= 3.1415;
float EPSILON	= 1e-3;
float EPSILON_NRM	= 0.1;

// sea
int ITER_GEOMETRY = 3;
int ITER_FRAGMENT = 5;
float SEA_HEIGHT = 0.12313212;
float SEA_CHOPPY = 0.521312;
float SEA_SPEED = 3.2;
float SEA_FREQ = 0.32;
vec3 SEA_BASE = vec3(0.1,0.19,0.22);
vec3 SEA_WATER_COLOR = vec3(0.8,0.9,0.6);


vec3 refract (vec3  I, vec3  N, float etaRatio)

{

  float cosI = dot(-I, N);

  float cosT2 = 1.0 - (etaRatio * etaRatio );
  cosT2 *= ( 1.0 - ( cosI * cosI ) );

  vec3 T = etaRatio * I + ((etaRatio * cosI - sqrt(abs(cosT2))) * N);

  return T * vec3(min(0,cosT2));
  //return vec3(0.0,0.0,0.0);

}

in float outtime;
in mat3 localSurface2World;

float SEA_TIME = outtime * SEA_SPEED;

mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);

// math
mat3 fromEuler(vec3 ang) {
	vec2 a1 = vec2(sin(ang.x),cos(ang.x));
    vec2 a2 = vec2(sin(ang.y),cos(ang.y));
    vec2 a3 = vec2(sin(ang.z),cos(ang.z));
    mat3 m;
    m[0] = vec3(a1.y*a3.y+a1.x*a2.x*a3.x,a1.y*a2.x*a3.x+a3.y*a1.x,-a2.y*a3.x);
	m[1] = vec3(-a2.y*a1.x,a1.y*a2.y,a2.x);
	m[2] = vec3(a3.y*a1.x*a2.x+a1.y*a3.x,a1.x*a3.x-a1.y*a3.y*a2.x,a2.y*a3.y);
	return m;
}
float hash( vec2 p ) {
	float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}
float noise( in vec2 p ) {
    vec2 i = floor( p );
    vec2 f = fract( p );	
	vec2 u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ), 
                     hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ), 
                     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

// lighting
float diffuse(vec3 n,vec3 l,float p) {
    return pow(dot(n,l) * 0.4 + 0.6,p);
}
float specular(vec3 n,vec3 l,vec3 e,float s) {    
    float nrm = (s + 8.0) / (3.1415 * 8.0);
    return pow(max(dot(reflect(e,n),l),0.0),s) * nrm;
}

// sky
vec3 getSkyColor(vec3 e) {
    e.y = max(e.y,0.0);
    vec3 ret;
    ret.x = pow(1.0-e.y,2.0);
    ret.y = 1.0-e.y;
    ret.z = 0.6+(1.0-e.y)*0.4;
    return ret;
}

// sea
float sea_octave(vec2 uv, float choppy) {
    uv += noise(uv);        
    vec2 wv = 1.0-abs(sin(uv));
    vec2 swv = abs(cos(uv));    
    wv = mix(wv,swv,wv);
    return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}

float map(vec3 p) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    vec2 uv = p.xz; uv.x *= 0.75;
    
    float d, h = 0.0;    
    for(int i = 0; i < ITER_GEOMETRY; i++) {        
    	d = sea_octave((uv+SEA_TIME)*freq,choppy);
    	d += sea_octave((uv-SEA_TIME)*freq,choppy);
        h += d * amp;        
    	uv *= octave_m; freq *= 1.9; amp *= 0.22;
        choppy = mix(choppy,1.0,0.2);
    }
    return p.y - h;
}

float map_detailed(vec3 p) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    vec2 uv = p.xz; uv.x *= 0.75;
    
    float d, h = 0.0;    
    for(int i = 0; i < ITER_FRAGMENT; i++) {        
    	d = sea_octave((uv+SEA_TIME)*freq,choppy);
    	d += sea_octave((uv-SEA_TIME)*freq,choppy);
        h += d * amp;        
    	uv *= octave_m; freq *= 1.9; amp *= 0.22;
        choppy = mix(choppy,1.0,0.2);
    }
    return h - p.y;
}

vec3 getSeaColor(vec3 p, vec3 n, vec3 l, vec3 eye, vec3 dist) {  
    float fresnel = 1.0 - max(dot(n,-eye),0.0);
    fresnel = pow(fresnel,3.0) * 0.65;
        
    vec3 reflected = getSkyColor(reflect(eye,n));    
    vec3 refracted = SEA_BASE + diffuse(n,l,80.0) * SEA_WATER_COLOR * 0.12; 
    
    vec3 color = mix(refracted,reflected,fresnel);
    
    float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);
    color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;
    
    color += vec3(specular(n,l,eye,60.0));
    
    return color;
}

// tracing
vec3 getNormal(vec3 p, float eps) {
    vec3 n;
    n.y = map_detailed(p);    
    n.x = map_detailed(vec3(p.x+eps,p.y,p.z)) - n.y;
    n.z = map_detailed(vec3(p.x,p.y,p.z+eps)) - n.y;
    n.y = eps;
    return normalize(n);
}

float heightMapTracing(vec3 ori, vec3 dir, out vec3 p) {  
    float tm = 0.0;
    float tx = 1000.0;    
    float hx = map(ori + dir * tx);
    if(hx > 0.0) return tx;   
    float hm = map(ori + dir * tm);    
    float tmid = 0.0;
    for(int i = 0; i < NUM_STEPS; i++) {
        tmid = mix(tm,tx, hm/(hm-hx));                   
        p = ori + dir * tmid;                   
    	float hmid = map(p);
		if(hmid < 0.0) {
        	tx = tmid;
            hx = hmid;
        } else {
            tm = tmid;
            hm = hmid;
        }
    }
    return tmid;
}
//in vec4 tex_coord;
in vec2 uv_coord;
in vec4 direction;
in vec4 position;
in vec4 ori;
in vec4 light;
uniform samplerCube cube_texture;

out vec4 fragColor;


				
void main( void ) {
    
    float x = fract(uv_coord.s * 100.0);
    float y = fract(uv_coord.t * 100.0);
    float f =  smoothstep(0.425, 0.45, x) - smoothstep(0.475, 0.5, x);
    float g = smoothstep(0.425, 0.45, y) - smoothstep(0.475, 0.5, y);

           
	
	
	//fragColor = texture(cube_texture,normalize(vec3(tex_coord)));
	//fragColor *= vec4(uv.s,uv.t,1.0f,1.0f);

	//uv = uv * 2.0 - 1.0;
    //uv.x *= iResolution.x / iResolution.y;    
    float time = outtime* SEA_TIME;
        
    // ray
    //vec3 ang = vec3(direction);    
    vec3 eye = vec3(ori);
    vec3 dir = normalize(vec3(position) - vec3(ori));
    /*
    vec3 dir = normalize(vec3(direction)); 
    dir.z += length(position) * 0.15;
    //dir = normalize(dir) * fromEuler(ang);

    //dir.z += length(position) * 0.15;
    // tracing
    vec3 p;
    heightMapTracing(ori,dir,p);
    vec3 dist = p - ori;
    vec3 n = getNormal(p, dot(dist,dist) * EPSILON_NRM);
    vec3 light2 = normalize(vec3(light)); 
             
    // color
    vec3 color = getSeaColor(p,n,light2,dir,dist);
    // post
	fragColor = vec4(pow(color,vec3(0.75)), 1.0);
 */  
    
	float height = 1.0 + map(vec3(position));
	vec3 ocean_normal = getNormal(vec3(position)+vec3(10000.0,0.0,0.0) ,EPSILON_NRM);// + vec3(1.0,1.0,1.0))*vec3(0.5,0.5,0.5);
        vec3 env_lookup = reflect(normalize(dir),normalize(ocean_normal));
	//vec3 temp = vec3(0.5*(vec3(1.0,1.0,1.0) + env_lookup));
        //fragColor = vec4(temp.x,temp.y,temp.z,1.0);
	
        //fragColor = vec4(env_lookup,1.0);
	//fragColor=texture(cube_texture,normalize(vec3(env_lookup)));
	//fragColor=vec4(w,w,w,1.0);
        float facingRatio = max(0.0,1-dot(normalize(ocean_normal),normalize(dir)));
        fragColor=mix(0.2*texture(cube_texture,normalize(vec3(ocean_normal))),0.0*texture(cube_texture,normalize(vec3(ocean_normal))),vec4(facingRatio,facingRatio,facingRatio,1.0));
        fragColor+=mix(0.8*texture(cube_texture,normalize(vec3(env_lookup))),0.9*texture(cube_texture,normalize(vec3(env_lookup))),vec4(vec3(facingRatio,facingRatio,facingRatio),1.0));
        //fragColor+=vec4(facingRatio,facingRatio,facingRatio,1.0);
}
