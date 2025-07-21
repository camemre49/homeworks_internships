#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <ft2build.h>

#include "callbackHandler.h"
#include "GameState.h"
#include "GameConstants.h"
using namespace std;

#include FT_FREETYPE_H
#define BUFFER_OFFSET(i) ((char*)NULL + (i))

/**
 * Program Variables
 */
GLuint gProgram[3];
int gWidth = 600, gHeight = 1000;
GLuint gVertexAttribBuffer, gTextVBO, gIndexBuffer;
int gVertexDataSizeInBytes, gNormalDataSizeInBytes;
int gTriangleIndexDataSizeInBytes, gLineIndexDataSizeInBytes;

GLint modelingMatrixLoc[2];
GLint viewingMatrixLoc[2];
GLint projectionMatrixLoc[2];
GLint eyePosLoc[2];
GLint lightPosLoc[2];
GLint kdLoc[2];

glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::mat4 modelingMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-0.5, -0.5, -0.5));
glm::vec3 eyePos = glm::vec3(0, INITIAL_EYE_Y_POSITION, INITIAL_EYE_GRID_DISTANCE);
glm::vec3 lightPos= glm::vec3(7+glm::radians(INITIAL_CAMERA_ANGLE), 10, 7);

glm::vec3 kdCubes(0.0, 0.0, 1.0); // Change to blue (R=0, G=0, B=1)
glm::vec3 kdRed(1.0, 0.0, 0.0); // Red color for the 3x3x3 cube

// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};
std::map<GLchar, Character> Characters;

// Modify keyboard function to handle H and K keys
void handleInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        gameState.keyIsPressed = true;
        switch (key) {
            case GLFW_KEY_H:
                handleRotationInput("H", -90.0f);
            break;

            case GLFW_KEY_K:
                handleRotationInput("K", 90.0f);
            break;

            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            gameState.lastKeyPressed = "ESC";
            break;

            case GLFW_KEY_S:
                handleFallingSpeedAdjustment("S");
            break;

            case GLFW_KEY_W:
                handleFallingSpeedAdjustment("W");
            break;

            case GLFW_KEY_A: {
                float angle = glm::radians(gameState.targetCameraAngle);
                float deltaX = -std::round(std::cos(angle) * 1000.0f) / 1000.0f;
                float deltaZ = std::round(std::sin(angle) * 1000.0f) / 1000.0f;
                handleMovementInput("A", deltaX, deltaZ);
                break;
            }

            case GLFW_KEY_D: {
                float angle = glm::radians(gameState.targetCameraAngle);
                float deltaX = std::round(std::cos(angle) * 1000.0f) / 1000.0f;
                float deltaZ = -std::round(std::sin(angle) * 1000.0f) / 1000.0f;
                handleMovementInput("D", deltaX, deltaZ);
                break;
            }
        }
        gameState.blockPosition[0] = glm::clamp(gameState.blockPosition[0], -3.0f, 3.0f);
        gameState.blockPosition[1] = glm::clamp(gameState.blockPosition[1], -3.0f, 3.0f);
    }
    else if (action == GLFW_RELEASE) {
        gameState.keyIsPressed = false;
        gameState.lastKeyPressed = "";
    }
}

