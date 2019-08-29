// Como escrever um arquivo OBJ
// A ordem nao importa
// o obejeto que o arquivo obj esta descrevendo é um quadrado com 4 vertices e 2 faces
// v 0 1 0
// v 1 1 0
// v 0 0 0 
// v 1 0 0
// vn 0 0 1 ---> este e a normal do objeto, como é a mesma normal para todos os vertices, so precisamos de uma normal
// f 4//1 2//1 1//1  ---> posicao em que o vertice aparece no arquivo//normal desse vertice
// f 3//1 4//1 1//1

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// Global variables
bool BACKGROUND_STATE = false;
float angle=0;

glm::mat4 MODEL(1.0f);
glm::mat4 PROJECTION(1.0f);


struct PointLight{
	glm::vec3 position;
	glm::vec3 color;
} light;

struct DiffuseMaterial{
	glm::vec3 color;
} diffuse;

// Load triangle mesh from Wavefront OBJ file
// TODO: 10 pts (first test)
bool readTriangleMesh(const std::string & filename,
	std::vector < glm::vec3> & positions,
	std::vector < glm::vec3> & normals,
	std::vector < glm::vec2> & textureCoordinates,
	std::vector < size_t> & positionIndices,
	std::vector < size_t> & normalsIndices,
	std::vector < size_t> & textureCoordinatesIndices) 
	{
		std::ifstream file (filename, std::ifstream::in);
		
		//abrindo o arquivo
		if(!file.is_open())
			return false;
		else 
		{
			std::string line;
			
			//passa linha por linha do arquivo
			while (std::getline (file, line)) 
			{
				//divide a linha separando pelo espaco
				//e pega a primeira "palavra" da linha e coloca na variavel type
				std::istringstream atribute (line);
				std::string type;
				atribute >> type;
				
				if(type == "v") //verifica se e um vertice
				{
					glm::vec3 p;
					atribute >> p.x >> p.y >> p.z;
					positions.push_back(p);
				}
				else if (type == "vn") //verifica se é uma normal
				{
					glm::vec3 n;
					atribute >> n.x >> n.y >> n.z;
					normals.push_back(n);
				}
				else if (type == "vt") 
				{
					glm::vec2 t;
					atribute >> t.x >> t.y;
					textureCoordinates.push_back(t);
				}
				else if (type == "f") //verifica se e uma face
				{
					for(int i=0; i<3; i++) 
					{
						std::string indices;
						atribute >> indices;
						
						//troca a / por espaco
						std::replace(indices.begin(), indices.end(), '/', ' ');
												
						size_t vi;
						std::istringstream ind (indices);
						ind >> vi;
						positionIndices.push_back(vi-1);
						
						if(ind.peek() == ' ')
						{
							ind.ignore();
							
							if(ind.peek() == ' ')
							{
								ind.ignore();	
								size_t vn;
								ind >> vn;
								normalsIndices.push_back(vn-1);	
							}
							else 
							{
								size_t vt;
								ind>>vt;
								textureCoordinatesIndices.push_back(vt-1);
							
								if(ind.peek() == ' ')
								{
									ind.ignore();
									size_t vn;
									ind >> vn;
									normalsIndices.push_back(vn-1);
								}		
							}
						}
						 
					}
				}
			
			}
			return true;
		}	
		
	}


