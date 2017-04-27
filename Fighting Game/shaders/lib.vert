vec2 spline(vec2 a, vec2 da, vec2 b, vec2 db, float alpha)
{
    vec2 x = da - (b - a),
    y = -db + (b - a);
    float inv_alpha = 1. - alpha;
    return a*inv_alpha + b*alpha + inv_alpha*alpha*(x*inv_alpha + y*alpha);
}