void updateCamera() {
    // Handle Camera Rotation
    if (gameState.cameraAngle != gameState.targetCameraAngle) {
        float diff = gameState.targetCameraAngle - gameState.cameraAngle;
        gameState.cameraAngle += gameState.rotationSpeed * (diff > 0 ? 1 : -1);

        float radians = glm::radians(gameState.cameraAngle);
        float distance = INITIAL_EYE_GRID_DISTANCE;
        eyePos.x = distance * sin(radians);
        eyePos.z = distance * cos(radians);
        eyePos.y = INITIAL_EYE_Y_POSITION;

        // Rotate light with camera
        float lightDistance = INITIAL_LIGHT_GRID_DISTANCE;
        lightPos.x = lightDistance * sin(radians);
        lightPos.z = lightDistance * cos(radians);
        lightPos.y = INITIAL_LIGHT_Y_POSITION;
    }

    // Handle Shake Effect
    if (gameState.shakeActive) {
        gameState.shakeTimer += 0.004f;
        float progress = gameState.shakeTimer / SHAKE_DURATION;

        if (progress >= 1.0f) {
            gameState.shakeActive = false;
            gameState.shakeTimer = 0.0f;
            // Back to the current position
            float radians = glm::radians(gameState.cameraAngle);
            float distance = INITIAL_EYE_GRID_DISTANCE;
            eyePos.x = distance * sin(radians);
            eyePos.z = distance * cos(radians);
            eyePos.y = INITIAL_EYE_Y_POSITION;
        } else {
            // Smooth shake effect with decay
            float shakeIntensity = (1.0f - progress) * SHAKE_MAX_INTENSITY;
            float shakeX = shakeIntensity * sin(gameState.shakeTimer * SHAKE_FREQUENCY);
            float shakeY = shakeIntensity * 4 * cos(gameState.shakeTimer * SHAKE_FREQUENCY);
            float shakeZ = shakeIntensity * sin(gameState.shakeTimer * SHAKE_FREQUENCY);
            eyePos.x += shakeX;
            eyePos.y += shakeY;
            eyePos.z += shakeZ;
        }
    }

    // Update Viewing Matrix
    viewingMatrix = glm::lookAt(eyePos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    for (int i = 0; i < 2; ++i) {
        glUseProgram(gProgram[i]);
        glUniformMatrix4fv(viewingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
        glUniform3fv(eyePosLoc[i], 1, glm::value_ptr(eyePos));
        glUniform3fv(lightPosLoc[i], 1, glm::value_ptr(lightPos));
    }
}

void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state
    glUseProgram(gProgram[2]);
    glUniform3f(glGetUniformLocation(gProgram[2], "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)

        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void renderGameText() {
    std::string viewText = gameState.currentView;
    renderText(viewText, 10, gHeight - 30, 0.5f, glm::vec3(1, 1, 0));
    if (gameState.keyIsPressed && !gameState.lastKeyPressed.empty()) {
        renderText(gameState.lastKeyPressed, 10, gHeight - 60, 0.5f, glm::vec3(1, 0, 0));
    }
    // Render points
    std::string pointsText = "Points: " + std::to_string(gameState.points);
    float charWidth = 16.2f;
    int pointsOffset = pointsText.length() * charWidth;
    renderText(pointsText, gWidth - pointsOffset, gHeight - 30, 0.5f, glm::vec3(1, 1, 0));
    // Render game over text if needed
    if (gameState.gameOver) {
        renderText("GAME OVER", gWidth/2 - 165, gHeight/2, 1.0f, glm::vec3(1, 0, 0));
    }
}

void drawCube();
void drawCubeEdges();
void drawCubeWithEdges(const glm::mat4& cubeMatrix);

/**
 * Draw the cubes
 */
void display() {
    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Update camera
    updateCamera();

    // Draw the blue grid
    glUseProgram(gProgram[0]);
    glUniform3fv(kdLoc[0], 1, glm::value_ptr(kdCubes));
    for (int x = 0; x < 9; x++) {
        for (int z = 0; z < 9; z++) {
            glm::mat4 cubeMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x - 4, -4, z - 4));
            drawCubeWithEdges(cubeMatrix);
        }
    }

    // Draw the red cubes
    glUseProgram(gProgram[0]);
    glUniform3fv(kdLoc[0], 1, glm::value_ptr(kdRed));

    static int frameCount = 0;
    frameCount++;
    // Move block down or land it if (60 / falling speed) is achieved by counting
    int framesNeeded = static_cast<int>(60.0f / gameState.fallingSpeed);
    if (frameCount >= framesNeeded && gameState.fallingSpeed > 0) {
        float nextHeight = gameState.blockHeight - 1.0f;

        // Check if block can move down
        if (gameState.canPlaceBlock(gameState.blockPosition[0], nextHeight, gameState.blockPosition[1])) {
            gameState.blockHeight = nextHeight;
        } else {
            gameState.landBlock();
        }
        frameCount = 0;
    }

    // Draw thrown cubes (from collapsed levels)
    constexpr  float thresholdMin = -30.0f;
    constexpr  float thresholdMax = 30.0f;
    for (auto it = gameState.thrownCubes.begin(); it != gameState.thrownCubes.end(); ) {
        auto& thrownCube = *it;

        // Update cube position based on velocity
        thrownCube.position += thrownCube.velocity * 0.1f; // Adjust time step for speed

        // Check if the cube's position is out of bounds
        if (thrownCube.position.x < thresholdMin || thrownCube.position.x > thresholdMax ||
            thrownCube.position.y < thresholdMin || thrownCube.position.y > thresholdMax ||
            thrownCube.position.z < thresholdMin || thrownCube.position.z > thresholdMax) {

            // Remove the cube if it's out of bounds
            it = gameState.thrownCubes.erase(it);
                continue;
            }

        // Draw cube
        glm::mat4 cubeMatrix = glm::translate(glm::mat4(1.0f), thrownCube.position);
        drawCubeWithEdges(cubeMatrix);

        ++it; // Move to the next cube
    }


    // Draw current falling block if game is not over
    if (!gameState.gameOver) {
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
                for (int z = 0; z < 3; z++) {
                    glm::mat4 cubeMatrix = glm::translate(glm::mat4(1.0f), 
                        glm::vec3(x + gameState.blockPosition[0] - 1,
                                 y + gameState.blockHeight,
                                 z + gameState.blockPosition[1] - 1));
                    drawCubeWithEdges(cubeMatrix);
                }
            }
        }
    }

    // Draw previously placed blocks
    glUseProgram(gProgram[0]);
    glUniform3fv(kdLoc[0], 1, glm::value_ptr(kdRed));
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 9; z++) {
                if (gameState.grid[x][y][z]) {
                    glm::mat4 cubeMatrix = glm::translate(glm::mat4(1.0f), 
                        glm::vec3(x - 4, y - 4, z - 4));
                    drawCubeWithEdges(cubeMatrix);
                }
            }
        }
    }

    renderGameText();
    assert(glGetError() == GL_NO_ERROR);
}

