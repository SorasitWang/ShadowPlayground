#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform mat4 rotation;

out vec4 pos;
out vec4 world;
out vec4 normal;
void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
    normal = rotation * vec4(aNormal,1.0) ;//vec4(mat3(transpose(inverse(model))) * aNormal,1.0);
     world = model * vec4(aPos, 1.0);
     pos = gl_Position*0.5 +0.5;
   }