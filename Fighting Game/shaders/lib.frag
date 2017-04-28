const float pi = 3.14159265358979323846;
#define F2 0.366025403
#define G2 0.211324865
#define K 0.0243902439 // 1/41

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
    return mod289(((x*34.0)+1.0)*x);
}

float permute(float x) {
    return mod((34.0 * x + 1.0)*x, 289.0);
}

vec2 grad2(vec2 p, float rot) {
    float u = permute(permute(p.x) + p.y) * 0.0243902439 + rot; // Rotate by shift
    u = fract(u) * 6.28318530718; // 2*pi
    return vec2(cos(u), sin(u));
}

float snoise(vec2 v)
{
    const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626,  // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
                     + i.x + vec3(0.0, i1.x, 1.0 ));
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

float srdnoise(in vec2 P, in float rot, out vec2 grad) {
    vec2 Ps = P + dot(P, vec2(F2));
    vec2 Pi = floor(Ps);
    vec2 P0 = Pi - dot(Pi, vec2(G2));
    vec2 v0 = P - P0;
    vec2 i1 = (v0.x > v0.y) ? vec2(1.0, 0.0) : vec2 (0.0, 1.0);
    vec2 v1 = v0 - i1 + G2;
    vec2 v2 = v0 - 1.0 + 2.0 * G2;
    Pi = mod(Pi, 289.0);
    vec3 t = max(0.5 - vec3(dot(v0,v0), dot(v1,v1), dot(v2,v2)), 0.0);
    vec3 t2 = t*t;
    vec3 t4 = t2*t2;
    vec2 g0 = grad2(Pi, rot);
    vec2 g1 = grad2(Pi + i1, rot);
    vec2 g2 = grad2(Pi + 1.0, rot);
    vec3 gv = vec3(dot(g0,v0), dot(g1,v1), dot(g2,v2)); // ramp: g dot v
    vec3 n = t4 * gv;  // Circular kernel times linear ramp
    vec3 temp = t2 * t * gv;
    vec3 gradx = temp * vec3(v0.x, v1.x, v2.x);
    vec3 grady = temp * vec3(v0.y, v1.y, v2.y);
    grad.x = -8.0 * (gradx.x + gradx.y + gradx.z);
    grad.y = -8.0 * (grady.x + grady.y + grady.z);
    grad.x += dot(t4, vec3(g0.x, g1.x, g2.x));
    grad.y += dot(t4, vec3(g0.y, g1.y, g2.y));
    grad *= 40.0;
    return 40.0 * (n.x + n.y + n.z);
}