void mainLoop(GLFWwindow* window)
{
    while (!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void initializeGame(GLFWwindow* window) {
    // Initialize game state
    gameState = GameState();
    // Set key callbacks
    glfwSetKeyCallback(window, handleInput);
}

void initializeAndCreateWindow(GLFWwindow* & window);
void initialGLConfiguration();
void initShaders();
void initVBO();
void initFonts(int windowWidth, int windowHeight);
void reshape(GLFWwindow* window, int w, int h);
int main(int argc, char** argv)
{
    // Initialize the window
    GLFWwindow* window = nullptr;
    initializeAndCreateWindow(window);
    // Set initial configurations of the openGL
    initialGLConfiguration();
    // Initialize shaders
    initShaders();
    // Initialize VBO for 1 cube
    initVBO();
    // Initialize fonts
    initFonts(gWidth, gHeight);
    // Set reshape function
    glfwSetWindowSizeCallback(window, reshape);
    reshape(window, gWidth, gHeight); // need to call this once ourselves

    // Initialize the game and add a cube
    initializeGame(window);
    mainLoop(window); // this does not return unless the window is closed

    // Terminate the game
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void initializeAndCreateWindow(GLFWwindow* &window) {
    if (!glfwInit())
    {
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(gWidth, gHeight, "tetrisGL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
    char rendererInfo[512] = {0};
    strcpy(rendererInfo, (const char*) glGetString(GL_RENDERER));
    strcat(rendererInfo, " - ");
    strcat(rendererInfo, (const char*) glGetString(GL_VERSION));
    glfwSetWindowTitle(window, rendererInfo);
}

// For reading GLSL files
bool ReadDataFromFile(
    const string& fileName, ///< [in]  Name of the shader file
    string&       data)     ///< [out] The contents of the file
{
    fstream myfile;

    // Open the input
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            data += curLine;
            if (!myfile.eof())
            {
                data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    return true;
}

void initialGLConfiguration() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Set clear color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // polygon offset is used to prevent z-fighting between the cube and its borders
    glPolygonOffset(0.5, 0.5);
    glEnable(GL_POLYGON_OFFSET_FILL);
}

GLuint createVS(const char* shaderName)
{
    string shaderSource;

    string filename(shaderName);
    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shader, &length);
    glCompileShader(vs);

    char output[1024] = {0};
    glGetShaderInfoLog(vs, 1024, &length, output);
    printf("VS compile log: %s\n", output);

    return vs;
}

GLuint createFS(const char* shaderName)
{
    string shaderSource;

    string filename(shaderName);
    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shader, &length);
    glCompileShader(fs);

    char output[1024] = {0};
    glGetShaderInfoLog(fs, 1024, &length, output);
    printf("FS compile log: %s\n", output);

    return fs;
}

void initShaders()
{
    // Create the programs

    gProgram[0] = glCreateProgram();
    gProgram[1] = glCreateProgram();
    gProgram[2] = glCreateProgram();

    // Create the shaders for both programs

    GLuint vs1 = createVS("vert.glsl"); // for cube shading
    GLuint fs1 = createFS("frag.glsl");

    GLuint vs2 = createVS("vert2.glsl"); // for border shading
    GLuint fs2 = createFS("frag2.glsl");

    GLuint vs3 = createVS("vert_text.glsl");  // for text shading
    GLuint fs3 = createFS("frag_text.glsl");

    // Attach the shaders to the programs

    glAttachShader(gProgram[0], vs1);
    glAttachShader(gProgram[0], fs1);

    glAttachShader(gProgram[1], vs2);
    glAttachShader(gProgram[1], fs2);

    glAttachShader(gProgram[2], vs3);
    glAttachShader(gProgram[2], fs3);

    // Link the programs

    for (int i = 0; i < 3; ++i)
    {
        glLinkProgram(gProgram[i]);
        GLint status;
        glGetProgramiv(gProgram[i], GL_LINK_STATUS, &status);

        if (status != GL_TRUE)
        {
            cout << "Program link failed: " << i << endl;
            exit(-1);
        }
    }


    // Get the locations of the uniform variables from both programs

    for (int i = 0; i < 2; ++i)
    {
        modelingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "modelingMatrix");
        viewingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "viewingMatrix");
        projectionMatrixLoc[i] = glGetUniformLocation(gProgram[i], "projectionMatrix");
        eyePosLoc[i] = glGetUniformLocation(gProgram[i], "eyePos");
        lightPosLoc[i] = glGetUniformLocation(gProgram[i], "lightPos");
        kdLoc[i] = glGetUniformLocation(gProgram[i], "kd");

        glUseProgram(gProgram[i]);
        glUniformMatrix4fv(modelingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
        glUniform3fv(eyePosLoc[i], 1, glm::value_ptr(eyePos));
        glUniform3fv(lightPosLoc[i], 1, glm::value_ptr(lightPos));
        glUniform3fv(kdLoc[i], 1, glm::value_ptr(kdCubes));
    }
}

// VBO setup for drawing a cube and its borders
void initVBO()
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    assert(vao > 0);
    glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	assert(glGetError() == GL_NONE);

	glGenBuffers(1, &gVertexAttribBuffer);
	glGenBuffers(1, &gIndexBuffer);

	assert(gVertexAttribBuffer > 0 && gIndexBuffer > 0);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

    GLuint indices[] = {
        0, 1, 2, // front
        3, 0, 2, // front
        4, 7, 6, // back
        5, 4, 6, // back
        0, 3, 4, // left
        3, 7, 4, // left
        2, 1, 5, // right
        6, 2, 5, // right
        3, 2, 7, // top
        2, 6, 7, // top
        0, 4, 1, // bottom
        4, 5, 1  // bottom
    };

    GLuint indicesLines[] = {
        7, 3, 2, 6, // top
        4, 5, 1, 0, // bottom
        2, 1, 5, 6, // right
        5, 4, 7, 6, // back
        0, 1, 2, 3, // front
        0, 3, 7, 4, // left
    };

    GLfloat vertexPos[] = {
        0, 0, 1, // 0: bottom-left-front
        1, 0, 1, // 1: bottom-right-front
        1, 1, 1, // 2: top-right-front
        0, 1, 1, // 3: top-left-front
        0, 0, 0, // 0: bottom-left-back
        1, 0, 0, // 1: bottom-right-back
        1, 1, 0, // 2: top-right-back
        0, 1, 0, // 3: top-left-back
    };

    GLfloat vertexNor[] = {
         1.0,  1.0,  1.0, // 0: unused
         0.0, -1.0,  0.0, // 1: bottom
         0.0,  0.0,  1.0, // 2: front
         1.0,  1.0,  1.0, // 3: unused
        -1.0,  0.0,  0.0, // 4: left
         1.0,  0.0,  0.0, // 5: right
         0.0,  0.0, -1.0, // 6: back
         0.0,  1.0,  0.0, // 7: top
    };

	gVertexDataSizeInBytes = sizeof(vertexPos);
	gNormalDataSizeInBytes = sizeof(vertexNor);
    gTriangleIndexDataSizeInBytes = sizeof(indices);
    gLineIndexDataSizeInBytes = sizeof(indicesLines);
    int allIndexSize = gTriangleIndexDataSizeInBytes + gLineIndexDataSizeInBytes;

	glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexPos);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, vertexNor);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, allIndexSize, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, gTriangleIndexDataSizeInBytes, indices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, gTriangleIndexDataSizeInBytes, gLineIndexDataSizeInBytes, indicesLines);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
}

