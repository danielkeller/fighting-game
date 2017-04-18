in vec2 position;

uniform float time;
uniform mat3 camera;
uniform mat3 transform;

out vec2 posFrag;

int rand(int x) {
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    return x;
}

void main()
{
    vec2 pos = position*.01;
    
    int seed = rand(gl_InstanceID) ^ int(1/fract(transform[0,2] + transform[1,2]));
    int angle = rand(seed);
    int vel = rand(angle);
    
    float theta = float(angle) / pow(2., 20);
    vec2 dir = vec2(cos(theta), sin(theta));
    
    pos += dir * time * (float(vel) / pow(2., 32));
    
    float gravity = time * time;
    pos.y -= gravity;
    
    gl_Position = vec4(
                       (camera * transform * vec3(pos, 1)).xy, 0, 1);
    posFrag = (transform * vec3(pos, 1)).xy;
    
}
