in vec2 pos_from, pos_to, deriv_from, deriv_to;
uniform float pos_alpha;

uniform mat3 camera;
uniform mat3 transform;

out vec2 posFrag;
out vec2 framebuffer_coord;

void main()
{
    //Spline calculation
    vec2 a = deriv_from - (pos_to - pos_from),
        b = -deriv_to + (pos_to - pos_from);
    float inv_alpha = 1. - pos_alpha;
    vec2 position = pos_from*inv_alpha + pos_to*pos_alpha
        + inv_alpha*pos_alpha*(a*inv_alpha + b*pos_alpha);
    
    gl_Position = vec4((camera * transform * vec3(position, 1)).xy, 0, 1);
    posFrag = (transform * vec3(position, 1)).xy;
    
    framebuffer_coord = gl_Position.xy / 2. + 0.5;
}
