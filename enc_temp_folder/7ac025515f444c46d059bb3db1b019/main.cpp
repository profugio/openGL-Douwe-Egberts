#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "glsl.h"
#include "objloader.h"
#include "texture.h"
#include "walkCamera.h"
#include "droneCamera.h"
#include "mesh.h"

using namespace std;


//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 800, HEIGHT = 600;
const unsigned int objectCount = 51;

const char* fragshader_name = "fragmentshader.fsh";
const char* vertexshader_name = "vertexshader.vsh";

static const int DRONE_CAMERA = 1; 
static const int WALK_CAMERA = 0;

//Time
unsigned const int DELTA_TIME = 1;
unsigned int timeSinceStart = 0;
unsigned int oldTimeSinceStart;
unsigned int deltaTime;


//Cameras
Camera* cameras[2];
int activeCamera;

//Input buffer
bool keyBuffer[128];


vector<Mesh*> meshes;
vector<Mesh*> primitiveMeshes;

vector<Vertex> vertexList;

//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

// ID's
GLuint program_id;
//GLuint vao[objectCount];

//Moved to model class
//GLuint texture_id[objectCount];

// Uniform ID's
GLuint uniform_mv;
GLuint uniform_proj;
GLuint uniform_light_pos;
GLuint uniform_material_ambient;
GLuint uniform_material_diffuse;
GLuint uniform_specular;
GLuint uniform_material_power;
GLuint uniform_hasTexture;
GLuint uniform_testColor;
GLuint uniform_isPrimitive;
GLuint uniform_lightsCounts;


//Primitives
const unsigned int couchMeshCount = 10;
vector<glm::vec3> couch_vertices[couchMeshCount] = {

    //Couch
    //left armrest
    {
        // front
        glm::vec3(-1.5, -1.0, 8.5),
        glm::vec3(1.5, -1.0, 8.5),
        glm::vec3(1.5, 1.0, 8.5),
        glm::vec3(-1.5, 1.0, 8.5),
        // back
        glm::vec3(-1.5, -1.0, 8.0),
        glm::vec3(1.5, -1.0, 8.0),
        glm::vec3(1.5, 1.0, 8.0),
        glm::vec3(-1.5, 1.0, 8.0)
    },
    //right armrest
    {
        // front 
        glm::vec3(-1.5, -1.0 , -8.0),
        glm::vec3(1.5, -1.0, -8.0),
        glm::vec3(1.5, 1.0, -8.0),
        glm::vec3(-1.5, 1.0, -8.0),
        //back
        glm::vec3(-1.5, -1.0 , -8.5),
        glm::vec3(1.5, -1.0, -8.5),
        glm::vec3(1.5, 1.0, -8.5),
        glm::vec3(-1.5, 1.0, -8.5)

    },
    //Backsupport
    {
        //front
        glm::vec3(-1.5, -1.0, 8.5),
        glm::vec3(-1.5, -1.0, -8.5),
        glm::vec3(-1.5, 1.0, -8.5),
        glm::vec3(-1.5, 1.0, 8.5),
        //back
        glm::vec3(-1.0, -1.0, 8.5),
        glm::vec3(-1.0, -1.0, -8.5),
        glm::vec3(-1.0, 1.0, -8.5),
        glm::vec3(-1.0, 1.0, 8.5),
    },
    //sit cushion
    {
        //front
        glm::vec3(-1.5, -1.0, 8.5),
        glm::vec3(-1.5, -1.0, -8.5),
        glm::vec3(1.5, -1.0, -8.5),
        glm::vec3(1.5, -1.0, 8.5),
        //back
        glm::vec3(-1.5, -0.5, 8.5),
        glm::vec3(-1.5, -0.5, -8.5),
        glm::vec3(1.5, -0.5, -8.5),
        glm::vec3(1.5, -0.5, 8.5),
    },
    //front right leg
    {
        //front
        glm::vec3(1.1, -1.0, -7.9),
        glm::vec3(1.1, -1.0, -8.1),
        glm::vec3(1.3, -1.0, -8.1),
        glm::vec3(1.3, -1.0, -7.9),
        //back
        glm::vec3(1.1, -1.8, -7.9),
        glm::vec3(1.1, -1.8, -8.1),
        glm::vec3(1.3, -1.8, -8.1),
        glm::vec3(1.3, -1.8, -7.9)
    },
    //front left leg
    {
                //front
        glm::vec3(1.1, -1.0, 8.1),
        glm::vec3(1.1, -1.0, 7.9),
        glm::vec3(1.3, -1.0, 7.9),
        glm::vec3(1.3, -1.0, 8.1),
        //back
        glm::vec3(1.1, -1.8, 8.1),
        glm::vec3(1.1, -1.8, 7.9),
        glm::vec3(1.3, -1.8, 7.9),
        glm::vec3(1.3, -1.8, 8.1)
    },
    //back right leg
    {
        //front
        glm::vec3(-1.1, -1.0, -7.9),
        glm::vec3(-1.1, -1.0, -8.1),
        glm::vec3(-0.9, -1.0, -8.1),
        glm::vec3(-0.9, -1.0, -7.9),
        //back
        glm::vec3(-1.1, -1.8, -7.9),
        glm::vec3(-1.1, -1.8, -8.1),
        glm::vec3(-0.9, -1.8, -8.1),
        glm::vec3(-0.9, -1.8, -7.9)
    },
    //back left leg
    {
        //front
        glm::vec3(-1.1, -1.0, 8.1),
        glm::vec3(-1.1, -1.0, 7.9),
        glm::vec3(-0.9, -1.0, 7.9),
        glm::vec3(-0.9, -1.0, 8.1),
        //back
        glm::vec3(-1.1, -1.8, 8.1),
        glm::vec3(-1.1, -1.8, 7.9),
        glm::vec3(-0.9, -1.8, 7.9),
        glm::vec3(-0.9, -1.8, 8.1)
    },
    //back middle leg
    {
        //front
        glm::vec3(-1.1, -1.0, 0.1),
        glm::vec3(-1.1, -1.0, -0.1),
        glm::vec3(-0.9, -1.0, -0.1),
        glm::vec3(-0.9, -1.0, 0.1),
        //back
        glm::vec3(-1.1, -1.8, 0.1),
        glm::vec3(-1.1, -1.8, -0.1),
        glm::vec3(-0.9, -1.8, -0.1),
        glm::vec3(-0.9, -1.8, 0.1)
    },
    //front middle leg
    {
        //front
        glm::vec3(0.6, -1.0, 0.1),
        glm::vec3(0.6, -1.0, -0.1),
        glm::vec3(0.8, -1.0, -0.1),
        glm::vec3(0.8, -1.0, 0.1),
        //back
        glm::vec3(0.6, -1.8, 0.1),
        glm::vec3(0.6, -1.8, -0.1),
        glm::vec3(0.8, -1.8, -0.1),
        glm::vec3(0.8, -1.8, 0.1)
    }
    //---End couch---

};

