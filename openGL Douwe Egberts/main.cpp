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
const unsigned int objectCount = 5;

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

vector<Vertex> vertexList;

//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

// ID's
GLuint program_id;
GLuint vao[objectCount];

//Moved to model class
GLuint texture_id[objectCount];

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
glm::mat4 primitiveModels[primitiveObjectCount];
glm::mat4 primitiveMvs[primitiveObjectCount];
GLuint primitiveVao[primitiveObjectCount];
vector<glm::vec3> primitive_vertices[primitiveObjectCount] = {
    //Right armrest
    {
        // front
        glm::vec3(-1.5, -1.0, 1.0),
        glm::vec3(1.0, -1.0, 1.0),
        glm::vec3(1.0, 1.0, 1.0),
        glm::vec3(-1.5, 1.0, 1.0),
        // back
        glm::vec3(-1.5, -1.0, 0.5),
        glm::vec3(1.0, -1.0, 0.5),
        glm::vec3(1.0, 1.0, 0.5),
        glm::vec3(-1.5, 1.0, 0.5)
    },
    //Left armrest
    {
        // front 
        glm::vec3(-1.5, -1.0 , -8.0),
        glm::vec3(1.0, -1.0, -8.0),
        glm::vec3(1.0, 1.0, -8.0),
        glm::vec3(-1.5, 1.0, -8.0),
        //back
        glm::vec3(-1.5, -1.0 , -8.5),
        glm::vec3(1.0, -1.0, -8.5),
        glm::vec3(1.0, 1.0, -8.5),
        glm::vec3(-1.5, 1.0, -8.5)

    },
    //Backsupport
    {
        //front
        glm::vec3(-1.5, -1.0, 1.0),
        glm::vec3(-1.5, -1.0, -8.5),
        glm::vec3(-1.5, 1.5, -8.5),
        glm::vec3(-1.5, 1.5, 1.0),
        //back
        glm::vec3(-1.0, -1.0, 1.0),
        glm::vec3(-1.0, -1.0, -8.5),
        glm::vec3(-1.0, 1.5, -8.5),
        glm::vec3(-1.0, 1.5, 1.0),
    },
    //sit cushion
    {
        //front
        glm::vec3(-1.5, -1.0, 1.0),
        glm::vec3(-1.5, -1.0, -8.5),
        glm::vec3(1.0, -1.0, -8.5),
        glm::vec3(1.0, -1.0, 1.0),
        //back
        glm::vec3(-1.5, -0.5, 1.0),
        glm::vec3(-1.5, -0.5, -8.5),
        glm::vec3(1.0, -0.5, -8.5),
        glm::vec3(1.0, -0.5, 1.0),
    },
    //front right leg
    {
        //front
        glm::vec3(0.6, -1.0, -7.9),
        glm::vec3(0.6, -1.0, -8.1),
        glm::vec3(0.8, -1.0, -8.1),
        glm::vec3(0.8, -1.0, -7.9),
        //back
        glm::vec3(0.6, -1.8, -7.9),
        glm::vec3(0.6, -1.8, -8.1),
        glm::vec3(0.8, -1.8, -8.1),
        glm::vec3(0.8, -1.8, -7.9)
    },
    //front left leg
    {
                //front
        glm::vec3(0.6, -1.0, 0.6),
        glm::vec3(0.6, -1.0, 0.4),
        glm::vec3(0.8, -1.0, 0.4),
        glm::vec3(0.8, -1.0, 0.6),
        //back
        glm::vec3(0.6, -1.8, 0.6),
        glm::vec3(0.6, -1.8, 0.4),
        glm::vec3(0.8, -1.8, 0.4),
        glm::vec3(0.8, -1.8, 0.6)
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
        glm::vec3(-1.1, -1.0, 0.6),
        glm::vec3(-1.1, -1.0, 0.4),
        glm::vec3(-0.9, -1.0, 0.4),
        glm::vec3(-0.9, -1.0, 0.6),
        //back
        glm::vec3(-1.1, -1.8, 0.6),
        glm::vec3(-1.1, -1.8, 0.4),
        glm::vec3(-0.9, -1.8, 0.4),
        glm::vec3(-0.9, -1.8, 0.6)
    },
    //back middle leg
    {
        //front
        glm::vec3(-1.1, -1.0, -3.65),
        glm::vec3(-1.1, -1.0, -3.85),
        glm::vec3(-0.9, -1.0, -3.85),
        glm::vec3(-0.9, -1.0, -3.65),
        //back
        glm::vec3(-1.1, -1.8, -3.65),
        glm::vec3(-1.1, -1.8, -3.85),
        glm::vec3(-0.9, -1.8, -3.85),
        glm::vec3(-0.9, -1.8, -3.65)
    },
    //front middle leg
    {
        //front
        glm::vec3(0.6, -1.0, -3.65),
        glm::vec3(0.6, -1.0, -3.85),
        glm::vec3(0.8, -1.0, -3.85),
        glm::vec3(0.8, -1.0, -3.65),
        //back
        glm::vec3(0.6, -1.8, -3.65),
        glm::vec3(0.6, -1.8, -3.85),
        glm::vec3(0.8, -1.8, -3.85),
        glm::vec3(0.8, -1.8, -3.65)
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

GLushort cube_elements[] = {
    0,1,1,2,2,3,3,0,  // front
    0,4,1,5,3,7,2,6,  // front to back
    4,5,5,6,6,7,7,4   //back
};
// Matrices will move to Model/camera class;


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
    if(keyBuffer[key] == false) {
        keyBuffer[key] = true;
    }
}

void keyboardUpHandler(unsigned char key, int a, int b) {
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
    light.position = glm::vec3(4.0, 4.0, 4.0);
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

    // Do transformation

    // Attach to program_id
    glUseProgram(program_id);
    glm::vec3 color;
    //Primitives
    for (int i = 0; i < primitiveObjectCount; i++) {
        if (i == 0) {
            color = glm::vec3(1.0, 0.0, 0.0);
        }
        else if (i == 1) {
            color = glm::vec3(0.0, 1.0, 0.0);
        }
        else if (i == 2) {
            color = glm::vec3(0.0, 0.0, 1.0);
        }
        primitiveMvs[i] = cameras[activeCamera]->view * primitiveModels[i];
        glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(primitiveMvs[i]));
        glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(cameras[activeCamera]->projection));
        glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(materials[0].ambient_color));
        glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(materials[0].diffuse_color));
        glUniform3fv(uniform_specular, 1, glm::value_ptr(materials[0].specular));
        glUniform1f(uniform_material_power, materials[0].power);
        glm::vec3 color = glm::vec3(1.0, 0.0, 0.0);
        glUniform1i(uniform_isPrimitive, true);
        glUniform3fv(uniform_testColor, 1, glm::value_ptr(color));
        // Send vao
        glBindVertexArray(primitiveVao[i]);

        glDrawElements(GL_LINES, sizeof(cube_elements) / sizeof(GLushort),
            GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
   
    
  


    for (int i = 0; i < objectCount; i++) {
        meshes[i]->mv = cameras[activeCamera]->view * meshes[i]->model;

        // Send mv
        glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(meshes[i]->mv));

        //Bind texture
        glBindTexture(GL_TEXTURE_2D, texture_id[i]);

        //Fill uniform vars for material
        glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(cameras[activeCamera]->projection));
        glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(meshes[i]->material.ambient_color));
        glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(meshes[i]->material.diffuse_color));
        glUniform3fv(uniform_specular, 1, glm::value_ptr(meshes[i]->material.specular));
        glUniform1f(uniform_material_power, meshes[i]->material.power);
        glUniform1i(uniform_isPrimitive, false);
        // Send vao

        glBindVertexArray(vao[i]);
        glDrawArrays(GL_TRIANGLES, 0, meshes[i]->vertices.size());
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
    const char* objects[objectCount] = { "teapot.obj", "cylinder18.obj", "tableTest.obj", "plane.obj", "cushion5.obj"};
    const char* textures[objectCount] = { "Textures/uvtemplate.bmp", "Textures/Yellobrk.bmp","Textures/woodTexture.bmp", "Textures/grass.bmp", "Textures/redCloth.bmp"};
   
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec2> uvs;
    for (int i = 0; i < objectCount; i++) {
        bool res = loadOBJ(objects[i], vertices, uvs, normals);
        for (int j = 0; j < vertices.size(); j++) {
            vertexList.push_back({vertices[j], normals[j], uvs[j]});
        }
        Mesh* m = new Mesh();
        m->CreateMesh(vertices, normals, uvs);
        meshes.push_back(m);
        vertices.clear();
        normals.clear();
        uvs.clear();
        vertexList.clear();
        texture_id[i] = loadBMP(textures[i]);
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
    //mv for every object
    for (int i = 0; i < primitiveObjectCount; i++) {
        primitiveModels[i] = glm::translate(glm::mat4(), glm::vec3(0.0, 20.0, 0.0));

    }
   
    meshes[0]->model = glm::translate(glm::mat4(), glm::vec3(3.0, 1.0, 0.0));
    meshes[1]->model = glm::mat4();
    meshes[2]->model = glm::translate(glm::mat4(), glm::vec3(-2.0, 1.0, 0.0));
    meshes[3]->model = glm::translate(glm::mat4(), glm::vec3(0.0, 1.0, -3.0));
    meshes[3]->model = glm::scale(meshes[3]->model, glm::vec3(10.0, 0.0, 10.0));
    meshes[4]->model = glm::translate(meshes[4]->model, glm::vec3(0.0, 20.0, 0.0));
    meshes[4]->model = glm::scale(meshes[4]->model, glm::vec3(3.0,3.0,3.0));


    for (int i = 0; i < objectCount; i++) {
        meshes[i]->mv = cameras[activeCamera]->view * meshes[i]->model;
    }
    
    for (int i = 0; i < primitiveObjectCount; i++) {
        primitiveMvs[i] = cameras[activeCamera]->view * primitiveModels[i];
    }

}



