#include <GL/glew.h>

// #include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// library for converting objects between GLM and SFML
// #include "GLM2SF.h"
// don't need it, use this constructor: sf::Glsl::Mat4(&glmmat4[0][0])

#include <iostream>
#include <unordered_map>

#include "oge/OGLObject.h"
#include "oge/Camera.h"
#include "ControlledObject.h"
#include "Follower.h"
#include "BackgroundObject.h"

/*
#include "common/shader.hpp"
*/

#include "oge/OGLSystem.h"
#include "MainScene.h"


int main() {

    oge::OGLSystem system;
    system.initialize();

    MainScene scene(system);
    scene.initialize();
    system.registerScene(scene, true);

    system.loop();

    return 0;
}

/*
void old_stuff()
{
    sf::Window window;
    // initializeSystem(window);

    // load resources

    // Dark blue background
    glClearColor(0.18f, 0.0f, 0.4f, 0.0f);


    // TODO: 2D text on screen


    // Enable Z-buffer read and write
    // without this, it would draw all of the triangles (in whatever order it finds them)
    // so this is to not draw things that are behind other things
    glEnable(GL_DEPTH_TEST);
    // glEnable(EnableCap.DepthTest);
    glDepthMask(GL_TRUE);
    glClearDepth(1);
    // one tutorial used this and didn't use the previous 2 lines
    // Accept fragment if it closer to the camera than the former one
    // glDepthFunc(GL_LESS);

    // only draw triangles that face the camera
    // (right-hand rule default - there's a setting that can change it to left-hand)
    glEnable(GL_CULL_FACE);


    // variables that will change at runtime (mouse input)
    bool leftMouseButtonIsDown = false;
    bool rightMouseButtonIsDown = false;
    sf::Vector2i previousMouseLocation;
    std::unordered_map<int, bool> keyIsDown;


    // Create and compile our GLSL program from the shaders
    // loader from tutorial
    // GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
    // loader from sfml
    sf::Shader colorProgram;
    std::cout << "created shader object\n";
    colorProgram.loadFromFile("ColorTransform.vertexshader", "Color.fragmentshader");
    std::cout << "loaded shaders from files\n";

    glm::mat4 myDummyFloatMatrix;  // need the address of a float in memory to reserve uniform

    // reserve a spot for MVP matrix
    // normal opengl way
    // GLint MatrixID = glGetUniformLocation(colorProgram.getNativeHandle(), "MVP");
    // sfml way
    // colorProgram.setUniform("MVP", glmToSfMat4(MVP));  // don't need this, use constructor with pointer parameter
    colorProgram.setUniform("MVP", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));



    // different program for textured objects
    sf::Shader textureProgram;
    textureProgram.loadFromFile("TextureTransform.vertexshader", "Texture.fragmentshader");
    textureProgram.setUniform("MVP", sf::Glsl::Mat4(&myDummyFloatMatrix[0][0]));

    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("resources/background.jpg");
    backgroundTexture.setSmooth(true);

    textureProgram.setUniform("myTextureSampler", backgroundTexture);




    // TODO: lighting

    oge::Camera camera;

    // Projection matrix : 45° Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
    camera.projection = glm::perspective(45.0f, (float)window.getSize().x / (float)window.getSize().y, 0.1f, 200.0f);
    // Or, for an ortho camera :
    //camera.projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

    // location and direction
    camera.focusPoint = glm::vec3(0, 0, 0);
    camera.distanceFromFocusPoint = 4.0f;
    camera.directionFromFocusPoint = glm::vec3(0, 0, 1);
    camera.upDirection = glm::vec3(0, 1, 0);


    ControlledObject ring;
    ring.loadFromFile("resources/bath spout cover new.stl");
    // 0 vertices
    // 1 color
    ring.uploadData();

    ring.setScale(glm::vec3(0.1f));

    // TODO: texture a model

    /* simple triangle
    // An array of 3 vectors which represents 3 vertices
    static const std::vector < GLfloat > g_vertex_buffer_data = {
            -0.8f, -0.8f, 0.0f,
            0.8f, -0.8f, 0.0f,
            0.0f,  0.8f, 0.0f,
    };
     */
