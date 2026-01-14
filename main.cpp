// template based on material from learnopengl.com
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <cmath>



#include "shaderHandle.h"
#include "objHandle.h"
#include "matrices.h"
#include "textureLoad.h"

using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);


bool rotating = false;
float rotationAngle = 0.0f;

float translateX = 0.0f;
float translateY = 0.0f;
float translateZ = 0.0f;
float scale = 1.0f;

float cameraX = 2.0f, cameraY = 1.0f, cameraZ = -15.0f;

bool upPressed = false;
bool downPressed = false;
bool leftPressed = false;
bool rightPressed = false;
bool pageUpPressed = false;
bool pageDownPressed = false;
bool plusPressed = false;
bool minusPressed = false;

int renderSetting = 1;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;



shaderHandle vertexShader = shaderHandle();
shaderHandle fragmentShader = shaderHandle();



int main()
{

    // load up the shaders and parse the obj files
    vertexShader.load("source.vs");
    fragmentShader.load("source.fs");

    const char *vertexShaderSource = vertexShader.program.data();
    const char *fragmentShaderSource = fragmentShader.program.data();

    OBJParser parser1;
    if (parser1.parse("head.obj")) {
        cout << "head loaded" << endl;
        parser1.printStats();

    }

    OBJParser parser2;
    if (parser2.parse("Cat.obj")) {
        cout << "cat loaded" << endl;
        parser2.printStats();
    }

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "viewGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // // glew: load all OpenGL function pointers
    glewInit();

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------


    const vector<float>& cowVertices = parser1.getVertices();
    const vector<float>& eagleVertices = parser2.getVertices();






    unsigned int VBO1, VBO2, VAO1, VAO2;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO1);

    // bind VBO1 (cow) -> send data to buffer

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, cowVertices.size()*sizeof(float), cowVertices.data(), GL_STATIC_DRAW);

    // 3 pointers, vertex, normal, color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //uv attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // unbind
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // generate vertex arrays for eagle
    // bind VBO2 -> send data to GPU
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, eagleVertices.size()*sizeof(float), eagleVertices.data(), GL_STATIC_DRAW);
    // define pointers again
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // unbind it
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);







    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // render loop
    // -----------

    unsigned int texture1 = loadTexture("missingTexture.png");


    bool spacePressed = false;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
            rotating = !rotating;
            spacePressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spacePressed = false;
        }

        // translation
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            translateY -= 0.05f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            translateY += 0.05f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            translateX -= 0.05f;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            translateX += 0.05f;
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
            translateZ += 0.05f;
        }
        if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
            translateZ -= 0.05f;
        }

        // scale
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
            scale += 0.01f;
            if (scale > 5.0f) scale = 5.0f;  // Max scale
            }
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
            scale -= 0.01f;
            if (scale < 0.1f) scale = 0.1f;  // Min scale
            }

        // reset
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            translateX = translateY = translateZ = 0.0f;
            scale = 1.0f;
            rotationAngle = 0.0f;
            renderSetting = 1;
            rotating = false;
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            renderSetting = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            renderSetting = 2;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            renderSetting = 3;
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            renderSetting = 4;
        }

        // rotate
        if (rotating) {
            rotationAngle += 0.01f; // this is speed in raidans
            if (rotationAngle > 2 * M_PI) {
                rotationAngle -= 2 * M_PI;
            }
        }


        // render
        // ------
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





    //Defining matrices to send to GPU
        Mat4 view = lookAt(
            cameraX, cameraY, cameraZ,
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f);


        Mat4 projection = perspective(
                45.0f * M_PI / 180.0f,  // FOV in radians
                 800.0f / 600.0f,         // Aspect ratio
                 0.1f,                    // Near plane
                 100.0f                   // Far plane
);

        Mat4 model1 = Mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            -5, 0, 0, 1

        );
        Mat4 model2 = Mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            5, 0, 0, 1
            );


        float cosA = cos(rotationAngle);
        float sinA = sin(rotationAngle);

        Mat4 rotationMatrix1(
        cosA,0, -sinA, 0,
        0, 1, 0, 0,
         sinA, 0, cosA, 0,
        0, 0, 0, 1);

        Mat4 rotationMatrix2(
            1, 0, 0, 0,
            0, -cosA, sinA, 0,
            0, -sinA, -cosA, 0,
            0, 0, 0, 1);



        Mat4 translateMatrix(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            translateX, translateY, translateZ, 1
            );

        Mat4 scaleMatrix(
            scale, 0, 0, 0,
            0, scale, 0, 0,
            0, 0, scale, 0,
            0, 0, 0, 1);


        // experiment with doing matrix calculations on CPU -- not as good

        // model1 = multiplyMatrices(model1.m, rotationMatrix.m);
        // model2 = multiplyMatrices(model2.m, rotationMatrix.m);
        //
        // model1 = multiplyMatrices(model1.m, translateMatrix.m);
        // model2 = multiplyMatrices(model2.m, translateMatrix.m);
        //
        // model1 = multiplyMatrices(model1.m, scaleMatrix.m);
        // model2 = multiplyMatrices(model2.m, scaleMatrix.m);

        // holy uniform
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "translate"), 1, GL_FALSE, translateMatrix.m);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "scale"), 1, GL_FALSE, scaleMatrix.m);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "rotate1"), 1, GL_FALSE, rotationMatrix1.m);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "rotate2"), 1, GL_FALSE, rotationMatrix2.m);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, model1.m);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view.m);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection.m);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 0.0, 25.0f, 2.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightDir"), -0.5f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.0f, 0.50f, 0.5f);

        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraX, cameraY, cameraZ);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), cameraX, cameraY, cameraZ);
        glUniform1f(glGetUniformLocation(shaderProgram, "ambientStrength"), 0.2f);
        glUniform1f(glGetUniformLocation(shaderProgram, "specularStrength"), 1.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), 16.0f);

        glUniform1i(glGetUniformLocation(shaderProgram, "renderSetting"), renderSetting);



        // draw our first triangle
        glUseProgram(shaderProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);





        glBindVertexArray(VAO1); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, parser1.getVertexCount());
        glBindVertexArray(0); // unbind our VA no need to unbind it every time


        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, model2.m);

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, parser2.getVertexCount());
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteProgram(shaderProgram);




    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {




    }

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
