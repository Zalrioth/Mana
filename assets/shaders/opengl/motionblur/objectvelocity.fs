#version 410

in vec4 vPosition;
in vec4 vPrevPosition;

out vec2 oVelocity;

void main(void) {
    vec2 a = (vPosition.xy / vPosition.w) * 0.5 + 0.5;
    vec2 b = (vPrevPosition.xy / vPrevPosition.w) * 0.5 + 0.5;
    //oVelocity = abs(a - b);
    oVelocity = (a - b);// * 0.5 + 0.5;

    //oVelocity *= 4;
}