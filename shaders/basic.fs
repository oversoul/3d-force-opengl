#version 300 es
precision mediump float;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 Normal;
in vec3 FragPos;


uniform vec3 color;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

void main() {
    // Step 1: Output a fixed color
    vec3 result = vec3(1.0, 0.0, 0.0);  // Bright red

    // Step 2: Uncomment to use the input color
    result = color;

    // Step 3: Uncomment to add very basic lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    result = color * (diff * lightColor + vec3(0.1));  // 0.1 is ambient light

    // Output debug information
    // result = Normal * 0.5 + 0.5;  // Visualize normals
    // result = vec3(length(FragPos) / 10.0);  // Visualize fragment position

    FragColor = vec4(result, 1.0);

    // For now, set BrightColor to black
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
