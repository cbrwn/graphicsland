#version 440

#define MAX_STEPS 100
#define MAX_DIST 100.0
#define HIT_DIST 0.01

uniform vec3 Color;
uniform mat4 CamTransform;
uniform vec3 LightPos;
uniform vec2 Resolution;
uniform float Time;

out vec4 fragColor;

vec4 minw(vec4 a, vec4 b) {
	if(a.w < b.w)
		return a;
	return b;
}

vec4 maxw(vec4 a, vec4 b) {
	if(a.w > b.w)
		return a;
	return b;
}

vec4 torusDist(vec3 pos, vec3 torusPos) {
    vec2 torusSize = vec2(1.0,0.3 * (sin(Time*2.8) + 2.3));

    vec3 t = pos - torusPos;

    vec2 q = vec2(length(t.xz)-torusSize.x,t.y);
    float dist = length(q)-torusSize.y;
	return vec4(1,1,1,dist);
}

vec4 boxDist(vec3 pos) {
    // temporary box info
    vec3 boxBounds = vec3(0.7, 0.7, 0.7);
    vec3 boxPos = vec3(1,boxBounds.y+0.05,5);

	vec3 c = vec3(3,0,7);
    vec3 d = abs(mod(pos-boxPos, c) - 0.5*c) - boxBounds;

    float dist = length(max(d,0.0)) +
        min( max(d.x, max(d.y, d.z)), 0.0f);

	return vec4(1, 0.4, 0.5, dist - sin(Time*11 + pos.x)*0.5);
}

vec4 sphereDist(vec3 pos, vec3 spherePos) {
    float sphereRad = 1.0;

    // get distance to sphere
    float result = length(pos - spherePos);
    // that is the distance to the center, so subtract the radius
    result -= sphereRad;
	return vec4(0.4, 0.5, 1.0, result);
}

vec4 groundDist(vec3 pos) {
	vec3 col = vec3(0.5, 1.0, 0.4);

	col += vec3((sin(pos.x*6) * cos(pos.z*8.3)))*0.04;

	return vec4(col, pos.y);
}

vec4 getDist(vec3 p) {
    return minw(
        maxw(sphereDist(p, vec3(0,1,5)), -torusDist(p, vec3(0,2,5))), 
        minw(groundDist(p),
            minw(boxDist(p),
				maxw(sphereDist(p, vec3(3,1,5)), torusDist(p, vec3(3,2,5) ) )
        ))
    );
}

vec4 rayMarch(vec3 ro, vec3 rd) {
	vec4 result = vec4(0);

    for(int i = 0; i < MAX_STEPS; ++i) {
        // the position of the sphere we are marching from
        vec3 p = ro + (rd * result.w);

        // get the distance to our objects
		float w = result.w;
		result = getDist(p);
		result.w += w;
        //result += getDist(p);

        if(result.w > MAX_DIST) {
			result = vec4(0,0.6,0.9,MAX_DIST+1);
			break;
		}
		if(result.w <= HIT_DIST) break;
    }

    return result;
}

vec3 getNormal(vec3 p) {
    float dist = getDist(p).w;

    vec2 e = vec2(0.01,0);
    vec3 n = vec3(
        dist - getDist(p-e.xyy).w,
        dist - getDist(p-e.yxy).w,
        dist - getDist(p-e.yyx).w
    );

    return normalize(n);
}

float getLight(vec3 point) {
    vec3 lightDir = normalize(LightPos-point);
    vec3 norm = getNormal(point);

    float cosTheta = clamp(dot(norm, lightDir), 0, 1);

    float rm = rayMarch(point + (norm*HIT_DIST*5.0), lightDir).w;
    if(rm < length(LightPos-point)) cosTheta *= 0.1;

    return cosTheta;
}

void main() {
    vec2 uv = (gl_FragCoord.xy - (Resolution*0.5))/Resolution.y;

    vec3 rayOrigin = CamTransform[3].xyz;

    vec3 rayDir = CamTransform[2].xyz + (uv.x * CamTransform[0].xyz) + (uv.y * CamTransform[1].xyz);

    // get distance to intersection
    vec4 march = rayMarch(rayOrigin, rayDir);

    // get light information
    vec3 marchPoint = rayOrigin + (rayDir*march.w);
    float diffuse = getLight(marchPoint);

    vec3 col = march.xyz * diffuse;
    col += march.xyz * 0.1;

	if(march.w > MAX_DIST)
		col = march.xyz;

    fragColor =  vec4(col,1.0);
}
