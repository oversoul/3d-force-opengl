#include <fstream>
#include <iostream>

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <ctime>
#include <functional>
#include <stdlib.h>

#include <jsoncpp/json/json.h>

#include "camera.h"
#include "layout.h"
#include "line.h"
#include "solid_sphere.h"
#include "springy.h"
#include "websocket.h"

#define WIN_WIDTH 1024
#define WIN_HEIGHT 1024

float yLocation = 0.0f; // Keep track of our position on the y axis.

Graph graph;
WebSocketServer server;

float damping = 0.1;
float stiffness = 200.0;
float repulsion = 4000.0;
float minEnergyThreshold = 0.00001;

Camera camera(WIN_WIDTH, WIN_HEIGHT);
ForceDirected *layout = new ForceDirected(&graph, stiffness, repulsion, damping, minEnergyThreshold);

float mouseX = 0;
float mouseY = 0;
float drag_sensitivity = 2.5;
float scroll_sensitivity = 1.0f;

bool mouseDown = false;
bool autoRotateX = false;
bool mouseDownFirst = false;

float MIN_DISTANCE_ORIGIN = 5.0f;

glm::mat4 view;
glm::mat4 projection;

SolidSphere sphere(0.4f, 12, 24);
Line line(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));

// void jsonLoader(Graph *graph, const char *file) {
//   Json::Value jsonValue;
//   std::ifstream sourceFile(file, std::ifstream::binary);
//   if (!sourceFile.is_open()) {
//     std::cerr << "Could not open the file!" << std::endl;
//     exit(1);
//   }
//
//   sourceFile >> jsonValue;
//
//   std::vector<Node *> nodes = {};
//   std::map<std::string, int> idToIndex = {};
//
//   int index = 0;
//   for (auto val : jsonValue["nodes"]) {
//     idToIndex[val["id"].asString()] = index;
//     auto n = graph->newNode();
//     nodes.push_back(n);
//     ++index;
//   }
//
//   for (auto link : jsonValue["links"]) {
//     auto sId = idToIndex[link["source"].asString()];
//     auto tId = idToIndex[link["target"].asString()];
//     graph->newEdge(nodes[sId], nodes[tId]);
//   }
// }
//
void draw_graph(float yloc) {
  (void)yloc;

  for (auto e : graph.edges) {
    auto sp = layout->point(e->source);
    auto tp = layout->point(e->target);

    line.setVertices(                            //
        glm::vec3(sp->p->x, sp->p->y, sp->p->z), //
        glm::vec3(tp->p->x, tp->p->y, tp->p->z)  //
    );

    line.setMVP(projection * view);
    line.draw(&camera);
  }

  for (auto n : graph.nodes) {
    auto p = layout->point(n);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(p->p->x, p->p->y, p->p->z));

    // float radius = 1.0f;
    // glm::vec3 scale = glm::vec3(radius, radius, radius);
    // model = glm::scale(model, scale);

    auto c = n->data.color;
    sphere.setColor({c.r, c.g, c.b});

    sphere.setMVP(model, view, projection);
    sphere.draw(&camera);
  }
}

void render() {

  // Clear the background of our window to red
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

  // Clear the colour buffer (more buffers later on)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  view = camera.get_view_mat();

  draw_graph(yLocation);
}

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %d = %s\n", error, description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  (void)mods;
  (void)scancode;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    server.stop();
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  if (key == GLFW_KEY_W) {
    camera.update_view_mat(0, 0.8);
  }
  if (key == GLFW_KEY_S) {
    camera.update_view_mat(0, -0.8);
  }
  if (key == GLFW_KEY_A) {
    camera.update_view_mat(0.8, 0);
  }
  if (key == GLFW_KEY_D) {
    camera.update_view_mat(-0.8, 0);
  }
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    autoRotateX = !autoRotateX;
  }
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
  (void)window;

  // If first time mouse down, reset the mouse positions
  // This prevents the camera from jerking when initially rotating
  if (mouseDownFirst) {
    std::cout << "Mouse clicked" << std::endl;
    mouseX = xpos;
    mouseY = ypos;
    mouseDownFirst = false;
  }
  // Rotate the camera around the origin
  // Only rotate if user is holding down mouse click
  if (mouseDown) {
    float dx = -(xpos - mouseX) * drag_sensitivity; // Negate dx
    float dy = -(ypos - mouseY) * drag_sensitivity; // Negate dy
    // double dx = (xpos - mouseX) * drag_sensitivity;
    // double dy = (ypos - mouseY) * drag_sensitivity;
    mouseX = xpos;
    mouseY = ypos;
    camera.update_view_mat(dx, dy);
  }
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  (void)window;
  (void)xoffset;

  // yoffset is -1 or 1
  std::cout << "Scroll clicked" << std::endl;
  // If zooming in, make sure not to zoom past the origin
  if (glm::distance(camera.pos, glm::vec3(0.0f)) > MIN_DISTANCE_ORIGIN) {
    camera.pos += (float)yoffset * scroll_sensitivity * camera.view_dir();
    // Push the camera back if it is zoomed in too far
  } else {
    camera.pos -= camera.view_dir() * scroll_sensitivity;
  }
}

