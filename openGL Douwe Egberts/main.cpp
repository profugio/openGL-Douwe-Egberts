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
const unsigned int objectCount = 20;

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
GLuint uniform_isPrimitive;
GLuint uniform_testColor;


//Primitives
const unsigned int primitiveObjectCount = 10;
vector<glm::vec3> primitive_vertices[primitiveObjectCount] = {
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
    

};
const unsigned int windowMeshesCount = 1;
vector<glm::vec3> primitive_window[windowMeshesCount]{
    {
    glm::vec3(15.0, 1.0, -15.0),
    glm::vec3(15.0, 1.0, 15.0),
    glm::vec3(15.0, 1.5, 15.0),
    glm::vec3(15.0, 1.5, -15.0),

    glm::vec3(15.5, 1.0, -15.0),
    glm::vec3(15.5, 1.0, 15.0),
    glm::vec3(15.5, 1.5, 15.0),
    glm::vec3(15.5, 1.5, -15.0)
    }
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
LightSource light;





void InitCameras() {
    cameras[0] = new WalkCamera();
    cameras[1] = new DroneCamera();
    activeCamera = 0;
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
    light.position = glm::vec3(4.0, 10.0, 4.0);
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



    // Attach to program_id
    glUseProgram(program_id);
    glm::vec3 color;

    for (int i = 0; i < meshes.size(); i++) {
        meshes[i]->mv = cameras[activeCamera]->view * meshes[i]->model;

        // Send mv
        glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(meshes[i]->mv));

        //Bind texture
        if (meshes[i]->texture_id != 0) {
            glBindTexture(GL_TEXTURE_2D, meshes[i]->texture_id);
        }
        else {
            color = glm::vec3(1.0, 0.0, 0.0);
            glUniform3fv(uniform_testColor, 1, glm::value_ptr(color));
        }
        
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

        //Fill uniform vars
        glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(cameras[activeCamera]->projection));
        glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(meshes[i]->material.ambient_color));
        glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(meshes[i]->material.diffuse_color));
        glUniform3fv(uniform_specular, 1, glm::value_ptr(meshes[i]->material.specular));
        glUniform1f(uniform_material_power, meshes[i]->material.power);
        glUniform1i(uniform_isPrimitive, false);
        
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
    glutCreateWindow("Hello OpenGL");
    glutDisplayFunc(Render);
    glutKeyboardFunc(keyboardHandler);
    glutTimerFunc(DELTA_TIME, Render, 0);

    glewInit();
}



