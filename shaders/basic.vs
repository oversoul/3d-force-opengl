#version 300 es
layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aOffset;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;

void main() {
  FragPos = vec3(model * vec4(aPos + aOffset, 1.0));
  Normal = normalize(aPos);
  gl_Position = projection * view * vec4(FragPos, 1.0);



  // vec3 instancePos = aPos + aOffset;
  // FragPos = vec3(model * vec4(instancePos, 1.0));
  // Normal = normalize(instancePos);
  // gl_Position = projection * view * vec4(FragPos, 1.0);
}