//axercicio extra valendo 10 pontos na prova	
size_t loadTriangleMesh(
        const std::vector<glm::vec3> & positions,
        const std::vector<glm::vec3> & normals,
        const std::vector<glm::vec2> & textureCoordinates,
        const std::vector<size_t> & positionIndices,
        const std::vector<size_t> & normalIndices,
        const std::vector<size_t> & textureCoordinateIndices,
        GLenum usage,
        GLuint & vao,
        GLuint & vbo) {
    struct Vertex {
    	glm::vec3 position;
    	glm::vec3 normal;
    	glm::vec2 textureCoordinate;
	};
    bool hasNormals = normalIndices.size() > 0;
    bool hasTextureCoordinates = textureCoordinateIndices.size() > 0;
    
    std::vector<Vertex> vertices;
    
    for(size_t i= 0; i < positionIndices.size() / 3; i++) {
    	Vertex triangleVertices[3];
    	
    	for(size_t j = 0; j < 3; j++) {
    		Vertex & vertex = triangleVertices[j];
    		vertex.position = positions[positionIndices[i * 3 + j]];
		}
		
		
		if(hasNormals) {
			for(size_t j = 0; j < 3; j++) {
    		Vertex & vertex = triangleVertices[j];
    		vertex.normal = normals[normalIndices[i * 3 + j]];
			}
		}
		else {
			Vertex & vertex0 = triangleVertices[0];
			Vertex & vertex1 = triangleVertices[1];
			Vertex & vertex2 = triangleVertices[2];
		
			glm::vec3 u = vertex1.position - vertex0.position;
			glm::vec3 v = vertex1.position - vertex0.position;
		
			glm::vec3 n = glm::normalize(glm::cross(u,v));
		
			vertex0.normal = n;
			vertex1.normal = n;
			vertex2.normal = n;
		
		}	
		if(hasTextureCoordinates){
			for(size_t j = 0; j < 3; j++) {
    			Vertex & vertex = triangleVertices[j];
    			vertex.textureCoordinate = textureCoordinates[textureCoordinateIndices[i * 3 + j]];
			}	
		}
		else {
			Vertex & vertex0 = triangleVertices[0];
			Vertex & vertex1 = triangleVertices[1];
			Vertex & vertex2 = triangleVertices[2];
		
			vertex0.textureCoordinate = glm::vec2(0.0f, 0.0f);
			vertex1.textureCoordinate = glm::vec2(1.0f, 0.0f);
			vertex2.textureCoordinate = glm::vec2(0.0f, 1.0f);
		
		}
		for(size_t i =0; i < 3; i++){
			vertices.push_back(triangleVertices[i]);
		}
	}
	 
    // Create and bind vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Create and bind vertex buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // Copy vertex attribute data to vertex buffer object
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        usage);
    
    // Define position attribute to shader program
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        false,
        sizeof(Vertex),
        (const GLvoid *)nullptr);
    
    // Enable position attribute to shader program
    glEnableVertexAttribArray(0);
    
    // Define normal attribute to shader program
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        false,
        sizeof(Vertex),
        (const GLvoid *)(3 * sizeof(GLfloat)));
    
    // Enable color attribute to shader program
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		false,
		8 * sizeof(GLfloat),
		(const GLvoid *)(6 * sizeof(GLfloat)));
    
    // Return vertex count or three (6 - two triangles) times the triangle count
    glEnableVertexAttribArray(2);
    
    return positionIndices.size();
}
	
// Compile shader source code from text file format
bool compileShader(const std::string & filename, GLenum type, GLuint & id) {
    // Read from text file to string
    std::ifstream file(filename, std::ifstream::in);

    if (!file.is_open())
        return false;

    std::stringstream buffer;
    std::string source;

    buffer << file.rdbuf();
    source = buffer.str();

    // Create shader
    GLuint shaderID = glCreateShader(type);

    // Setup shader source code
    const GLchar * src = source.data();
    glShaderSource(shaderID, 1, &src, nullptr);

    // Compile shader
    glCompileShader(shaderID);

    // Get compilation status
    GLint status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);

    // Check compilation errors
    if (status != GL_TRUE) {
        // Get log message size of the compilation process
        GLint size = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &size);

        std::string message;
        message.resize(size);

        // Get log message of the compilation process
        glGetShaderInfoLog(shaderID, size, nullptr, (GLchar *)message.data());

        // Print log message
        std::cout << message << std::endl;

        // Delete shader
        glDeleteShader(shaderID);

        return false;
    }

    // Return shader id
    id = shaderID;

    return true;
}

