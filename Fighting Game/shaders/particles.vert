in vec2 position;

uniform float time;
uniform vec2 init_vel;
uniform mat3 camera;
uniform mat3 transform;

out vec2 posFrag;

int rand(int x) {
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    return x;
}

void main()
{
    int seed = rand(gl_InstanceID) ^ int(1/fract(transform[0,2] + transform[1,2]));
    int angle = rand(seed);
    int speed = rand(angle);
    
    float theta = float(angle) / pow(2., 20);
    vec2 dir = vec2(cos(theta), sin(theta));
    vec2 vel = dir * (float(speed) / pow(2., 32)) + init_vel + vec2(0, -time);
    
    vec2 pos = position*vec2(.05,.002);
    vec2 vel_dir = normalize(vel);
    pos = mat2(vel_dir.x, vel_dir.y, -vel_dir.y, vel_dir.x) * pos;
    
    pos += time * (vel);
    
    gl_Position = vec4((camera * transform * vec3(pos, 1)).xy, 0, 1);
    posFrag = (transform * vec3(pos, 1)).xy;
    
}