const unsigned int windowMeshCount = 11;
vector<glm::vec3> windowMeshes[windowMeshCount] = {
    //Bottom beam
     {
       //front
       glm::vec3(-3.0, -0.6, 0.2),
       glm::vec3(3.0, -0.6, 0.2),
       glm::vec3(3.0, -0.9, 0.2),
       glm::vec3(-3.0, -0.9, 0.2),
       //back
       glm::vec3(-3.0, -0.6, -0.2),
       glm::vec3(3.0, -0.6, -0.2),
       glm::vec3(3.0, -0.9, -0.2),
       glm::vec3(-3.0, -0.9, -0.2),
     },
     //Right beam
    {
        //front
        glm::vec3(2.8, -0.6, 0.2),
        glm::vec3(3.0, -0.6, 0.2),
        glm::vec3(3.0, 8.0, 0.2),
        glm::vec3(2.8, 8.0, 0.2),
        //back
        glm::vec3(2.8, -0.6, -0.2),
        glm::vec3(3.0, -0.6, -0.2),
        glm::vec3(3.0, 7.4, -0.2),
        glm::vec3(2.8, 7.4, -0.2)
    },
    //Left beam
    {
        //front
        glm::vec3(-3.0, -0.6, 0.2),
        glm::vec3(-2.8, -0.6, 0.2),
        glm::vec3(-2.8, 7.4, 0.2),
        glm::vec3(-3.0, 7.4, 0.2),
        //back
        glm::vec3(-3.0, -0.6, -0.2),
        glm::vec3(-2.8, -0.6, -0.2),
        glm::vec3(-2.8, 7.4, -0.2),
        glm::vec3(-3.0, 7.4, -0.2)
    },
    //Top beam
    {
        //front
        glm::vec3(-3.0, 7.4, 0.2),
        glm::vec3(3.0, 7.4, 0.2),
        glm::vec3(3.0, 7.6, 0.2),
        glm::vec3(-3.0, 7.6, 0.2),
        //back
        glm::vec3(-3.0, 7.4, -0.2),
        glm::vec3(3.0, 7.4, -0.2),
        glm::vec3(3.0, 7.6, -0.2),
        glm::vec3(-3.0, 7.6, -0.2),
    },
    //Middle beam horizontal
    {
        //front
        glm::vec3(-3.0, 3.3, 0.2),
        glm::vec3(3.0, 3.3, 0.2),
        glm::vec3(3.0, 3.5, 0.2),
        glm::vec3(-3.0, 3.5, 0.2),
        //back
        glm::vec3(-3.0, 3.3, -0.2),
        glm::vec3(3.0, 3.3, -0.2),
        glm::vec3(3.0, 3.5, -0.2),
        glm::vec3(-3.0, 3.5, -0.2),
    },
    //Middle beam vertical
    {
        //front
        glm::vec3(-0.1, -0.6, 0.2),
        glm::vec3(0.1, -0.6, 0.2),
        glm::vec3(0.1, 12.0, 0.2),
        glm::vec3(-0.1, 12.0, 0.2),
        //back
        glm::vec3(-0.1, -0.6, -0.2),
        glm::vec3(0.1, -0.6, -0.2),
        glm::vec3(0.1, 12.0, -0.2),
        glm::vec3(-0.1, 12.0, -0.2),
    },
    //Left frame
    {
        //front
        glm::vec3(-3.6, -0.4, 0.4),
        glm::vec3(-3.0, -0.4, 0.4),
        glm::vec3(-3.0, 12.0, 0.4),
        glm::vec3(-3.6, 12.0, 0.4),
        //back
        glm::vec3(-3.6, -0.4, -0.4),
        glm::vec3(-3.0, -0.4, -0.4),
        glm::vec3(-3.0, 12.0, -0.4),
        glm::vec3(-3.6, 12.0, -0.4)
    },
    //Right frame
    {
        //front
        glm::vec3(3.0, -0.4, 0.4),
        glm::vec3(3.6, -0.4, 0.4),
        glm::vec3(3.6, 12.0, 0.4),
        glm::vec3(3.0, 12.0, 0.4),
        //back
        glm::vec3(3.0, -0.4, -0.4),
        glm::vec3(3.6, -0.4, -0.4),
        glm::vec3(3.6, 12.0, -0.4),
        glm::vec3(3.0, 12.0, -0.4)
    },
    //Bottom Frame
    {
       //front
       glm::vec3(-3.6, -2.6, 0.4),
       glm::vec3(3.6, -2.6, 0.4),
       glm::vec3(3.6, -0.4, 0.4),
       glm::vec3(-3.6, -0.4, 0.4),
       //back
       glm::vec3(-3.6, -2.6, -0.4),
       glm::vec3(3.6, -2.6, -0.4),
       glm::vec3(3.6, -0.4, -0.4),
       glm::vec3(-3.6, -0.4, -0.4),
     },
     //Middle horizontal frame
     {
        //front
        glm::vec3(-3.0, 7.6, 0.4),
        glm::vec3(3.0, 7.6, 0.4),
        glm::vec3(3.0, 8.2, 0.4),
        glm::vec3(-3.0, 8.2, 0.4),
        //back
        glm::vec3(-3.0, 7.6, -0.4),
        glm::vec3(3.0, 7.6, -0.4),
        glm::vec3(3.0, 8.2, -0.4),
        glm::vec3(-3.0, 8.2, -0.4),
     },
     //Top frame
     {
         //front
         glm::vec3(-3.6, 12.0, 0.8),
         glm::vec3(3.6, 12.0, 0.8),
         glm::vec3(3.6, 12.6, 0.8),
         glm::vec3(-3.6, 12.6, 0.8),
         //back
         glm::vec3(-3.6, 12.0, -0.4),
         glm::vec3(3.6, 12.0, -0.4),
         glm::vec3(3.6, 12.6, -0.4),
         glm::vec3(-3.6, 12.6, -0.4),
     },
};