// Create shader program
bool createProgram(const std::string & name, GLuint & id) {
    GLuint vertexShaderID, fragmentShaderID;

    // Load and compile vertex shader
    if (!compileShader(name + ".vert", GL_VERTEX_SHADER, vertexShaderID))
        return false;

    // Load and compile fragment shader
    if (!compileShader(name + ".frag", GL_FRAGMENT_SHADER, fragmentShaderID))
        return false;

    // Create shader program
    GLuint programID = glCreateProgram();

    // Attach compiled shaders to program
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);

    // Link attached shaders to create an executable
    glLinkProgram(programID);

    // Delete compiled shaders
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    // Get linkage status
    GLint status;
    glGetProgramiv(programID, GL_LINK_STATUS, &status);

    // Check linkage errors
    if (status != GL_TRUE) {
        // Get log message size of the linkage process
        GLint size = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &size);

        std::string message;
        message.resize(size);

        // Get log message of the linkage process
        glGetProgramInfoLog(programID, size, nullptr, (GLchar *)message.data());

        // Print log message
        std::cout << message << std::endl;

        // Delete shader program
        glDeleteProgram(programID);

        return false;
    }

    // Return shader program id
    id = programID;

    return true;
}

// Resize event callback
void resize(GLFWwindow * window, int width, int height) {
	glViewport(0, 0, width, height);
	PROJECTION = glm::perspective(45.0f, width / (float)height, 0.001f, 1000.0f);
}