void InitLoadObjects() {
    //Change these to some sort of dictionairy
    const char* objects[objectCount] = { "Objects/teapot.obj", "Objects/cylinder18.obj", "Objects/tableTest.obj", "Objects/box.obj", "Objects/cushion5.obj", "Objects/roundTable.obj", "Objects/roundTable.obj" , "Objects/roundTable.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj", "Objects/cushion5.obj" , "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/box.obj", "Objects/LampRail.obj", "Objects/LampRail.obj"};
    const char* textures[objectCount] = { "Textures/uvtemplate.bmp", "Textures/Yellobrk.bmp","Textures/woodTexture.bmp", "Textures/woodFloor.bmp", "Textures/greyCushion.bmp", "Textures/marble.bmp", "Textures/marble.bmp", "Textures/marble.bmp", "Textures/redCloth.bmp", "Textures/redCloth.bmp", "Textures/redCloth.bmp", "Textures/greyCushion.bmp", "Textures/redCloth.bmp", "Textures/greyCushion.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/whiteWall3.bmp", "Textures/ceiling.bmp", "Textures/lampRailTexture2.bmp", "Textures/lampRailTexture2.bmp"};
   
    vector<glm::vec3> vertices{};
    vector<glm::vec3> normals{};
    vector<glm::vec2> uvs{};
    glm::vec3 color;
    //Add all imported objects to mesh list
    for (int i = 0; i < objectCount; i++) {
        bool res = loadOBJ(objects[i], vertices, uvs, normals);
        for (int j = 0; j < vertices.size(); j++) {
            vertexList.push_back({vertices[j], normals[j], uvs[j]});
        }
        Mesh* m = new Mesh();
        m->CreateMesh(vertices, normals, uvs);
        m->texture_id = loadBMP(textures[i]);
        meshes.push_back(m);
        vertices.clear();
        normals.clear();
        uvs.clear();
        vertexList.clear();


       


    }
    //Add all primtive meshes to meshList
    for (int i = 0; i < primitiveObjectCount; i++) {
        Mesh* m = new Mesh();
        m->isPrimitive = true;
        for (int j = 0; j < 8; j++) {
            vertices.push_back(primitive_vertices[i][j]);
            uvs.push_back(glm::vec2(primitive_vertices[i][j].x / primitive_vertices[i][j].z, primitive_vertices[i][j].y / primitive_vertices[i][j].z));
            
        }
        for (int k = 0; k < 22; k += 3) {
            normals.push_back(glm::normalize(glm::cross(vertices[solid_cube_elements[k + 1]] - vertices[solid_cube_elements[k]], vertices[solid_cube_elements[k + 2]] - vertices[solid_cube_elements[k]])));
        }
        m->CreateMesh(vertices, normals, uvs);
        
        m->texture_id = loadBMP(textures[4]);
        meshes.push_back(m);
        vertices.clear();
        normals.clear();
        uvs.clear();
    }

    ////Test for new primitive window
    //for (int i = 0; i < windowMeshesCount; i++) {
    //    Mesh* m = new Mesh();
    //    m->isPrimitive = true;
    //    for (int j = 0; j < 8; j++) {
    //        vertices.push_back(primitive_window[i][j]);
    //        uvs.push_back(glm::vec2(primitive_window[i][j].x / primitive_window[i][j].z, primitive_window[i][j].y / primitive_window[i][j].z));

    //    }
    //    for (int k = 0; k < 22; k += 3) {
    //        normals.push_back(glm::normalize(glm::cross(vertices[solid_cube_elements[k + 1]] - vertices[solid_cube_elements[k]], vertices[solid_cube_elements[k + 2]] - vertices[solid_cube_elements[k]])));
    //    }
    //    m->CreateMesh(vertices, normals, uvs);
    //    meshes.push_back(m);
    //    vertices.clear();
    //    normals.clear();
    //    uvs.clear();
    //}
   
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
    for (int i = 0; i < meshes.size(); i++) {
        if (meshes[i]->isPrimitive) {
            meshes[i]->model = glm::translate(meshes[i]->model, glm::vec3(-35.0, 3.0, -20.0));
            meshes[i]->model = glm::rotate(meshes[i]->model, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
        }
       
     

    }
    //primitiveMeshes[10]->model = glm::translate(glm::mat4(), glm::vec3(5.0, 5.0, 5.0));
    meshes[0]->model = glm::translate(glm::mat4(), glm::vec3(3.0, 1.0, 0.0));
    meshes[1]->model = glm::mat4();
    meshes[2]->model = glm::translate(glm::mat4(), glm::vec3(-2.0, 1.0, 0.0));
    //Floor
    meshes[3]->model = glm::translate(glm::mat4(), glm::vec3(-15.0, 1.0, -10.0));
    meshes[3]->model = glm::scale(meshes[3]->model, glm::vec3(80.0, 0.2, 30.0));
    //Cushions
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
    //Round tables
    meshes[5]->model = glm::translate(meshes[5]->model, glm::vec3(-41.0, 1.15, -15.0));
    meshes[6]->model = glm::translate(meshes[6]->model, glm::vec3(-35.0, 1.15, -15.0));
    meshes[7]->model = glm::translate(meshes[7]->model, glm::vec3(-29.0, 1.15, -15.0));
    //Walls
    meshes[14]->model = glm::translate(meshes[14]->model, glm::vec3(-15.0, 0.0, -24.8));
    meshes[14]->model = glm::scale(meshes[14]->model, glm::vec3(80.0, 15.0, 0.2));
    meshes[14]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[15]->model = glm::translate(meshes[15]->model, glm::vec3(-8.0, 0.0, -23.15));
    meshes[15]->model = glm::scale(meshes[15]->model, glm::vec3(66, 15.0, 3.3));
    meshes[15]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    meshes[16]->model = glm::translate(meshes[16]->model, glm::vec3(-15.0, 0.0, 5.0));
    meshes[16]->model = glm::scale(meshes[16]->model, glm::vec3(80.0, 15.0, 0.2));
    meshes[16]->material.specular = glm::vec3(0.0, 0.0, 0.0);
    //Ceiling
    meshes[17]->model = glm::translate(meshes[17]->model, glm::vec3(-15.0, 15.0, -5.0));
    meshes[17]->model = glm::rotate(meshes[17]->model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[17]->model = glm::scale(meshes[17]->model, glm::vec3(40.0, 0.2, 80.0));
    meshes[17]->material.specular = glm::vec3(0.0, 0.0, 0.0);

    //Lamp Rails
    meshes[18]->model = glm::translate(meshes[18]->model, glm::vec3(-20.0, 13.0, -2.0));
    meshes[18]->model = glm::rotate(meshes[18]->model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[18]->model = glm::scale(meshes[18]->model, glm::vec3(0.8, 0.8, 0.8));

    meshes[19]->model = glm::translate(meshes[19]->model, glm::vec3(-20, 13.0, -19.0));
    meshes[19]->model = glm::rotate(meshes[19]->model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    meshes[19]->model = glm::scale(meshes[19]->model, glm::vec3(0.8, 0.8, 0.8));


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
    
    
    for (int i = 0; i < objectCount + primitiveObjectCount; i++) {

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
    uniform_isPrimitive = glGetUniformLocation(program_id, "isPrimitive");
    uniform_testColor = glGetUniformLocation(program_id, "testColor");
    
    //Fill uniform vars
    glUniform3fv(uniform_light_pos, 1, glm::value_ptr(light.position));

    
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
