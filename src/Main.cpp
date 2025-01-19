#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

float triangleVertices[6] = {
    -0.5f, -0.5f,
     0.0f,  0.5f,
     0.5f, -0.5f
};

float squareVertices[8] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f,
    -0.5f,  0.5f
};

float rectangleVertices[8] = {
    -0.7f, -0.4f,
     0.7f, -0.4f,
     0.7f,  0.4f,
    -0.7f,  0.4f
};

enum Views 
{
    View1 = 0,
    View2,
    View3,
    MaxViews
};

// Animation state
int currentView = 0;

struct ShaderProgramSources
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSources ParseShader(const std::string& filePath)
{
    std::ifstream stream (filePath);

    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while(getline(stream, line))
    {
        if(line.find("#shader") != std::string::npos)
        {
            if(line.find("vertex") != std::string::npos)
            {
                //set type to vertex
                type = ShaderType::VERTEX;
            }
            else if(line.find("fragment") != std::string::npos)
            {
                //set mode to fragment
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            //push into the correct string stream
            ss[(int)type] << line << '\n';
        }
    }

    return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") <<"shader!" << endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    //unsigned int instead of GLuint for abstraction to different APIs
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}   

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Callback to handle key presses
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        if(key == GLFW_KEY_A)
        {
            currentView = (currentView - 1 + MaxViews) % MaxViews; // Cycle backward
        }
        else if(key == GLFW_KEY_D)
        {
            currentView = (currentView + 1) % MaxViews; // Cycle forward
        }
    }
}

// Function to render based on the current view
void renderCurrentView(unsigned int shader, unsigned int buffer, unsigned int screenWidth, unsigned int screenHeight)
{
    // Get uniform locations
    int screenSizeLocation = glGetUniformLocation(shader, "screenSize");
    int objectColorLocation = glGetUniformLocation(shader, "objectColor");
    float objectColor[4] = {0.5f, 1.0f, 1.0f, 1.0f};

    glUseProgram(shader);
    switch(currentView)
    {
        case View1:
            // Render Triangle
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
    
        case View2:
            glUniform2f(screenSizeLocation, screenWidth, screenHeight);
            
            // Render Square
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            break;
            
        case View3:
            glUniform2f(screenSizeLocation, screenWidth, screenHeight);
            glUniform4fv(objectColorLocation, 1, objectColor);

            // Render Rectangle
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            break;

        default:
            break;
    }
}


int main() 
{
    if (!glfwInit())
    {
        cout << "Failed to initialize GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Prototype", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to open GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    glfwSetKeyCallback(window, key_callback);

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    ShaderProgramSources defaultSource = ParseShader("D:\\OpenGL\\OpenGL\\res\\shaders\\Default.shader");
    ShaderProgramSources rainbowSource = ParseShader("D:\\OpenGL\\OpenGL\\res\\shaders\\Rainbow.shader");
    ShaderProgramSources reflectionSource = ParseShader("D:\\OpenGL\\OpenGL\\res\\shaders\\Reflection.shader");

    unsigned int defaultShader = CreateShader(defaultSource.VertexSource, defaultSource.FragmentSource);
    unsigned int rainbowShader = CreateShader(rainbowSource.VertexSource, rainbowSource.FragmentSource);
    unsigned int reflectionShader = CreateShader(reflectionSource.VertexSource, reflectionSource.FragmentSource);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        switch(currentView)
        {
            case View1:
                renderCurrentView(defaultShader, buffer, 800, 600);
                break;
            
            case View2:
                renderCurrentView(rainbowShader, buffer, 800, 600);
                break;
            
            case View3:
                renderCurrentView(reflectionShader, buffer, 800, 600);
                break;
            
            default:
                break;
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //Delete all shaders
    glDeleteProgram(defaultShader);
    glDeleteProgram(rainbowShader);
    glDeleteProgram(reflectionShader);

    glDeleteBuffers(1, &buffer);
    glfwTerminate();
    return 0;
}