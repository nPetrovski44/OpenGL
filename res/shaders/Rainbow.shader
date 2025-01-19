#shader vertex
#version 330 core
    
layout(location = 0) in vec4 position;

void main()
{
   gl_Position = position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform vec2 screenSize;

void main()
{
    // Normalize the screen coordinates to [0, 1]
    vec2 normalizedCoord = gl_FragCoord.xy / screenSize;

    // Create a rainbow gradient using sine waves
    float red = sin(normalizedCoord.x * 10.0 + 0.0) * 0.5 + 0.5;
    float green = sin(normalizedCoord.x * 10.0 + 2.0) * 0.5 + 0.5;
    float blue = sin(normalizedCoord.x * 10.0 + 4.0) * 0.5 + 0.5;

    color = vec4(red, green, blue, 1.0);
};