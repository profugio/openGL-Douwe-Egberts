#version 430 core

// Uniform matrices
uniform mat4 mv;            //x
uniform mat4 projection;    //
uniform int lightsCount;
uniform vec3 light_pos[4];     //
uniform bool isPrimitive;
// Per-vertex inputs
in vec3 position;           //
//in vec3 normal;             //

//in vec2 uv;
out vec2 UV;


layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;



out VS_OUT
{
   vec3 N;
   vec3 L[4];
   vec3 V;
} vs_out;

void main()
{

        // Calculate view-space coordinate
        vec4 P = mv * vec4(pos, 1.0);

        // Calculate normal in view-space
        vs_out.N = mat3(mv) * normal;

        // Calculate light vector
        for (int i = 0; i < 4; i++) {
            vs_out.L[i] = light_pos[i] - P.xyz;
        }
        




        // Calculate view vector;
        vs_out.V = -P.xyz;

        // Calculate the clip-space position of each vertex
        gl_Position = projection * P;

        //texture
        UV = uv;
    

 

}