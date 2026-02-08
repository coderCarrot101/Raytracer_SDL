#version 460
layout(location = 0) out vec4 outColor;
layout(location = 1) in vec2 v_pos;

void main() {
    vec2 center = vec2(0.0, 0.0);
    float radius = 0.4;

    // distance from center
    float dist = length(v_pos - center);

    // draw yellow face circle
    if (dist > radius)
    {
        discard; // outside the circle
    }

    vec4 faceColor = vec4(1.0, 1.0, 0.0, 1.0);

    // eyes
    vec2 leftEye  = vec2(-0.15, 0.15);
    vec2 rightEye = vec2( 0.15, 0.15);
    float eyeRadius = 0.05;

    if (length(v_pos - leftEye) < eyeRadius ||
        length(v_pos - rightEye) < eyeRadius)
    {
        faceColor = vec4(0.0, 0.0, 0.0, 1.0);
    }


    outColor = faceColor;
}