//------------------------------------------------------------
// void InitBuffers()
// Allocates and fills buffers
//------------------------------------------------------------


void InitBuffers()
{
    GLuint position_id, normal_id;
    GLuint vbo_vertices;
    GLuint vbo_normals;
    GLuint vbo_uvs;
    GLuint ibo_cube_elements;
    GLuint vbo;
    //GLuint layout;

    // Get vertex attributes
    position_id = glGetAttribLocation(program_id, "position");
    //layout = glGetAttribLocation(program_id, "layout");
    //color_id = glGetAttribLocation(program_id, "color");
    normal_id = glGetAttribLocation(program_id, "normal");
    GLuint uv_id = glGetAttribLocation(program_id, "uv");
    
    for (int i = 0; i < objectCount; i++) {




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

        //glGenBuffers(1, &vbo);
        //glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //glBufferData(GL_ARRAY_BUFFER, vertexList.size() * sizeof(Vertex), vertexList.data(), GL_STATIC_DRAW);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);


        // Allocate memory for vao
        glGenVertexArrays(1, &vao[i]);

        // Bind to vao
        glBindVertexArray(vao[i]);

        
        //glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vertex)-(5*sizeof(float))));
        //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vertex)-(2*sizeof(float))));
        //glEnableVertexAttribArray(0);
        //glEnableVertexAttribArray(1);
        //glEnableVertexAttribArray(2);
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    //Primitive----------
    for (int i = 0; i < primitiveObjectCount; i++) {
        glGenBuffers(1, &vbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, primitive_vertices[i].size() * sizeof(glm::vec3), &primitive_vertices[i][0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &ibo_cube_elements);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements),
            cube_elements, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


        // Get vertex attributes
       // position_id = glGetAttribLocation(program_id, "position");


        // Allocate memory for vao
        glGenVertexArrays(1, &primitiveVao[i]);

        // Bind to vao
        glBindVertexArray(primitiveVao[i]);

        // Bind vertices to vao
        glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        //glVertexAttribPointer(position_id, 3, GL_FLOAT, GL_FALSE, 0, 0);
        //glEnableVertexAttribArray(position_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);


        // Stop bind to vao
        glBindVertexArray(0);

    }
   
 

   
    //End primitive------------
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