GLfloat primitive_colors[] = {
    // front colors
    1.0, 1.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    1.0, 1.0, 1.0,
    // back colors
    0.0, 1.0, 1.0,
    1.0, 0.0, 1.0,
    1.0, 0.0, 0.0,
    1.0, 1.0, 0.0
};

GLushort solid_cube_elements[] = {
    0,1,2,0,2,3,// front
    1,5,6,1,6,2,//right side
    0,4,7,0,3,7,//left side
    4,5,6,4,6,7,//back
    0,1,5,0,5,4, // bottom
    3,2,6,3,6,7, //top
    0,3,1,2,5,6,4,7  // top to bottom
};



struct LightSource {
    glm::vec3 position;
};



Material materials[objectCount];
//LightSource light;
const unsigned int lightCount = 4;
glm::vec3 lights[lightCount];




void InitCameras() {
    cameras[0] = new WalkCamera();
    cameras[1] = new DroneCamera();
    activeCamera = WALK_CAMERA;
}


//--------------------------------------------------------------------------------
// Keyboard handling
//--------------------------------------------------------------------------------

void clearKeyBuffer() {
    for (int i = 0; i < sizeof(keyBuffer) / sizeof(bool); i++) {
        keyBuffer[i] = false;
    }
}

void keyboardHandler(unsigned char key, int a, int b){
    //Add pressed keys to keybuffer
    if(keyBuffer[key] == false) {
        keyBuffer[key] = true;
    }
}

void keyboardUpHandler(unsigned char key, int a, int b) {
    //Switch between camera mode
    if (key == 'v') {
        if (activeCamera == WALK_CAMERA) {
           
            delete cameras[DRONE_CAMERA];
            cameras[DRONE_CAMERA] = new DroneCamera();
            activeCamera = DRONE_CAMERA;
            clearKeyBuffer();
        }
        else {
            activeCamera = WALK_CAMERA;
        }
    }
    //Remove released keys from keybuffer
    else if (keyBuffer[key] == true) {
        keyBuffer[key] = false;
    }
   
}





//If keyboard input is for the camera send to camera
void doKeyboardInput(int key, int deltaTime) {
    vector<char> cameraInputs{ 'w', 's', 'a', 'd', 'q', 'e', 'i', 'j', 'k', 'l'};
    if (std::find(cameraInputs.begin(), cameraInputs.end(), key)!= cameraInputs.end()) {
        (cameras[activeCamera])->Move(key, deltaTime);
    }
    else if (key == 27) {
        glutExit();
    }
}


//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------

void InitLight() {
 //For loop for multiple lightsources
    lights[0] = glm::vec3(4.0, 10.0, 4.0);
    lights[1] = glm::vec3(-35.0, 10.0, -10.0);
    lights[2] = glm::vec3(3.0, 2.0, -2.0);
    lights[3] = glm::vec3(-35.0, 3.0, -20.0);
    for (int i = 0; i < objectCount; i++) {
       
        materials[i].ambient_color = glm::vec3(0.2, 0.2, 0.1);
        materials[i].diffuse_color = glm::vec3(0.5, 0.5, 0.3);
        materials[i].specular = glm::vec3(0.7, 0.7, 0.7);
        materials[i].power = 1024;
    }
  
}

