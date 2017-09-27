out vec4 color;
uniform sampler2DRect framebuffer;
in vec2 posFrag;
uniform float time;
float snoise(vec3 v);

float hash(vec3 p)
{
    p  = fract( p*0.3183099+.1 );
    p *= 17.0;
    return fract( p.x*p.y*p.z*(p.x+p.y+p.z) );
}

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
    
    return mix(mix(mix( hash(p+vec3(0,0,0)),
                       hash(p+vec3(1,0,0)),f.x),
                   mix( hash(p+vec3(0,1,0)),
                       hash(p+vec3(1,1,0)),f.x),f.y),
               mix(mix( hash(p+vec3(0,0,1)),
                       hash(p+vec3(1,0,1)),f.x),
                   mix( hash(p+vec3(0,1,1)),
                       hash(p+vec3(1,1,1)),f.x),f.y),f.z);
}

const mat3 m = mat3( 0.00,  0.80,  0.60,
                    -0.80,  0.36, -0.48,
                    -0.60, -0.48,  0.64 );

float fractnoise(in vec3 pos) {
    vec3 q = pos;
    float f  = 0.5000*noise( q ); q = m*q*2.01;
          f += 0.2500*noise( q ); q = m*q*2.02;
          f += 0.1250*noise( q ); q = m*q*2.03;
          f += 0.0625*noise( q ); q = m*q*2.01;
    return f;
}

void main()
{
    //ivec2 init = ivec2(gl_FragCoord.xy+23462232);
    //init = rand(rand(~rand(init))*ivec2(time*50, time*73));
    
    color = texture(framebuffer, gl_FragCoord.xy) + vec4(1,1,1,0) // *float(init.x+init.y) / pow(2., 32)
        * smoothstep(.4, .5, fractnoise(vec3(gl_FragCoord.xy*.1, time)));
}

