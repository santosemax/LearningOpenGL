#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float mixValue;

// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    // Results in 80% of texture1 and 20% texture2 mixed
    FragColor = mix(texture(texture1, TexCoord), 
            texture(texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), mixValue);
}