void mouse_button_callback(GLFWwindow *w, int btn, int action, int mods) {
  (void)w;
  (void)mods;

  if (btn == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    mouseDown = true;
  mouseDownFirst = true;
  if (btn == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    mouseDown = false;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  (void)window;

  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
  camera.set_viewport(width, height);

  float aspectRatio = (float)width / (float)height;
  projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 500.0f);
}

std::function<void()> loop;
void main_loop() { loop(); }

int main(void) {

  std::thread server_thread(&WebSocketServer::run, &server, 9002);

  std::cout << "WebSocket server running on thread. Press Enter to stop." << std::endl;

  std::cout << "Starting visu..." << std::endl;
  srand(time(NULL));

  std::cout << "Initiating graph..." << std::endl;

  GLFWwindow *window;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    exit(EXIT_FAILURE);

  window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Simple example", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  if (GLEW_OK != glewInit()) {
    std::cout << "GLEW INIT FAILED, EXITING" << std::endl;
    exit(EXIT_FAILURE);
  }

  // gladLoadGL(glfwGetProcAddress);
  glfwSwapInterval(1);
  // glfwSwapInterval(0); // 0 = unlimited

  glfwWindowHint(GLFW_SAMPLES, 4); // 4 samples for MSAA
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

  /* Set rendering settings */
  projection = glm::perspective(
      // FOV & aspect
      glm::radians(60.0f), (GLfloat)WIN_WIDTH / (GLfloat)WIN_HEIGHT,
      // Near and far planes
      0.1f, 500.0f);

  sphere.init();
  line.init();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_LINE_STRIP_ADJACENCY);

  // INIT
  // jsonLoader(&graph, "data/structure.json");
  // jsonLoader(&graph, "data/miserables.json");

  server.set_message_handler([](websocketpp::connection_hdl, socketServer::message_ptr msg) {
    auto val = msg->get_payload();

    std::string s = val;
    std::string delimiter = " ";
    std::string command = s.substr(0, s.find(delimiter));

    if (command == "add") {
      s.erase(0, s.find(delimiter) + delimiter.length());
      std::string type = s.substr(0, s.find(delimiter));
      if (type == "node") {
        s.erase(0, s.find(delimiter) + delimiter.length());

        graph.addNodes({s});
      }
    } else if (command == "connect") {
      s.erase(0, s.find(delimiter) + delimiter.length());
      std::string n1 = s.substr(0, s.find(delimiter));
      s.erase(0, s.find(delimiter) + delimiter.length());
      std::string n2 = s;
      if (n1 == n2) {
        printf("Can't add a connection between the same nodes.\n");
        return;
      }
      graph.addEdges({{n1, n2}});
    }
  });

  /*
  auto dennis = graph.newNode();
  dennis->data.color.r = 0;
  dennis->data.color.b = 255;

  auto michael = graph.newNode();
  michael->data.color.r = 0;
  michael->data.color.g = 255;

  auto jessica = graph.newNode();
  auto timothy = graph.newNode();
  auto barbara = graph.newNode();
  auto franklin = graph.newNode();
  auto monty = graph.newNode();
  auto james = graph.newNode();
  auto bianca = graph.newNode();

  graph.newEdge(dennis, michael);
  graph.newEdge(michael, dennis);
  graph.newEdge(michael, jessica);
  graph.newEdge(jessica, barbara);
  graph.newEdge(michael, timothy);
  graph.newEdge(franklin, monty);
  graph.newEdge(dennis, monty);
  graph.newEdge(monty, james);
  graph.newEdge(barbara, timothy);
  graph.newEdge(dennis, bianca);
  graph.newEdge(bianca, monty);
  */

  /*
  graph.addNodes({"Dennis", "Michael", "Jessica", "Timothy", "Barbara"});
  graph.addNodes({"Amphitryon", "Alcmene", "Iphicles", "Heracles"});

  graph.addEdges({
      {"Dennis", "Michael"},
      {"Michael", "Dennis"},
      {"Michael", "Jessica"},
      {"Jessica", "Barbara"},
      {"Michael", "Timothy"},
      {"Amphitryon", "Alcmene"},
      {"Alcmene", "Amphitryon"},
      {"Amphitryon", "Iphicles"},
      {"Amphitryon", "Heracles"},
      {"Barbara", "Timothy"},
  });
  */

  int frames = 0;
  double t0 = glfwGetTime();

  // Render loop
  loop = [&] {
    layout->start();

    if (autoRotateX) {
      camera.update_view_mat(0.5, 0);
    }

    render();

    double t = glfwGetTime();
    if ((t - t0) > 1.0 || frames == 0) {
      double fps = (double)frames / (t - t0);
      printf("FPS: %.1f\n", fps);
      t0 = t;
      frames = 0;
    }

    frames++;

    glfwSwapBuffers(window);
    glfwPollEvents();
  };

  while (!glfwWindowShouldClose(window))
    main_loop();

  server.stop();
  server_thread.join();
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
