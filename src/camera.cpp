#include "camera.h"
#include <glm/ext.hpp>

Camera::Camera(int vh, int vw)
    : pos{glm::vec3(0, 0, 25.0)}, target_pos{glm::vec3(0, 0, 0)}, angle{glm::vec3(0, 0, 0)}, view_mat{glm::mat4x4()} {
  view_mat = get_view_mat();
  set_viewport(vw, vh);
}

glm::vec3 Camera::right_vector() { return glm::transpose(view_mat)[0]; }
glm::vec3 Camera::up_vector() { return glm::vec3(0.0, 1.0, 0.0); }

glm::mat4x4 Camera::get_view_mat() { return glm::lookAt(pos, target_pos, up_vector()); }

glm::vec3 Camera::view_dir() { return -glm::transpose(view_mat)[2]; }

void Camera::set_viewport(int width, int height) {
  viewport_width = (float)width;
  viewport_height = (float)height;
}

// From example here: https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
void Camera::update_view_mat(float dx, float dy) {
  // step 1 : Calculate the amount of rotation given the mouse movement.
  dx = dx * (2 * M_PI / viewport_width); // a movement from left to right = 2*PI = 360 deg
  dy = dy * (M_PI / viewport_height);    // a movement from top to bottom = PI = 180 deg
  glm::vec4 pos4(pos.x, pos.y, pos.z, 1.0);
  glm::vec4 target_pos4(target_pos.x, target_pos.y, target_pos.z, 1.0);

  // Extra step to handle the problem when the camera direction is the same as the up vector
  float cosAngle = glm::dot(view_dir(), up_vector());
  if (cosAngle * ((dy > 0) ? 1 : ((dy < 0) ? -1 : 0)) > 0.99f)
    dy = 0;

  // step 2: Rotate the camera around the pivot point on the first axis.
  glm::mat4x4 rotationMatrixX(1.0f);
  rotationMatrixX = glm::rotate(rotationMatrixX, dx, up_vector());
  pos4 = (rotationMatrixX * (pos4 - target_pos4)) + target_pos4;
  glm::mat4x4 rotationMatrixY(1.0f);

  // step 3: Rotate the camera around the pivot point on the second axis.
  rotationMatrixY = glm::rotate(rotationMatrixY, dy, right_vector());
  glm::vec4 final_pos4 = (rotationMatrixY * (pos4 - target_pos4)) + target_pos4;
  pos = glm::vec3(final_pos4[0], final_pos4[1], final_pos4[2]);

  view_mat = glm::lookAt(pos, target_pos, up_vector());
}
