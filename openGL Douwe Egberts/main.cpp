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
#include "model.h"

using namespace std;


//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 800, HEIGHT = 600;
const unsigned int objectCount = 58;

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


vector<Model*> models;
vector<Mesh*> meshes;

//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

// ID's
GLuint program_id;

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

GLushort solid_cube_elements[] = {
    0,1,2,0,2,3,// front
    1,5,6,1,6,2,//right side
    0,4,7,0,3,7,//left side
    4,5,6,4,6,7,//back
    0,1,5,0,5,4, // bottom
    3,2,6,3,6,7, //top
    0,3,1,2,5,6,4,7  // top to bottom
};
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
        for (int i = 0; i < models.size(); i++) {
            delete models[i];
        }
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
    //Animations
    models[0]->Rotate(0.01f, glm::vec3(0.0, 1.0, 0.0));


    // Attach to program_id
    glUseProgram(program_id);

    for (int i = 0; i < models.size(); i++) {
        if (!(models[i]->isRoof && activeCamera == DRONE_CAMERA)) {
            models[i]->CreateModelViewMatrix(cameras[activeCamera]->view);

            for (int j = 0; j < models[i]->meshes.size(); j++) {

                //Bind texture
                glBindTexture(GL_TEXTURE_2D, models[i]->meshes[j]->texture_id);
                glUniform1i(uniform_hasTexture, true);

                //Fill uniform vars
                glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(models[i]->meshes[j]->mv));
                glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(cameras[activeCamera]->projection));
                glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(models[i]->meshes[j]->material.ambient_color));
                glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(models[i]->meshes[j]->material.diffuse_color));
                glUniform3fv(uniform_specular, 1, glm::value_ptr(models[i]->meshes[j]->material.specular));
                glUniform1f(uniform_material_power, models[i]->meshes[j]->material.power);
                glBindVertexArray(models[i]->meshes[j]->vao);
                //If mesh is primitive draw using elements, else simply draw triangles
                if (models[i]->meshes[j]->isPrimitive) {
                    glDrawElements(GL_TRIANGLES, sizeof(solid_cube_elements) / sizeof(GLushort),
                        GL_UNSIGNED_SHORT, 0);
                }
                else {
                    glDrawArrays(GL_TRIANGLES, 0, models[i]->meshes[j]->vertices.size());
                }

                glBindVertexArray(0);
            }
        }
           
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
    const char* objects[objectCount] = { "Objects/teapot.obj", "Objects/cylinder18.obj", "Objects/smallTable.obj", "Objects/box.obj", "Objects/cushion5.obj", "Objects/roundTable.obj", "Objects/roundTable.obj" , "Objects/roundTable.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj" , "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/LampRailRight.obj", "Objects/LampRailLeft.obj", "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/cushion5.obj", "Objects/cushion5.obj" , "Objects/cushion5.obj" , "Objects/cushion5.obj" , "Objects/cushion5.obj" , "Objects/cushion5.obj", "Objects/bookCase.obj", "Objects/roundTable.obj", "Objects/roundTable.obj" , "Objects/roundTable.obj", "Objects/smallTable.obj", "Objects/armChair.obj", "Objects/armChair.obj", "Objects/chair2.obj", "Objects/chair2.obj" , "Objects/chair2.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj", "Objects/chair1.obj" , "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj"};
    const char* textures[objectCount] = { "Textures/uvtemplate.bmp", "Textures/Yellobrk.bmp","Textures/woodTexture.bmp", "Textures/woodFloor.bmp", "Textures/greyCushion.bmp", "Textures/roundTableMarble.bmp", "Textures/roundTableMarble.bmp", "Textures/roundTableMarble.bmp", "Textures/redCloth.bmp", "Textures/redCloth.bmp", "Textures/redCloth.bmp", "Textures/greyCushion.bmp", "Textures/redCloth.bmp", "Textures/greyCushion.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/ceiling.bmp", "Textures/lampRailTexture.bmp", "Textures/lampRailTexture.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/greyCushion.bmp" , "Textures/greyCushion.bmp" , "Textures/redCloth.bmp" , "Textures/redCloth.bmp" , "Textures/greyCushion.bmp" , "Textures/redCloth.bmp", "Textures/bookCaseTexture.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp" , "Textures/black_metal.bmp", "Textures/woodTexture.bmp", "Textures/armChair.bmp", "Textures/armChair.bmp" , "Textures/black_metal.bmp" , "Textures/black_metal.bmp" , "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp", "Textures/black_metal.bmp" , "Textures/greyCushion.bmp" , "Textures/greyCushion.bmp" , "Textures/redCloth.bmp" , "Textures/redCloth.bmp" , "Textures/greyCushion.bmp" , "Textures/redCloth.bmp", "Textures/redCloth.bmp"};
   
    vector<glm::vec3> vertices{};
    vector<glm::vec3> normals{};
    vector<glm::vec2> uvs{};
    glm::vec3 color;
    GLuint id;
    Model* model;
    //Add all imported objects to mesh list
    for (int i = 0; i < objectCount; i++) {
        if (i == 0 || (objects[i] != objects[i - 1])) {
            vertices.clear();
            normals.clear();
            uvs.clear();
            bool res = loadOBJ(objects[i], vertices, uvs, normals);
        }
        model = new Model();
        Mesh* m = new Mesh();
        m->CreateMesh(vertices, normals, uvs);
        if (i == 0 || (textures[i] != textures[i - 1])) {
            id = loadBMP(textures[i]);
           
        }
        else{
          
            id = models[models.size()-1]->meshes[0]->texture_id;
        }
        if (i == 17 || i == 18 || i == 19) {
            model->isRoof = true;
        }
        m->texture_id = id;
        model->meshes.push_back(m);
        models.push_back(model);       
    }
 
    vertices.clear();
    normals.clear();
    uvs.clear();
   
    //Add all couch models 
    for (int i = 0; i < 2; i++) {
        model = new Model();
        
        for (int j = 0; j < couchMeshCount; j++) {

            Mesh* m = new Mesh();
            m->isPrimitive = true;
            for (int k = 0; k < 8; k++) {
                vertices.push_back(couch_vertices[j][k]);
                uvs.push_back(glm::vec2(couch_vertices[j][k].x / couch_vertices[j][k].z, couch_vertices[j][k].y / couch_vertices[j][k].z));

            }
            for (int l = 0; l < 22; l += 3) {
                normals.push_back(glm::normalize(glm::cross(vertices[solid_cube_elements[l + 1]] - vertices[solid_cube_elements[l]], vertices[solid_cube_elements[l + 2]] - vertices[solid_cube_elements[l]])));
            }
            m->CreateMesh(vertices, normals, uvs);

            if (j >= couchMeshCount - 6) {
                m->texture_id = loadBMP(textures[33]);
            }
            else {
                m->texture_id = loadBMP(textures[4]);
            }
            
            model->meshes.push_back(m);

            vertices.clear();
            normals.clear();
            uvs.clear();
        }
        models.push_back(model);
    }
   
    
    //Add all windows
    for (int l = 0; l < 3; l++) {
        model = new Model();
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
            model->meshes.push_back(m);
            vertices.clear();
            normals.clear();
            uvs.clear();
        }
        models.push_back(model);
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

    //Teapot
    models[0]->Translate(glm::vec3(1.5, 4.20, -12.5));
    models[0]->Scale(glm::vec3(0.8, 0.8, 0.8));
    //Couches
    models[58]->Translate(glm::vec3(-35.0, 3.0, -20.0));
    models[58]->Rotate(-90.0f, glm::vec3(0.0, 1.0, 0.0));
    models[58]->SetAmbient(glm::vec3(0.25, 0.25, 0.15));
    models[58]->SetDiffuse(glm::vec3(0.6,0.6,0.4));
    models[58]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

    models[59]->Translate(glm::vec3(1.5, 3.0, -20.0));
    models[59]->Rotate(-90.0f, glm::vec3(0.0, 1.0, 0.0));
    models[59]->SetAmbient(glm::vec3(0.25, 0.25, 0.15));
    models[59]->SetDiffuse(glm::vec3(0.6, 0.6, 0.4));
    models[59]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

    //Windows
    models[60]->Translate(glm::vec3(25.2, 7.4, -17.0));
    models[60]->Rotate(-90.0f, glm::vec3(0.0, 1.0, 0.0));

    models[61]->Translate(glm::vec3(25.2, 7.4, -8.3));
    models[61]->Rotate(-90.0f, glm::vec3(0.0, 1.0, 0.0));

    models[62]->Translate(glm::vec3(25.2, 7.4, 0.3));
    models[62]->Rotate(-90.0f, glm::vec3(0.0, 1.0, 0.0));

    //Cushions couch close to window
    models[51]->Translate(glm::vec3(-5.5, 3.3, -20.5));
    models[51]->Rotate(-50.0f, glm::vec3(0.0, 1.0, 0.0));
    models[51]->Rotate(20.0f, glm::vec3(0.0, 0.0, 1.0));
    models[51]->Scale(glm::vec3(3.0, 3.0, 3.0));

    models[52]->Translate(glm::vec3(-4.2, 3.3, -20.5));
    models[52]->Rotate(-70.0f, glm::vec3(0.0, 1.0, 0.0));
    models[52]->Rotate(20.0f, glm::vec3(0.0, 0.0, 1.0));
    models[52]->Scale(glm::vec3(3.0, 3.0, 3.0));

    models[53]->Translate(glm::vec3(-3.0, 3.0, -20.3));
    models[53]->Rotate(-70.0f, glm::vec3(0.0, 1.0, 0.0));
    models[53]->Rotate(70.0f, glm::vec3(0.0, 0.0, 1.0));
    models[53]->Rotate(-35.0f, glm::vec3(1.0, 0.0, 0.0));
    models[53]->Scale(glm::vec3(3.0, 3.0, 3.0));

    models[54]->Translate(glm::vec3(1.0, 3.3, -20.5));
    models[54]->Rotate(-90.0f, glm::vec3(0.0, 1.0, 0.0));
    models[54]->Rotate(-3.0f, glm::vec3(0.0, 0.0, 1.0));
    models[54]->Scale(glm::vec3(3.0, 3.0, 3.0));

    models[55]->Translate(glm::vec3(2.5, 3.3, -20.5));
    models[55]->Rotate(-100.0f, glm::vec3(0.0, 1.0, 0.0));
    models[55]->Rotate(20.0f, glm::vec3(0.0, 0.0, 1.0));
    models[55]->Scale(glm::vec3(3.0, 3.0, 3.0));

    models[56]->Translate(glm::vec3(7.2, 3.3, -20.5));
    models[56]->Rotate(-135.0f, glm::vec3(0.0, 1.0, 0.0));
    models[56]->Rotate(20.0f, glm::vec3(0.0, 0.0, 1.0));
    models[56]->Scale(glm::vec3(3.0, 3.0, 3.0));

    models[57]->Translate(glm::vec3(8.6, 3.3, -20.2));
    models[57]->Rotate(-155.0f, glm::vec3(0.0, 1.0, 0.0));
    models[57]->Rotate(20.0f, glm::vec3(0.0, 0.0, 1.0));
    models[57]->Scale(glm::vec3(3.0, 3.0, 3.0));

    //Cushions couch far from window
    models[4]->Translate(glm::vec3(-42.0, 3.3, -20.5));
    models[4]->Rotate(-50.0f, glm::vec3(0.0, 1.0, 0.0));
    models[4]->Rotate(20.0f, glm::vec3(0.0, 0.0, 1.0));
    models[4]->Scale(glm::vec3(3.0, 3.0, 3.0));

    models[8]->Translate(glm::vec3(-40.7, 3.3, -20.5));
    models[8]->Rotate(-70.0f, glm::vec3(0.0, 1.0, 0.0));
    models[8]->Rotate(20.0f, glm::vec3(0.0, 0.0, 1.0));
    models[8]->Scale(glm::vec3(3.0, 3.0, 3.0));

    models[9]->Translate(glm::vec3(-39.5, 3.0, -20.3));
    models[9]->Rotate(-70.0f, glm::vec3(0.0, 1.0, 0.0));
    models[9]->Rotate(70.0f, glm::vec3(0.0, 0.0, 1.0));
    models[9]->Rotate(-35.0f, glm::vec3(1.0, 0.0, 0.0));
    models[9]->Scale(glm::vec3(3.0, 3.0, 3.0));

    models[10]->Translate(glm::vec3(-35.5, 3.3, -20.5));
    models[10]->Rotate(-90.0f, glm::vec3(0.0, 1.0, 0.0));
    models[10]->Rotate(-3.0f, glm::vec3(0.0, 0.0, 1.0));
    models[10]->Scale(glm::vec3(3.0, 3.0, 3.0));

    models[11]->Translate(glm::vec3(-34.0, 3.3, -20.5));
    models[11]->Rotate(-100.0f, glm::vec3(0.0, 1.0, 0.0));
    models[11]->Rotate(20.0f, glm::vec3(0.0, 0.0, 1.0));
    models[11]->Scale(glm::vec3(3.0, 3.0, 3.0));

    models[12]->Translate(glm::vec3(-29.3, 3.3, -20.5));
    models[12]->Rotate(-135.0f, glm::vec3(0.0, 1.0, 0.0));
    models[12]->Rotate(20.0f, glm::vec3(0.0, 0.0, 1.0));
    models[12]->Scale(glm::vec3(3.0, 3.0, 3.0));

    models[13]->Translate(glm::vec3(-27.9, 3.3, -20.2));
    models[13]->Rotate(-155.0f, glm::vec3(0.0, 1.0, 0.0));
    models[13]->Rotate(20.0f, glm::vec3(0.0, 0.0, 1.0));
    models[13]->Scale(glm::vec3(3.0, 3.0, 3.0));

    //Small table corner
   models[2]->Translate(glm::vec3(1.5, 1.85, -12.5));
   models[2]->Rotate(40.0f, glm::vec3(0.0, 1.0, 0.0));
   models[2]->Scale(glm::vec3(1.8, 1.8, 1.8));
   models[36]->Translate(glm::vec3(3.0, 1.7, -15.0));
   models[36]->Scale(glm::vec3(1.3, 1.3, 1.3));

   //Armchairs
   models[37]->Translate(glm::vec3(5.5, 1.25, -11.5));
   models[37]->Scale(glm::vec3(0.08, 0.08, 0.08));
   models[37]->Rotate(170.0f, glm::vec3(0.0, 1.0, 0.0));
   models[37]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[38]->Translate(glm::vec3(-2.5, 1.25, -11.5));
   models[38]->Scale(glm::vec3(0.08, 0.08, 0.08));
   models[38]->Rotate(20.0f, glm::vec3(0.0, 1.0, 0.0));
   models[38]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));
   //Floor
   models[3]->Translate(glm::vec3(-15.0, 1.0, -10.0));
   models[3]->Scale(glm::vec3(80.0, 0.2, 30.0));
  
   //Round marble tables
   models[5]->Translate(glm::vec3(-41.0, 1.15, -15.0));
   models[6]->Translate(glm::vec3(-35.0, 1.15, -15.0));
   models[7]->Translate(glm::vec3(-29.0, 1.15, -15.0));
   //Walls
   models[14]->Translate(glm::vec3(-15.0, 0.0, -24.8));
   models[14]->Scale(glm::vec3(80.0, 20.0, 0.2));
   models[14]->SetDiffuse(glm::vec3(0.2, 0.2, 0.2));
   models[14]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[15]->Translate(glm::vec3(-8.0, 0.0, -23.15));
   models[15]->Scale(glm::vec3(66, 20.0, 3.3));
   models[15]->SetDiffuse(glm::vec3(0.2, 0.2, 0.2));
   models[15]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[16]->Translate(glm::vec3(-15.0, 0.0, 5.0));
   models[16]->Scale(glm::vec3(80.0, 20.0, 0.2));
   models[16]->SetDiffuse(glm::vec3(0.2, 0.2, 0.2));
   models[16]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));
   //Ceiling
   models[17]->Translate(glm::vec3(-15.0, 20.0, -5.0));
   models[17]->Rotate(90.0f, glm::vec3(0.0, 1.0, 0.0));
   models[17]->Scale(glm::vec3(40.0, 0.2, 80.0));
   models[17]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   //Lamp Rails right
   models[18]->Translate(glm::vec3(-20.0, 18.0, -2.0));
   models[18]->Rotate(90.0f, glm::vec3(0.0, 1.0, 0.0));
   models[18]->Scale(glm::vec3(0.8, 0.8, 0.8));

   //Lamp rails left
   models[19]->Translate(glm::vec3(-23, 18.0, -16.5));
   models[19]->Scale(glm::vec3(0.8, 0.8, 0.8));
   models[19]->Rotate(-90.0f, glm::vec3(0.0, 1.0, 0.0));


   //Window seating
   models[20]->Translate(glm::vec3(24.0, 1.0, -8.5));
   models[20]->Scale(glm::vec3(3.0, 3.7, 27.0));

   models[21]->Translate( glm::vec3(23.0, 4.5, -8.5));
   models[21]->Scale(glm::vec3(5.0, 0.3, 27.0));

   //Walls between windows
   models[22]->Translate(glm::vec3(23.5, 4.5, -12.6));
   models[22]->Scale(glm::vec3(3.7, 15.5, 1.5));

   models[23]->Translate(glm::vec3(23.5, 4.5, -4.0));
   models[23]->Scale(glm::vec3(3.7, 15.5, 1.5));

   models[24]->Translate(glm::vec3(23.5, 4.5, -21.0));
   models[24]->Scale(glm::vec3(3.7, 15.5, 1.0));

   models[25]->Translate(glm::vec3(23.5, 4.5, 4.4));
   models[25]->Scale(glm::vec3(3.7, 15.5, 1.0));

   //Window cushions
   models[26]->Translate(glm::vec3(23.9, 5.8, -19.8));
   models[26]->Rotate(50.0f, glm::vec3(0.0, 1.0, 0.0));
   models[26]->Scale(glm::vec3(3.5, 3.5, 3.5));

   models[27]->Translate(glm::vec3(23.9, 5.8, -14.1));
   models[27]->Rotate(-50.0f, glm::vec3(0.0, 1.0, 0.0));
   models[27]->Scale(glm::vec3(3.5, 3.5, 3.5));

   models[28]->Translate(glm::vec3(23.9, 5.8, -11.0));
   models[28]->Rotate(50.0f, glm::vec3(0.0, 1.0, 0.0));
   models[28]->Scale(glm::vec3(3.5, 3.5, 3.5));

   models[29]->Translate(glm::vec3(23.9, 5.8, -5.2));
   models[29]->Rotate(-50.0f, glm::vec3(0.0, 1.0, 0.0));
   models[29]->Scale(glm::vec3(3.5, 3.5, 3.5));

   models[30]->Translate(glm::vec3(23.9, 5.8, -2.4));
   models[30]->Rotate(50.0f, glm::vec3(0.0, 1.0, 0.0));
   models[30]->Scale(glm::vec3(3.5, 3.5, 3.5));

   models[31]->Translate(glm::vec3(23.9, 5.8, 3.3));
   models[31]->Rotate(-50.0f, glm::vec3(0.0, 1.0, 0.0));
   models[31]->Scale(glm::vec3(3.5, 3.5, 3.5));

   //BookCase
   models[32]->Translate(glm::vec3(-20.0, 10.0, -16.0));
   models[32]->Scale(glm::vec3(1.0, 1.4, 1.0));

   //Black round tables
   models[33]->Translate(glm::vec3(-40.0, 1.15, 2.5));
   models[33]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[34]->Translate(glm::vec3(-25.0, 1.15, 2.5));
   models[34]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[35]->Translate(glm::vec3(-2.0, 1.15, 2.5));
   models[35]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   //Chairs at marble tables
   models[39]->Translate(glm::vec3(-41.0, 3.5, -11.0));
   models[39]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[40]->Translate(glm::vec3(-35.0, 3.5, -11.0));
   models[40]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[41]->Translate(glm::vec3(-29.0, 3.5, -11.0));
   models[41]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   //Chairs at black tables
   //table 1
   models[42]->Translate(glm::vec3(-40.0, 1.0, -2.0));
   models[42]->Scale(glm::vec3(0.04, 0.04, 0.04));
   models[42]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[43]->Translate(glm::vec3(-44.5, 1.0, 2.5));
   models[43]->Scale(glm::vec3(0.04, 0.04, 0.04));
   models[43]->Rotate(90.0, glm::vec3(0.0, 1.0, 0.0));
   models[43]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[44]->Translate(glm::vec3(-35.5, 1.0, 2.5));
   models[44]->Scale(glm::vec3(0.04, 0.04, 0.04));
   models[44]->Rotate(-90.0f, glm::vec3(0.0, 1.0, 0.0));
   models[44]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   //table 2
   models[45]->Translate(glm::vec3(-25.0, 1.0, -2.0));
   models[45]->Scale(glm::vec3(0.04, 0.04, 0.04));
   models[45]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[46]->Translate(glm::vec3(-29.5, 1.0, 2.5));
   models[46]->Scale(glm::vec3(0.04, 0.04, 0.04));
   models[46]->Rotate(90.0f, glm::vec3(0.0, 1.0, 0.0));
   models[46]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[47]->Translate(glm::vec3(-20.5, 1.0, 2.5));
   models[47]->Scale(glm::vec3(0.04, 0.04, 0.04));
   models[47]->Rotate(-90.0f, glm::vec3(0.0, 1.0, 0.0));
   models[47]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   //table 3
   models[48]->Translate(glm::vec3(-2.0, 1.0, -2.0));
   models[48]->Scale(glm::vec3(0.04, 0.04, 0.04));
   models[48]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[49]->Translate( glm::vec3(2.5, 1.0, 2.5));
   models[49]->Scale(glm::vec3(0.04, 0.04, 0.04));
   models[49]->Rotate(-90.0f, glm::vec3(0.0, 1.0, 0.0));
   models[49]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));

   models[50]->Translate(glm::vec3(-6.5, 1.0, 2.5));
   models[50]->Scale(glm::vec3(0.04, 0.04, 0.04));
   models[50]->Rotate(90.0f, glm::vec3(0.0, 1.0, 0.0));
   models[50]->SetSpecular(glm::vec3(0.0, 0.0, 0.0));
    
   
    //Create model-view Matrix for every mesh
    for (int i = 0; i < models.size() ; i++) {
        models[i]->CreateModelViewMatrix(cameras[activeCamera]->view);
        //meshes[i]->mv = cameras[activeCamera]->view * meshes[i]->model;
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
    
    
    for (int i = 0; i < models.size(); i++) {
        for (int j = 0; j < models[i]->meshes.size(); j++) {
            // vbo for uvs
            glGenBuffers(1, &vbo_uvs);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
            glBufferData(GL_ARRAY_BUFFER, models[i]->meshes[j]->uvs.size() * sizeof(glm::vec2),
                &models[i]->meshes[j]->uvs[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // vbo for vertices
            glGenBuffers(1, &vbo_vertices);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
            glBufferData(GL_ARRAY_BUFFER,
                models[i]->meshes[j]->vertices.size() * sizeof(glm::vec3), &models[i]->meshes[j]->vertices[0],
                GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // vbo for normals
            glGenBuffers(1, &vbo_normals);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
            glBufferData(GL_ARRAY_BUFFER,
                models[i]->meshes[j]->normals.size() * sizeof(glm::vec3),
                &models[i]->meshes[j]->normals[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Allocate memory for vao
            glGenVertexArrays(1, &models[i]->meshes[j]->vao);

            // Bind to vao
            glBindVertexArray(models[i]->meshes[j]->vao);


            //If mesh is a primitive create and bind vbo for indices
            if (models[i]->meshes[j]->isPrimitive) {
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
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Bind normals to vao
            glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Bind to vao
            glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Stop bind to vao
            glBindVertexArray(0);
        }

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
