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

uniform vec2 screenSize; // Screen size passed as a uniform
uniform vec4 objectColor; // Shape color uniform

void main()
{
   vec2 normalizedCoord = gl_FragCoord.xy / screenSize;

   if(normalizedCoord.y > 0.5)
   {
      // Upper shape (original)
      color = objectColor;
   }
   else
   {
      // Inverted fade out
      color = objectColor * (1 - normalizedCoord.y * 2.3);
   }
};