void initFonts(int windowWidth, int windowHeight)
{
    // Set OpenGL options
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), 0.0f, static_cast<GLfloat>(windowHeight));
    glUseProgram(gProgram[2]);
    glUniformMatrix4fv(glGetUniformLocation(gProgram[2], "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 0, &face))
    //if (FT_New_Face(ft, "/usr/share/fonts/truetype/gentium-basic/GenBkBasR.ttf", 0, &face)) // you can use different fonts
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
                );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (GLuint) face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    //
    // Configure VBO for texture quads
    //
    glGenBuffers(1, &gTextVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void reshape(GLFWwindow* window, int w, int h)
{
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    gWidth = w;
    gHeight = h;

    glViewport(0, 0, w, h);

    // Use perspective projection

    float fovyRad = (float) (45.0 / 180.0) * M_PI;
    projectionMatrix = glm::perspective(fovyRad, gWidth / (float) gHeight, 1.0f, 100.0f);

    // always look toward (0, 0, 0)
    viewingMatrix = glm::lookAt(eyePos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    for (int i = 0; i < 2; ++i)
    {
        glUseProgram(gProgram[i]);
        glUniformMatrix4fv(projectionMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(viewingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
    }

    // Update text positions
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(w - 45), 0.0f, static_cast<GLfloat>(h));
    glUseProgram(gProgram[2]);
    glUniformMatrix4fv(glGetUniformLocation(gProgram[2], "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void drawCube()
{
    glUseProgram(gProgram[0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void drawCubeEdges()
{
    glLineWidth(3);

    glUseProgram(gProgram[1]);

    for (int i = 0; i < 6; ++i)
    {
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(gTriangleIndexDataSizeInBytes + i * 4 * sizeof(GLuint)));
    }
}

void drawCubeWithEdges(const glm::mat4& cubeMatrix) {
    glm::mat4 translateToMid = glm::translate(cubeMatrix, TRANSLATION_TO_MIDDLE_VECTOR);
    glUseProgram(gProgram[0]);
    glUniformMatrix4fv(modelingMatrixLoc[0], 1, GL_FALSE, glm::value_ptr(translateToMid));
    drawCube();

    glUseProgram(gProgram[1]);
    glUniformMatrix4fv(modelingMatrixLoc[1], 1, GL_FALSE, glm::value_ptr(translateToMid));
    drawCubeEdges();
}
