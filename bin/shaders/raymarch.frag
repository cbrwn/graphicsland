#version 410

#define MAX_STEPS 100
#define MAX_DIST 100.0
#define HIT_DIST 0.01

uniform vec3 Color;
uniform vec3 CamPos;
uniform vec3 LightPos;
uniform vec2 Resolution;
uniform float Time;

out vec4 fragColor;

float torusDist(vec3 pos) {
    // temporary torus info
    vec3 torusPos = vec3(0,1,5);
    vec2 torusSize = vec2(1.0,0.3 * (sin(Time*3.8) + 1.3));

    vec3 t = pos - torusPos;

    vec2 q = vec2(length(t.xz)-torusSize.x,t.y);
    return length(q)-torusSize.y;
}

float boxDist(vec3 pos) {
    // temporary box info
    vec3 boxBounds = vec3(0.7);
    vec3 boxPos = vec3(1,boxBounds.y,5);

    vec3 d = abs(pos-boxPos) - boxBounds;

    return length(max(d,0.0)) +
        min( max(d.x, max(d.y, d.z)), 0.0f);
}

float sphereDist(vec3 pos) {
    // temporary sphere info
    vec3 spherePos = vec3(-1,1,5);
    float sphereRad = 1.0;

    // get distance to sphere
    float result = length(pos - spherePos);
    // that is the distance to the center, so subtract the radius
    result -= sphereRad;
    return result;
}

float groundDist(vec3 pos) {
    return pos.y;
}

float getDist(vec3 p) {
    return min(
        max(sphereDist(p), -torusDist(p)), 
        min(groundDist(p),
            max(boxDist(p), -torusDist(p))
        )
    );
}

float rayMarch(vec3 ro, vec3 rd) {
    float result = 0.0;

    for(int i = 0; i < MAX_STEPS; ++i) {
        // the position of the sphere we are marching from
        vec3 p = ro + (rd * result);

        // get the distance to our objects
        result += getDist(p);

        if(result > MAX_DIST || result <= HIT_DIST) break;
    }

    return result;
}

vec3 getNormal(vec3 p) {
    float dist = getDist(p);

    vec2 e = vec2(0.01,0);
    vec3 n = vec3(
        dist - getDist(p-e.xyy),
        dist - getDist(p-e.yxy),
        dist - getDist(p-e.yyx)
    );

    return normalize(n);
}

float getLight(vec3 point) {
    vec3 lightDir = normalize(LightPos-point);
    vec3 norm = getNormal(point);

    float cosTheta = clamp(dot(norm, lightDir), 0, 1);

    float rm = rayMarch(point + (norm*HIT_DIST*5.0), lightDir);
    if(rm < length(LightPos-point)) cosTheta *= 0.1;

    return cosTheta;
}

void main() {
    vec2 uv = (gl_FragCoord.xy - (Resolution*0.5))/Resolution.y;

    vec3 rayOrigin = vec3(0,4,0);
    vec3 rayDir = normalize(vec3(uv.xy + vec2(0,-0.6), 1));

    // get distance to intersection
    float march = rayMarch(rayOrigin, rayDir);

    // get light information
    vec3 marchPoint = rayOrigin + (rayDir*march);
    float diffuse = getLight(marchPoint);

    vec3 col = vec3(diffuse);
    col += vec3(0.1);

    fragColor =  vec4(col,1.0);
}