// Keyboard event callback
// reponsavel por saber se o a foi apertado
void keyboard(
		GLFWwindow * window,
		int key, int scancode, int action, int modifier) {
	// verifica se o a foi apertado, e se for inverte a respota
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		BACKGROUND_STATE = !BACKGROUND_STATE;
		
	if(key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		MODEL = glm::rotate(MODEL, 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));	
		
	if(key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		MODEL = glm::rotate(MODEL, 0.1f, glm::vec3(0.0f, -1.0f, 0.0f));
		
	if(key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
		MODEL = glm::rotate(MODEL, 0.1f, glm::vec3(-1.0f, 0.0f, 0.0f));
		
	if(key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
		MODEL = glm::rotate(MODEL, 0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
}

void cursor(GLFWwindow * window, double x, double y) {
	MODEL = glm::translate(glm::mat4(1.0f), glm::vec3((float)(x * 0.01f), (float)(y * -0.01f), 0.0f));
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	angle = (angle-xpos)/1000; 
	
	MODEL = glm::rotate(MODEL, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	
	angle = xpos;
}

int main(int argc, char ** argv) {
    // GLM usage
    //
    // glm::vec3 u(1.0f, 0, 0);
    // glm::vec3 v(0, 1.0f, 0);
    //
    // glm::vec3 d = glm::dot(u, v);
    //
    // glm::mat3 m(0.0f, 1.0f, 2.0f,  // First column
    //			   3.0f, 4.0f, 5.0f,  // Second column
    //			   6.0f, 7.0f, 8.0f); // Third column
    //
    // Print GLM matrix
    // std::cout << glm::to_string(m) << std::endl;
    //
    // Access the first column as GLM vector
    // std::cout << glm::to_string(m[0]) << std::endl;
    //
    // Access the second element of the first column as float
    // std::cout << m[0][1] << std::endl;

    // Access vertex position attributes of the first triangle
    //
    // std::vector<glm::vec3> positions;
    // std::vector<glm::vec3> normals;
    // std::vector<glm::vec2> textureCoordinates;
    // std::vector<size_t> positionIndices;
    // std::vector<size_t> normalIndices;
    // std::vector<size_t> textureCoordinateIndices;

    // if (loadTriangleMesh(
    //         "../res/meshes/bunny.obj",
    //         positions, normals, textureCoordinates,
    //         positionIndices, normalIndices, textureCoordinateIndices)) {
    //	   size_t triangleIndex = 0;
    //	   
    //	   glm::vec3 p0 = positions[positionIndices[triangleIndex * 3]];
    //	   glm::vec3 p1 = positions[positionIndices[triangleIndex * 3 + 1]];
    //	   glm::vec3 p2 = positions[positionIndices[triangleIndex * 3 + 2]];
    //	   
    //	   std::cout << "First triangle vertices: " << std::endl;
    //	   
    //	   std::cout << glm::to_string(p0) << std::endl;
    //	   std::cout << glm::to_string(p1) << std::endl;
    //     std::cout << glm::to_string(p2) << std::endl;
    // }

    // Check GLFW initialization
    if (!glfwInit()) {
        std::cout << "Cannot initialize GLFW." << std::endl;
        return -1;
    }

    // Setup OpenGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 16);

    // Create window
    GLFWwindow * window = glfwCreateWindow(800, 600, "Window", nullptr, nullptr);

    // Check if cannot create window
    if (window == nullptr) {
        glfwTerminate();

        std::cout << "Cannot create window." << std::endl;
        return -1;
    }

    // Register event callbacks
    //esses sao as chamadas do calbecks, o redimencionameto do tela e o teclado
    glfwSetFramebufferSizeCallback(window, resize);
    glfwSetKeyCallback(window, keyboard);
    //glfwSetCursorPosCallback(window, cursor);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Setup window context
    glfwMakeContextCurrent(window);

    // Check if cannot load OpenGL procedures
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();

        std::cout << "Cannot load OpenGL procedures." << std::endl;
        return -1;
    }
    
    // Shader program ID
    GLuint programID;
    
    // Check if cannot create shader program
    if (!createProgram("../res/shaders/diffuse", programID)) {
        glfwTerminate();

        std::cout << "Cannot create shader program." << std::endl;
        return -1;
    }
    
    // Use shader program
    glUseProgram(programID);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    
    // Read triangle mesh from Wavefront OBJ file format
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoordinates;
    std::vector<size_t> positionIndices;
    std::vector<size_t> normalIndices;
    std::vector<size_t> textureCoordinateIndices;
    
    readTriangleMesh(
        "../res/meshes/dragon.obj",
        positions,
        normals,
        textureCoordinates,
        positionIndices,
        normalIndices,
        textureCoordinateIndices);
    
    // Load triangle mesh to OpenGL
    GLuint vao, vbo;
    
    size_t vertexCount = loadTriangleMesh(
        positions,
        normals,
        textureCoordinates,
        positionIndices,
        normalIndices,
        textureCoordinateIndices,
        GL_STATIC_DRAW,
        vao,
        vbo);
    
    // Setup view matrix
    // camera, ajustando o z, podemos controlar a distancia da camera
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 30.0f),
        glm::vec3(0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Initialize projection matrix and viewport
    resize(window, 800, 600);
    
    //colocar luz
    
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Setup color buffer
        if (BACKGROUND_STATE)
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        else
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            
        // Clear color buffer
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Setup depth buffer
        glClearDepth(1.0f);
        
        // Clear depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);
        
        // Pass model matrix as parameter to shader program
        GLint modelLocationID = glGetUniformLocation(programID, "model");
        glUniformMatrix4fv(modelLocationID, 1, GL_FALSE, glm::value_ptr(MODEL));
        
        // Pass view matrix as parameter to shader program
        GLint viewLocationID = glGetUniformLocation(programID, "view");
        glUniformMatrix4fv(viewLocationID, 1, GL_FALSE, glm::value_ptr(view));
        
        // Pass projection matrix as parameter to shader program
        GLint projectionLocationID = glGetUniformLocation(programID, "projection");
        glUniformMatrix4fv(projectionLocationID, 1, GL_FALSE, glm::value_ptr(PROJECTION));
        
        //colocar a luz de fundo
        
        // Draw vertex array as triangles
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        
        // Swap double buffer
        glfwSwapBuffers(window);

        // Process events and callbacks
        glfwPollEvents();
    }

    // Delete shader program
    glDeleteProgram(programID);

    // Delete vertex array object
    glDeleteVertexArrays(1, &vao);

    // Delete vertex buffer object
    glDeleteBuffers(1, &vbo);

    // Destroy window
    glfwDestroyWindow(window);

    // Deinitialize GLFW
    glfwTerminate();

    return 0;
}
