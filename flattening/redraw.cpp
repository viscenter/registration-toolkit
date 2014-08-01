#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <vector>

const GLchar* vertexSource =
  "#version 150\n"
  "in vec2 position;"
  "in vec2 texcoord;"
  "out vec3 Color;"
  "out vec2 Texcoord;"
  "void main() {"
  "   gl_Position = vec4(position, 0.0, 1.0);"
  "   Texcoord = texcoord;"
  "}";
const GLchar* fragmentSource =
  "#version 150\n"
  "in vec2 Texcoord;"
  "out vec4 outColor;"
  "uniform sampler2D red;"
  "void main() {"
  // "   outColor = texture(red, Texcoord);"
  "   outColor = vec4(red.r, 0, 0, 1);"
  "}";

// Create a texture from an image file
// Since the images being used might be larger than the cooperative texture size,
// read them in one quadrant at a time
// +---+---+
// | 2 | 1 |
// +---+---+
// | 3 | 4 |
// +---+---+
void loadTextures(const GLchar* path, GLuint& rt, GLuint& gt, GLuint& bt) {
  cv::Mat image = cv::imread(path);
  if(image.empty()) {
    std::cout << "Texture image empty!" << std::endl;
  } else {
    cv::flip(image, image, 0);

    glGenTextures(1, &rt);
    glGenTextures(1, &gt);
    glGenTextures(1, &bt);

    glBindTexture(GL_TEXTURE_2D, rt);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    cv::Mat rm, gm, bm;
    std::vector<cv::Mat> channels(3);
    cv::split(image, channels);
    bm = channels[0];
    gm = channels[1];
    rm = channels[2];

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_R8, GL_UNSIGNED_BYTE, rm.ptr());
  }
}

void createShaderProgram(const GLchar* vertSrc, const GLchar* fragSrc, GLuint& vertexShader, GLuint& fragmentShader, GLuint& shaderProgram) {
  // Create and compile the vertex shader
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);

  // Check status of vertex shader compilation
  GLint status;
  char buffer[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
  if(status != GL_TRUE) {
    glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
    std::cout << "Vertex shader compilation error:" << std::endl
	      << buffer << std::endl;
    return;
  }

  // Create and compile the fragment shader
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);

  // Check the status of fragment shader compilation
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
  if(status != GL_TRUE) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
    std::cout << "Fragment shader compilation error:" << std::endl
	      << buffer << std::endl;
    return;
  }

  // Link the vertex and fragment shader into a shader program
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);
}

void specifyScreenVertexAttributes(GLuint shaderProgram) {
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  // Read 2 floats for position coordinates, skip 0 first (position is given first)
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

  GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
  glEnableVertexAttribArray(texAttrib);
  // Read 2 floats for texture coordinates, skip 2 floats first (don't read position)
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
}

void glTexToCVmat(cv::Mat& image, int width, int height) {
  // http://stackoverflow.com/questions/9097756/converting-data-from-glreadpixels-to-opencvmat/
  image.create(height, width, CV_8UC3);

  // Use fast 4-byte alignment (default anyway) if possible
  glPixelStorei(GL_PACK_ALIGNMENT, (image.step & 3) ? 1 : 4);

  // Set length of one complete row in destination data (doesn't need to equal image.cols)
  glPixelStorei(GL_PACK_ROW_LENGTH, image.step/image.elemSize());

  // Read pixels into Mat. OpenCV stores colors as BGR rather than RGB
  glReadPixels(0, 0, image.cols, image.rows, GL_BGR, GL_UNSIGNED_BYTE, image.data);
}

void GLinit(GLFWwindow** window, int width, int height) {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

  (*window) = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);

  glfwMakeContextCurrent(*window);

  glewExperimental = GL_TRUE;
  glewInit();
}

int main(int argc, char* argv[]) {
  if(argc < 5) {
    std::cout << "Usage: " << argv[0] << " originalMeshOBJ flattenedMeshOBJ textureImage flattenedImage" << std::endl;
    return EXIT_FAILURE;
  }
  char* originalMeshName = argv[1];
  char* flattenedMeshName = argv[2];
  char* textureImageName = argv[3];
  char* flattenedImageName = argv[4];

  cv::Mat textureImage = cv::imread(textureImageName);
  int width, height;
  width = textureImage.cols;
  height = textureImage.rows;
  textureImage.release();
  // std::cout << "Width: " << width << std::endl
  // 	    << "Height: " << height << std::endl;
  
  GLFWwindow* window;
  GLinit(&window, width, height);

  // int maxSize;
  // const GLubyte* version;
  // glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
  // std::cout << "Max texture size: " << maxSize << std::endl;
  // version = glGetString(GL_VERSION);
  // std::cout << "Version: " << version << std::endl;

  // Create Vertex Array Object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Create a Vertex Buffer Object and copy the vertex data to it
  GLuint vbo;
  glGenBuffers(1, &vbo);

  GLfloat vertices[] = {
    // coord, color, texcoord
    -1.0f,  1.0f, 0.0f, 0.0f, // Top-left
    1.0f,  1.0f, 1.0f, 0.0f, // Top-right
    1.0f, -1.0f, 1.0f, 1.0f, // Bottom-right
    -1.0f, -1.0f, 0.0f, 1.0f  // Bottom-left
  };

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint ebo;
  glGenBuffers(1, &ebo);

  GLuint elements[] = {
    0, 1, 2,
    2, 3, 0
  };

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  GLuint frameBuffer;
  glGenFramebuffers(1, &frameBuffer);

  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

  GLuint texColorBuffer;
  glGenTextures(1, &texColorBuffer);
  glBindTexture(GL_TEXTURE_2D, texColorBuffer);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

  GLuint vertexShader, fragmentShader, shaderProgram;
  createShaderProgram(vertexSource, fragmentSource, vertexShader, fragmentShader, shaderProgram);

  specifyScreenVertexAttributes(shaderProgram);

  cv::Mat img;
  img.create(height, width, CV_8UC3);

    GLuint red, blue, green;
    loadTextures(textureImageName, red, blue, green);

    // Set texture background color
    // float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  

    glTexToCVmat(img, width, height);

    // glDeleteTextures(1, &tex);

  cv::imwrite(flattenedImageName, img);

  // Swap buffers
  glfwSwapBuffers(window);

  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);

  glDeleteBuffers(1, &ebo);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  glDeleteFramebuffers(1, &frameBuffer);

  glfwTerminate();

  return 0;
}
