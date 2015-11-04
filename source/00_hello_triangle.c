#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//GLfloat vertices[] = {
//    -0.5f, -0.5f, 0.0f,
//     0.5f, -0.5f, 0.0f,
//     0.0f,  0.5f, 0.0f
//};

//GLfloat vertices[] = {
//    0.5f,  0.5f, 0.0f,
//    0.5f, -0.5f, 0.0f,
//   -0.5f, -0.5f, 0.0f,
//   -0.5f,  0.5f, 0.0f
//};
//GLuint indices[] = {
//    0, 1, 3,
//    1, 2, 3
//};

GLfloat vertices[] = {
    -0.5f,  0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f
};

const GLchar* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "\n"
    "void main() {\n"
    "   gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}\n";

const GLchar* orange_fragmentShaderSource = "#version 330 core\n"
    "out vec4 color;\n"
    "void main() {\n"
    "   color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n";

const GLchar* yellow_fragmentShaderSource = "#version 330 core\n"
    "out vec4 color;\n"
    "void main() {\n"
    "   color = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
    "}\n";

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    (void) scancode;
    (void) mode;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(void) {
    GLFWwindow* window;

    if(!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    //window = glfwCreateWindow(800, 600, "Hello Triangle", NULL, NULL);
    window = glfwCreateWindow(800, 600, "Hello Triangle WIN", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetKeyCallback(window, key_callback);

    GLint success;
    GLchar infoLog[512];

    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr,"Error: Vertex shader failed with:\n  %s",infoLog);
    }

    GLuint orange_framgentShader;
    orange_framgentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(orange_framgentShader, 1, &orange_fragmentShaderSource, NULL);
    glCompileShader(orange_framgentShader);
    glGetShaderiv(orange_framgentShader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(orange_framgentShader, 512, NULL, infoLog);
        fprintf(stderr,"Error: Fragment shader failed with:\n %s",infoLog);
    }

    GLuint yellow_framgentShader;
    yellow_framgentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(yellow_framgentShader, 1, &yellow_fragmentShaderSource, NULL);
    glCompileShader(yellow_framgentShader);
    glGetShaderiv(yellow_framgentShader, GL_COMPILE_STATUS, &success);

    if(!success) {
        glGetShaderInfoLog(yellow_framgentShader, 512, NULL, infoLog);
        fprintf(stderr,"Error: Fragment shader failed with:\n %s",infoLog);
    }

    GLuint orange_shaderProrgam;
    orange_shaderProrgam = glCreateProgram();

    glAttachShader(orange_shaderProrgam, vertexShader);
    glAttachShader(orange_shaderProrgam, orange_framgentShader);
    glLinkProgram(orange_shaderProrgam);

    glGetProgramiv(orange_shaderProrgam, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(orange_shaderProrgam, 512, NULL, infoLog);
        fprintf(stderr,"Error: Shader linkage failed with:\n  %s",infoLog);
    }

    GLuint yellow_shaderProrgam;
    yellow_shaderProrgam = glCreateProgram();

    glAttachShader(yellow_shaderProrgam, vertexShader);
    glAttachShader(yellow_shaderProrgam, yellow_framgentShader);
    glLinkProgram(yellow_shaderProrgam);

    glGetProgramiv(yellow_shaderProrgam, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(yellow_shaderProrgam, 512, NULL, infoLog);
        fprintf(stderr,"Error: Shader linkage failed with:\n  %s",infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(orange_framgentShader);
    glDeleteShader(yellow_framgentShader);

    // First triangle.
    GLuint VAO1;
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);

    GLuint VBO1;
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //GLuint EBO;
    //glGenBuffers(1, &EBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //Second Triangle.
    GLuint VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(3*3*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while(!glfwWindowShouldClose(window)) {
        /* Poll and process events */
        glfwPollEvents();

        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(orange_shaderProrgam);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUseProgram(yellow_shaderProrgam);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

    }

    glfwTerminate();
    return 0;
}