void Render()
{
    oldTimeSinceStart = timeSinceStart;
    timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = timeSinceStart - oldTimeSinceStart;
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Do keyboard input
    for (int i = 0; i < (sizeof(keyBuffer) / sizeof(bool)); i++) {
        if (keyBuffer[i]) {
            doKeyboardInput(i, deltaTime);
        }
    }
    //Change light to camera position
    glUniform3fv(uniform_light_pos, 1, glm::value_ptr(cameras[activeCamera]->cameraPos));


    // Attach to program_id
    glUseProgram(program_id);
    glm::vec3 color;

    for (int i = 0; i < meshes.size(); i++) {
        meshes[i]->mv = cameras[activeCamera]->view * meshes[i]->model;

        // Send mv
        glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(meshes[i]->mv));

        //Bind texture
        //if (meshes[i]->texture_id != 0) {
            glBindTexture(GL_TEXTURE_2D, meshes[i]->texture_id);
            glUniform1i(uniform_hasTexture, true);
        //}
        //else {
        //    glUniform1i(uniform_hasTexture, false);
        //    color = glm::vec3(1.0, 0.0, 0.0);
        //    glUniform3fv(uniform_testColor, 1, glm::value_ptr(color));
        //}
        
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

        //Fill uniform vars
        glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(cameras[activeCamera]->projection));
        glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(meshes[i]->material.ambient_color));
        glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(meshes[i]->material.diffuse_color));
        glUniform3fv(uniform_specular, 1, glm::value_ptr(meshes[i]->material.specular));
        glUniform1f(uniform_material_power, meshes[i]->material.power);

        
        // Send vao
        glBindVertexArray(meshes[i]->vao);

        //If mesh is primitive draw using elements, else simply draw triangles
        if (meshes[i]->isPrimitive) {
            glDrawElements(GL_TRIANGLES, sizeof(solid_cube_elements) / sizeof(GLushort),
                GL_UNSIGNED_SHORT, 0);
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, meshes[i]->vertices.size());
        }
        
        glBindVertexArray(0);
   }



    glutSwapBuffers();
}


//------------------------------------------------------------
// void Render(int n)
// Render method that is called by the timer function
//------------------------------------------------------------

void Render(int n)
{
    Render();
    glutTimerFunc(DELTA_TIME, Render, 0);
}


//------------------------------------------------------------
// void InitGlutGlew(int argc, char **argv)
// Initializes Glut and Glew
//------------------------------------------------------------

void InitGlutGlew(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Douwe Egberts");
    glutDisplayFunc(Render);
    glutKeyboardFunc(keyboardHandler);
    glutTimerFunc(DELTA_TIME, Render, 0);

    glewInit();
}



void InitLoadObjects() {
    //Change these to some sort of dictionairy
    const char* objects[objectCount] = { "Objects/teapot.obj", "Objects/cylinder18.obj", "Objects/smallTable.obj", "Objects/box.obj", "Objects/cushion5.obj", "Objects/roundTable.obj", "Objects/roundTable.obj" , "Objects/roundTable.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj" , "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/LampRailRight.obj", "Objects/LampRailLeft.obj", "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/cushion5.obj", "Objects/cushion5.obj" , "Objects/cushion5.obj" , "Objects/cushion5.obj" , "Objects/cushion5.obj" , "Objects/cushion5.obj", "Objects/bookCase.obj", "Objects/roundTable.obj", "Objects/roundTable.obj" , "Objects/roundTable.obj", "Objects/smallTable.obj", "Objects/armChair.obj", "Objects/armChair.obj", "Objects/chair2.obj", "Objects/chair2.obj" , "Objects/chair2.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj" };
    const char* textures[objectCount] = { "Textures/uvtemplate.bmp", "Textures/Yellobrk.bmp","Textures/woodTexture.bmp", "Textures/woodFloor.bmp", "Textures/greyCushion.bmp", "Textures/roundTableMarble.bmp", "Textures/roundTableMarble.bmp", "Textures/roundTableMarble.bmp", "Textures/redCloth.bmp", "Textures/redCloth.bmp", "Textures/redCloth.bmp", "Textures/greyCushion.bmp", "Textures/redCloth.bmp", "Textures/greyCushion.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/ceiling.bmp", "Textures/lampRailTexture.bmp", "Textures/lampRailTexture.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/greyCushion.bmp" , "Textures/greyCushion.bmp" , "Textures/redCloth.bmp" , "Textures/redCloth.bmp" , "Textures/greyCushion.bmp" , "Textures/redCloth.bmp", "Textures/bookCaseTexture.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp" , "Textures/black_metal.bmp", "Textures/woodTexture.bmp", "Textures/armChair.bmp", "Textures/armChair.bmp" , "Textures/black_metal.bmp" , "Textures/black_metal.bmp" , "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp" };
   
    vector<glm::vec3> vertices{};
    vector<glm::vec3> normals{};
    vector<glm::vec2> uvs{};
    glm::vec3 color;
    GLuint id;
    //Add all imported objects to mesh list
    for (int i = 0; i < objectCount; i++) {
        if (i == 0 || (objects[i] != objects[i - 1])) {
            vertices.clear();
            normals.clear();
            uvs.clear();
            bool res = loadOBJ(objects[i], vertices, uvs, normals);
        }

        for (int j = 0; j < vertices.size(); j++) {
            vertexList.push_back({vertices[j], normals[j], uvs[j]});
        }
        Mesh* m = new Mesh();
        m->CreateMesh(vertices, normals, uvs);
        if (i == 0 || (textures[i] != textures[i - 1])) {
            id = loadBMP(textures[i]);
           
        }
        else{
          
            id = meshes[i-1]->texture_id;
        }
        m->texture_id = id;
        meshes.push_back(m);
       
        vertexList.clear();


       


    }
    vertices.clear();
    normals.clear();
    uvs.clear();

    //Add all primtive meshes to meshList
    for (int i = 0; i < couchMeshCount; i++) {
        Mesh* m = new Mesh();
        m->isPrimitive = true;
        for (int j = 0; j < 8; j++) {
            vertices.push_back(couch_vertices[i][j]);
            uvs.push_back(glm::vec2(couch_vertices[i][j].x / couch_vertices[i][j].z, couch_vertices[i][j].y / couch_vertices[i][j].z));
            
        }
        for (int k = 0; k < 22; k += 3) {
            normals.push_back(glm::normalize(glm::cross(vertices[solid_cube_elements[k + 1]] - vertices[solid_cube_elements[k]], vertices[solid_cube_elements[k + 2]] - vertices[solid_cube_elements[k]])));
        }
        m->CreateMesh(vertices, normals, uvs);
        
        if (i >= couchMeshCount - 6) {
            m->texture_id = loadBMP(textures[33]);
        }
        else {
            m->texture_id = loadBMP(textures[4]);
        }
        
        meshes.push_back(m);
        vertices.clear();
        normals.clear();
        uvs.clear();
    }

    //Test for new primitive window
    for (int l = 0; l < 3; l++) {
        for (int i = 0; i < windowMeshCount; i++) {
            Mesh* m = new Mesh();
            m->isPrimitive = true;
            for (int j = 0; j < 8; j++) {
                vertices.push_back(windowMeshes[i][j]);
                uvs.push_back(glm::vec2(windowMeshes[i][j].x / windowMeshes[i][j].z, windowMeshes[i][j].y / windowMeshes[i][j].z));

            }
            for (int k = 0; k < 22; k += 3) {
                normals.push_back(glm::normalize(glm::cross(vertices[solid_cube_elements[k + 1]] - vertices[solid_cube_elements[k]], vertices[solid_cube_elements[k + 2]] - vertices[solid_cube_elements[k]])));
            }
            m->CreateMesh(vertices, normals, uvs);
            m->texture_id = loadBMP(textures[21]);
            meshes.push_back(m);
            vertices.clear();
            normals.clear();
            uvs.clear();
        }
    }
   
   
}