/*
    Follower sphere;
    sphere.loadFromFile("resources/myascii205.stl");
    // 0 vertices
    // 1 color

    // calculate vertex normals (just away from center)
    glm::vec3 thisPoint;
    std::vector< float > normalArray;
    for (size_t i = 0; i < sphere.getBufferArray(0).size(); i += 3) {
        thisPoint[0] = sphere.getBufferArray(0)[i];
        thisPoint[1] = sphere.getBufferArray(0)[i+1];
        thisPoint[2] = sphere.getBufferArray(0)[i+2];
        thisPoint = glm::normalize(thisPoint);
        normalArray.push_back(thisPoint[0]);
        normalArray.push_back(thisPoint[1]);
        normalArray.push_back(thisPoint[2]);
        // This isn't what it looks like... honest...
        // (a vertex normal is not usually just a normalized vertex (only for a sphere))
    }
    sphere.addBufferArray(normalArray);
    // 2 vertex normal
    // TODO: use this for lighting

    sphere.uploadData();

    sphere.setScale(glm::vec3(0.1f));
    sphere.setLocation(glm::vec3(0.5f, 0.0f, 0.0f));


    BackgroundObject background;
    // 0 vertices
    background.addBufferArray(std::vector<float>({
        -95.0f, -62.0f, -18.0f,
        95.0f, -62.0f, -18.0f,
        -95.0f, 62.0f, -18.0f,
        95.0f, -62.0f, -18.0f,
        95.0f, 62.0f, -18.0f,
        -95.0f, 62.0f, -18.0f
    }));
    // vertexUV will be at layout location 1 / attribute 1
    background.addBufferArray(std::vector<float>({
         0.0f, 1.0f,
         1.0f, 1.0f,
         0.0f, 0.0f,
         1.0f, 1.0f,
         1.0f, 0.0f,
         0.0f, 0.0f,
    }));
    background.uploadData();



    sf::Clock clock;  // TODO: something changing with time

    // run the main loop
    bool running = true;
    while (running) {
        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                // end the program
                running = false;
            }
            else if (event.type == sf::Event::MouseWheelScrolled)
            {
                // change the zoom
                // camera.zoom(1 - event.mouseWheelScroll.delta / 10.0f);
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    leftMouseButtonIsDown = true;
                }
                else if (event.mouseButton.button == sf::Mouse::Right) {
                    rightMouseButtonIsDown = true;
                }
                previousMouseLocation = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    leftMouseButtonIsDown = false;
                }
                else if (event.mouseButton.button == sf::Mouse::Right) {
                    rightMouseButtonIsDown = false;
                }
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                if (leftMouseButtonIsDown)
                {
                    // rotate
                    int xDiff = event.mouseMove.x - previousMouseLocation.x;
                    int yDiff = event.mouseMove.y - previousMouseLocation.y;

                    ring.rotate(xDiff, yDiff);
                }
                if (rightMouseButtonIsDown) {
                    // translate
                    int xDiff = event.mouseMove.x - previousMouseLocation.x;
                    int yDiff = event.mouseMove.y - previousMouseLocation.y;

                    // ring.translate(xDiff * 0.002f * camera.distanceFromFocusPoint, yDiff * -0.002f * camera.distanceFromFocusPoint, 0);
                }
                previousMouseLocation = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
            }
            else if (event.type == sf::Event::KeyPressed) {
                keyIsDown[event.key.code] = true;
            }
            else if (event.type == sf::Event::KeyReleased)
                keyIsDown[event.key.code] = false;
        }

        glm::vec3 temp;  // = camera.getCameraLocation();
        // move camera with arrow keys
        if (keyIsDown[sf::Keyboard::Up]) {
            // camera.focusPoint[1] += 0.1f;
        }
        if (keyIsDown[sf::Keyboard::Down]) {
            // camera.focusPoint[1] -= 0.1f;
        }
        if (keyIsDown[sf::Keyboard::Left]) {
            // camera.focusPoint[0] -= 0.1f;
        }
        if (keyIsDown[sf::Keyboard::Right]) {
            // camera.focusPoint[0] += 0.1f;
        }
        // camera.setCameraLocation(temp);


        //draw

        // Clear the screen
        // glClear( GL_COLOR_BUFFER_BIT );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        sf::Shader::bind(&colorProgram);
        // glUseProgram(programID);

        // for ring
        // ring.draw(camera, colorProgram);

        // for sphere
        sphere.rotate(clock.getElapsedTime().asMilliseconds() % 3, 0);
        if (glm::distance(sphere.getLocation(), ring.getLocation()) > 0.05f) {
            sphere.translate(glm::normalize(ring.getLocation() - sphere.getLocation()) * 0.04f);
        }
        // sphere.draw(camera, colorProgram);


        // background
        sf::Shader::bind(&textureProgram);
        // background.draw(camera, textureProgram);


        window.display();
    }

    // Cleanup VBO, VAO, and shader

    /* OGLObject destructors delete these
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteVertexArrays(1, &ringVertexArrayID);

    glDeleteBuffers(1, &sphereVertexBufferID);
    glDeleteBuffers(1, &sphereColorBufferID);
    glDeleteVertexArrays(1, &sphereVertexArrayID);
     */
/*
    // glDeleteProgram(programID);
    // SFML Shader object destructor deletes the shader program
    // return 0;
}
*/