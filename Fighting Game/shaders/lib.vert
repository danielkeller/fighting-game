vec2 spline(vec2 a, vec2 da, vec2 b, vec2 db, float alpha)
{
    vec2 x = da - (b - a),
    y = -db + (b - a);
    float inv_alpha = 1. - alpha;
    return a*inv_alpha + b*alpha + inv_alpha*alpha*(x*inv_alpha + y*alpha);
}

vec2 spline(vec2 a, vec2 da, vec2 b, vec2 db, float alpha, out vec2 deriv)
{
    vec2 x = da - (b - a),
    y = -db + (b - a);
    float inv_alpha = 1. - alpha,
        d_alpha1 = 2. - 3.*alpha,
        d_alpha2 = 1. - 2.*alpha;
    
    deriv = b - a + d_alpha2*x + d_alpha1*(y*alpha - x*alpha);
    return a*inv_alpha + b*alpha + inv_alpha*alpha*(x*inv_alpha + y*alpha);
}