//------------------------------------------------------------
// void InitShaders()
// Initializes the fragmentshader and vertexshader
//------------------------------------------------------------

void InitShaders()
{
    char* vertexshader = glsl::readFile(vertexshader_name);
    GLuint vsh_id = glsl::makeVertexShader(vertexshader);

    char* fragshader = glsl::readFile(fragshader_name);
    GLuint fsh_id = glsl::makeFragmentShader(fragshader);

    program_id = glsl::makeShaderProgram(vsh_id, fsh_id);
}


//------------------------------------------------------------
// void InitMatrices()
//------------------------------------------------------------

void InitMatrices()
{
    //Position all couch meshes correctly
    for (int i = objectCount; i < objectCount + couchMeshCount; i++) {
        if (meshes[i]->isPrimitive) {
            meshes[i]->model = glm::translate(meshes[i]->model, glm::vec3(-35.0, 3.0, -20.0));
            meshes[i]->model = glm::rotate(meshes[i]->model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
            meshes[i]->material.ambient_color = glm::vec3(0.25, 0.25, 0.1);
            meshes[i]->material.diffuse_color += glm::vec3(0.1, 0.1, 0.1);
            meshes[i]->material.specular = glm::vec3(0.0, 0.0, 0.0);
        }
       
     

    }

    for (int i = objectCount + couchMeshCount; i < (objectCount + couchMeshCount+ windowMeshCount); i++) {
        meshes[i]->model = glm::translate(meshes[i]->model, glm::vec3(25.2, 7.4, -17.0));
        meshes[i]->model = glm::rotate(meshes[i]->model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
        
    }
    for (int i = objectCount + couchMeshCount + windowMeshCount; i < (objectCount + couchMeshCount + 2*windowMeshCount); i++) {
        meshes[i]->model = glm::translate(meshes[i]->model, glm::vec3(25.2, 7.4, -8.3));
        meshes[i]->model = glm::rotate(meshes[i]->model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
        
    }
    for (int i = objectCount + couchMeshCount + 2*windowMeshCount; i < (objectCount + couchMeshCount + 3 * windowMeshCount); i++) {
        meshes[i]->model = glm::translate(meshes[i]->model, glm::vec3(25.2, 7.4, 0.3));
        meshes[i]->model = glm::rotate(meshes[i]->model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
        
    }

    meshes[0]->model = glm::translate(glm::mat4(), glm::vec3(3.0, 1.0, 0.0));
    meshes[1]->model = glm::mat4();

    //Small table corner
    meshes[2]->model = glm::translate(glm::mat4(), glm::vec3(1.5, 1.85, -12.5));
    meshes[2]->model = glm::rotate(meshes[2]->model, glm::radians(40.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[2]->model = glm::scale(meshes[2]->model, glm::vec3(1.8, 1.8, 1.8));
    meshes[36]->model = glm::translate(meshes[36]->model, glm::vec3(3.0, 1.7, -15.0));
    meshes[36]->model = glm::scale(meshes[36]->model, glm::vec3(1.3, 1.3, 1.3));

    //Armchairs
    meshes[37]->model = glm::translate(meshes[37]->model, glm::vec3(5.5, 1.25, -11.5));
    meshes[37]->model = glm::scale(meshes[37]->model, glm::vec3(0.08, 0.08, 0.08));
    meshes[37]->model = glm::rotate(meshes[37]->model, glm::radians(170.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[37]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[38]->model = glm::translate(meshes[38]->model, glm::vec3(-2.5, 1.25, -11.5));
    meshes[38]->model = glm::scale(meshes[38]->model, glm::vec3(0.08, 0.08, 0.08));
    meshes[38]->model = glm::rotate(meshes[38]->model, glm::radians(20.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[38]->material.specular = glm::vec3(0.0, 0.0, 0.0);
    //Floor
    meshes[3]->model = glm::translate(glm::mat4(), glm::vec3(-15.0, 1.0, -10.0));
    meshes[3]->model = glm::scale(meshes[3]->model, glm::vec3(80.0, 0.2, 30.0));
    //Couch cushions
    meshes[4]->model = glm::translate(meshes[4]->model, glm::vec3(-42.0, 3.3, -20.5));
    meshes[4]->model = glm::rotate(meshes[4]->model, glm::radians(-50.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[4]->model = glm::rotate(meshes[4]->model, glm::radians(20.0f), glm::vec3(0.0, 0.0, 1.0));
    meshes[4]->model = glm::scale(meshes[4]->model, glm::vec3(3.0,3.0,3.0));

    meshes[8]->model = glm::translate(meshes[8]->model, glm::vec3(-40.7, 3.3, -20.5));
    meshes[8]->model = glm::rotate(meshes[8]->model, glm::radians(-70.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[8]->model = glm::rotate(meshes[8]->model, glm::radians(20.0f), glm::vec3(0.0, 0.0, 1.0));
    meshes[8]->model = glm::scale(meshes[8]->model, glm::vec3(3.0, 3.0, 3.0));

    meshes[9]->model = glm::translate(meshes[9]->model, glm::vec3(-39.5, 3.0, -20.3));
    meshes[9]->model = glm::rotate(meshes[9]->model, glm::radians(-70.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[9]->model = glm::rotate(meshes[9]->model, glm::radians(70.0f), glm::vec3(0.0, 0.0, 1.0));
    meshes[9]->model = glm::rotate(meshes[9]->model, glm::radians(-35.0f), glm::vec3(1.0, 0.0, 0.0));
    meshes[9]->model = glm::scale(meshes[9]->model, glm::vec3(3.0, 3.0, 3.0));

    meshes[10]->model = glm::translate(meshes[10]->model, glm::vec3(-35.5, 3.3, -20.5));
    meshes[10]->model = glm::rotate(meshes[10]->model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[10]->model = glm::rotate(meshes[10]->model, glm::radians(-3.0f), glm::vec3(0.0, 0.0, 1.0));
    meshes[10]->model = glm::scale(meshes[10]->model, glm::vec3(3.0, 3.0, 3.0));

    meshes[11]->model = glm::translate(meshes[11]->model, glm::vec3(-34.0, 3.3, -20.5));
    meshes[11]->model = glm::rotate(meshes[11]->model, glm::radians(-100.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[11]->model = glm::rotate(meshes[11]->model, glm::radians(20.0f), glm::vec3(0.0, 0.0, 1.0));
    meshes[11]->model = glm::scale(meshes[11]->model, glm::vec3(3.0, 3.0, 3.0));

    meshes[12]->model = glm::translate(meshes[12]->model, glm::vec3(-29.3, 3.3, -20.5));
    meshes[12]->model = glm::rotate(meshes[12]->model, glm::radians(-135.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[12]->model = glm::rotate(meshes[12]->model, glm::radians(20.0f), glm::vec3(0.0, 0.0, 1.0));
    meshes[12]->model = glm::scale(meshes[12]->model, glm::vec3(3.0, 3.0, 3.0));

    meshes[13]->model = glm::translate(meshes[13]->model, glm::vec3(-27.9, 3.3, -20.2));
    meshes[13]->model = glm::rotate(meshes[13]->model, glm::radians(-155.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[13]->model = glm::rotate(meshes[13]->model, glm::radians(20.0f), glm::vec3(0.0, 0.0, 1.0));
    meshes[13]->model = glm::scale(meshes[13]->model, glm::vec3(3.0, 3.0, 3.0));
    //Round marble tables
    meshes[5]->model = glm::translate(meshes[5]->model, glm::vec3(-41.0, 1.15, -15.0));
    meshes[6]->model = glm::translate(meshes[6]->model, glm::vec3(-35.0, 1.15, -15.0));
    meshes[7]->model = glm::translate(meshes[7]->model, glm::vec3(-29.0, 1.15, -15.0));
    //Walls
    meshes[14]->model = glm::translate(meshes[14]->model, glm::vec3(-15.0, 0.0, -24.8));
    meshes[14]->model = glm::scale(meshes[14]->model, glm::vec3(80.0, 20.0, 0.2));
    meshes[14]->material.diffuse_color = glm::vec3(0.2, 0.2, 0.2);
    meshes[14]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[15]->model = glm::translate(meshes[15]->model, glm::vec3(-8.0, 0.0, -23.15));
    meshes[15]->model = glm::scale(meshes[15]->model, glm::vec3(66, 20.0, 3.3));
    meshes[15]->material.diffuse_color = glm::vec3(0.2, 0.2, 0.2);
    meshes[15]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[16]->model = glm::translate(meshes[16]->model, glm::vec3(-15.0, 0.0, 5.0));
    meshes[16]->model = glm::scale(meshes[16]->model, glm::vec3(80.0, 20.0, 0.2));
    meshes[16]->material.diffuse_color = glm::vec3(0.2, 0.2, 0.2);
    meshes[16]->material.specular = glm::vec3(0.0, 0.0, 0.0);
    //Ceiling
    meshes[17]->model = glm::translate(meshes[17]->model, glm::vec3(-15.0, 20.0, -5.0));
    meshes[17]->model = glm::rotate(meshes[17]->model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[17]->model = glm::scale(meshes[17]->model, glm::vec3(40.0, 0.2, 80.0));
    meshes[17]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    //Lamp Rails right
    meshes[18]->model = glm::translate(meshes[18]->model, glm::vec3(-20.0, 18.0, -2.0));
    meshes[18]->model = glm::rotate(meshes[18]->model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[18]->model = glm::scale(meshes[18]->model, glm::vec3(0.8, 0.8, 0.8));

    //Lamp rails left
    meshes[19]->model = glm::translate(meshes[19]->model, glm::vec3(-23, 18.0, -16.5));
    meshes[19]->model = glm::scale(meshes[19]->model, glm::vec3(0.8, 0.8, 0.8));
    meshes[19]->model = glm::rotate(meshes[19]->model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));


    //Window seating
    meshes[20]->model = glm::translate(meshes[20]->model, glm::vec3(24.0, 1.0, -8.5));
    meshes[20]->model = glm::scale(meshes[20]->model, glm::vec3(3.0, 3.7, 27.0));

    meshes[21]->model = glm::translate(meshes[21]->model, glm::vec3(23.0, 4.5, -8.5));
    meshes[21]->model = glm::scale(meshes[21]->model, glm::vec3(5.0, 0.3, 27.0));

    //Walls between windows
    meshes[22]->model = glm::translate(meshes[22]->model, glm::vec3(23.5, 4.5, -12.6));
    meshes[22]->model = glm::scale(meshes[22]->model, glm::vec3(3.7, 15.5, 1.5));

    meshes[23]->model = glm::translate(meshes[23]->model, glm::vec3(23.5, 4.5, -4.0));
    meshes[23]->model = glm::scale(meshes[23]->model, glm::vec3(3.7, 15.5, 1.5));

    meshes[24]->model = glm::translate(meshes[24]->model, glm::vec3(23.5, 4.5, -21.0));
    meshes[24]->model = glm::scale(meshes[24]->model, glm::vec3(3.7, 15.5, 1.0));

    meshes[25]->model = glm::translate(meshes[25]->model, glm::vec3(23.5, 4.5, 4.4));
    meshes[25]->model = glm::scale(meshes[25]->model, glm::vec3(3.7, 15.5, 1.0));

    //Window cushions
    meshes[26]->model = glm::translate(meshes[26]->model, glm::vec3(23.9, 5.8, -19.8));
    meshes[26]->model = glm::rotate(meshes[26]->model, glm::radians(50.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[26]->model = glm::scale(meshes[26]->model, glm::vec3(3.5, 3.5, 3.5));

    meshes[27]->model = glm::translate(meshes[27]->model, glm::vec3(23.9, 5.8, -14.1));
    meshes[27]->model = glm::rotate(meshes[27]->model, glm::radians(-50.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[27]->model = glm::scale(meshes[27]->model, glm::vec3(3.5, 3.5, 3.5));

    meshes[28]->model = glm::translate(meshes[28]->model, glm::vec3(23.9, 5.8, -11.0));
    meshes[28]->model = glm::rotate(meshes[28]->model, glm::radians(50.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[28]->model = glm::scale(meshes[28]->model, glm::vec3(3.5, 3.5, 3.5));

    meshes[29]->model = glm::translate(meshes[29]->model, glm::vec3(23.9, 5.8, -5.2));
    meshes[29]->model = glm::rotate(meshes[29]->model, glm::radians(-50.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[29]->model = glm::scale(meshes[29]->model, glm::vec3(3.5, 3.5, 3.5));

    meshes[30]->model = glm::translate(meshes[30]->model, glm::vec3(23.9, 5.8, -2.4));
    meshes[30]->model = glm::rotate(meshes[30]->model, glm::radians(50.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[30]->model = glm::scale(meshes[30]->model, glm::vec3(3.5, 3.5, 3.5));

    meshes[31]->model = glm::translate(meshes[31]->model, glm::vec3(23.9, 5.8, 3.3));
    meshes[31]->model = glm::rotate(meshes[31]->model, glm::radians(-50.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[31]->model = glm::scale(meshes[31]->model, glm::vec3(3.5, 3.5, 3.5));

    //BookCase
    meshes[32]->model = glm::translate(meshes[32]->model, glm::vec3(-20.0, 10.0, -16.0));
    meshes[32]->model = glm::scale(meshes[32]->model, glm::vec3(1.0, 1.4, 1.0));

    //Black round tables
    meshes[33]->model = glm::translate(meshes[33]->model, glm::vec3(-40.0, 1.15, 2.5));
    meshes[33]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[34]->model = glm::translate(meshes[34]->model, glm::vec3(-25.0, 1.15, 2.5));
    meshes[34]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[35]->model = glm::translate(meshes[35]->model, glm::vec3(-2.0, 1.15, 2.5));
    meshes[35]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    //Chairs at marble tables
    meshes[39]->model = glm::translate(meshes[39]->model, glm::vec3(-41.0, 3.5, -11.0));
    meshes[39]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[40]->model = glm::translate(meshes[40]->model, glm::vec3(-35.0, 3.5, -11.0));
    meshes[40]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[41]->model = glm::translate(meshes[41]->model, glm::vec3(-29.0, 3.5, -11.0));
    meshes[41]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    //Chairs at black tables
    //table 1
    meshes[42]->model = glm::translate(meshes[42]->model, glm::vec3(-40.0, 1.0, -2.0));
    meshes[42]->model = glm::scale(meshes[42]->model, glm::vec3(0.04, 0.04, 0.04));
    meshes[42]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[43]->model = glm::translate(meshes[43]->model, glm::vec3(-44.5, 1.0, 2.5));
    meshes[43]->model = glm::scale(meshes[43]->model, glm::vec3(0.04, 0.04, 0.04));
    meshes[43]->model = glm::rotate(meshes[43]->model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[43]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[44]->model = glm::translate(meshes[44]->model, glm::vec3(-35.5, 1.0, 2.5));
    meshes[44]->model = glm::scale(meshes[44]->model, glm::vec3(0.04, 0.04, 0.04));
    meshes[44]->model = glm::rotate(meshes[44]->model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[44]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    //table 2
    meshes[45]->model = glm::translate(meshes[45]->model, glm::vec3(-25.0, 1.0, -2.0));
    meshes[45]->model = glm::scale(meshes[45]->model, glm::vec3(0.04, 0.04, 0.04));
    meshes[45]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[46]->model = glm::translate(meshes[46]->model, glm::vec3(-29.5, 1.0, 2.5));
    meshes[46]->model = glm::scale(meshes[46]->model, glm::vec3(0.04, 0.04, 0.04));
    meshes[46]->model = glm::rotate(meshes[46]->model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[46]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[47]->model = glm::translate(meshes[47]->model, glm::vec3(-20.5, 1.0, 2.5));
    meshes[47]->model = glm::scale(meshes[47]->model, glm::vec3(0.04, 0.04, 0.04));
    meshes[47]->model = glm::rotate(meshes[47]->model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[47]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    //table 3
    meshes[48]->model = glm::translate(meshes[48]->model, glm::vec3(-2.0, 1.0, -2.0));
    meshes[48]->model = glm::scale(meshes[48]->model, glm::vec3(0.04, 0.04, 0.04));
    meshes[48]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[49]->model = glm::translate(meshes[49]->model, glm::vec3(2.5, 1.0, 2.5));
    meshes[49]->model = glm::scale(meshes[49]->model, glm::vec3(0.04, 0.04, 0.04));
    meshes[49]->model = glm::rotate(meshes[49]->model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[49]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[50]->model = glm::translate(meshes[50]->model, glm::vec3(-6.5, 1.0, 2.5));
    meshes[50]->model = glm::scale(meshes[50]->model, glm::vec3(0.04, 0.04, 0.04));
    meshes[50]->model = glm::rotate(meshes[50]->model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[50]->material.specular = glm::vec3(0.0, 0.0, 0.0);
    //Create model-view Matrix for every mesh
    for (int i = 0; i < meshes.size() ; i++) {
        meshes[i]->mv = cameras[activeCamera]->view * meshes[i]->model;
    }
}



//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------


void InitBuffers()
{
    GLuint vbo_vertices;
    GLuint vbo_normals;
    GLuint vbo_uvs;
    GLuint ibo_cube_elements;
    
    
    for (int i = 0; i < meshes.size(); i++) {

        // vbo for uvs
        glGenBuffers(1, &vbo_uvs);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
        glBufferData(GL_ARRAY_BUFFER, meshes[i]->uvs.size() * sizeof(glm::vec2),
            &meshes[i]->uvs[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vbo for vertices
        glGenBuffers(1, &vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER,
            meshes[i]->vertices.size() * sizeof(glm::vec3), &meshes[i]->vertices[0],
            GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // vbo for normals
        glGenBuffers(1, &vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
        glBufferData(GL_ARRAY_BUFFER,
            meshes[i]->normals.size() * sizeof(glm::vec3),
            &meshes[i]->normals[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Allocate memory for vao
        glGenVertexArrays(1, &meshes[i]->vao);

        // Bind to vao
        glBindVertexArray(meshes[i]->vao);


        //If mesh is a primitive create and bind vbo for indices
        if (meshes[i]->isPrimitive) {
            glGenBuffers(1, &ibo_cube_elements);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, sizeof(solid_cube_elements),
                solid_cube_elements, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
        }
       

        // Bind vertices to vao
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Bind normals to vao
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float),0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Bind to vao
        glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Stop bind to vao
        glBindVertexArray(0);
    }

    glUseProgram(program_id);

    // Make uniform vars
    uniform_mv = glGetUniformLocation(program_id, "mv");
    uniform_proj = glGetUniformLocation(program_id, "projection");
    uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
    uniform_material_ambient = glGetUniformLocation(program_id,
        "mat_ambient");
    uniform_material_diffuse = glGetUniformLocation(program_id,
        "mat_diffuse");
    uniform_specular = glGetUniformLocation(
        program_id, "mat_specular");
    uniform_material_power = glGetUniformLocation(
        program_id, "mat_power");
    uniform_hasTexture = glGetUniformLocation(program_id, "hasTexture");
    uniform_testColor = glGetUniformLocation(program_id, "testColor");
    uniform_lightsCounts = glGetUniformLocation(program_id, "lightsCount");

    
    //Fill uniform vars
    glUniform3fv(uniform_light_pos, lightCount, reinterpret_cast<GLfloat*>(lights));
    glUniform1i(uniform_lightsCounts, lightCount);
    
}

int main(int argc, char** argv)
{
    InitGlutGlew(argc, argv);
    InitLight();
    InitShaders();
    InitCameras();
    InitLoadObjects();
    InitMatrices();
    
    InitBuffers();
    timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glutKeyboardUpFunc(keyboardUpHandler);

    // Hide console window
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    // Main loop
    glutMainLoop();

    return 0;
}
