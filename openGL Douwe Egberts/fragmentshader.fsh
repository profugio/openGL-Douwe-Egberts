#version 430 core

uniform int lightsCount;
// Input from vertex shader
in VS_OUT
{
    vec3 N;
    vec3 L[4];
    vec3 V;
} fs_in;

in vec2 UV;     //x

// Material properties
uniform vec3 mat_ambient;   //
uniform vec3 mat_diffuse;   //
uniform float mat_power;    //
uniform vec3 mat_specular;  //
uniform bool hasTexture;

uniform sampler2D texsampler;   //
uniform vec3 testColor;


void main()
{

     // Normalize the incoming N, L and V vectors
    vec3 N = normalize(fs_in.N);
    
    vec3 V = normalize(fs_in.V);

    // Calculate R locally
  

    // Compute the diffuse and specular components for each fragment
    //vec3 diffuse = max(dot(N, L), 0.0) * mat_diffuse;

    //texture texture texture
    if(hasTexture){
   
  
 
 

    vec3 L[4];
    vec3 R[4];
    vec3 diffuse;
    vec3 specular;
    for(int i = 0; i < 4; i++){
        L[i] = normalize(fs_in.L[i]);
        R[i] = reflect(-L[i], N);
        diffuse += max(dot(N, (0.35 * L[i])), 0.0) * texture2D(texsampler, UV).rgb;
        specular += pow(max(dot(R[i], V), 0.0), mat_power) * mat_specular;
    }
    // Write final color to the framebuffer
    gl_FragColor = vec4(mat_ambient + diffuse + specular, 1.0);
    //gl_FragColor = vec4(mat_ambient + diffuse, 1.0);
  
    }else{
       gl_FragColor = vec4(testColor,1.0);
    }

   




}