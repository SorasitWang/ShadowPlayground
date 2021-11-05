#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

out vec4 world;
out vec4 pos;
out float depth;
void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
    pos = gl_Position*0.5 +0.5;
    world = model * vec4(aPos, 1.0);

   }