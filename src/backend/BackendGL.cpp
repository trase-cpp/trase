/*
Copyright (c) 2018, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of trase.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "BackendGL.hpp"

// Needs to go in the cpp file (should only be included once)
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#include "imgui_impl_glfw_gl3.h"

namespace trase {
static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Error %d: %s\n", error, description);
}

void BackendGL::init(int x_pixels, int y_pixels, const char *name) {
  m_window = create_window(x_pixels, y_pixels, name);
  if (!m_window)
    throw Exception("trase: Cannot create OpenGL window");
  init_imgui(m_window);
  m_vg = init_nanovg(x_pixels, y_pixels);
}

void BackendGL::finalise() {
  // Cleanup
  ImGui_ImplGlfwGL3_Shutdown();
  ImGui::DestroyContext();

  nvgDeleteGL3(m_vg);

  glfwDestroyWindow(m_window);
  glfwTerminate();
}

vfloat2_t BackendGL::begin_frame() {
  glfwPollEvents();
  ImGui_ImplGlfwGL3_NewFrame();

  int winWidth, winHeight;
  int fbWidth, fbHeight;

  // glfwGetCursorPos(window, &mx, &my);
  glfwGetWindowSize(m_window, &winWidth, &winHeight);
  glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
  // Calculate pixel ration for hi-dpi devices.
  float pxRatio = (float)fbWidth / (float)winWidth;

  glViewport(0, 0, fbWidth, fbHeight);
  // glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  // glClear(GL_COLOR_BUFFER_BIT);

  nvgBeginFrame(m_vg, winWidth, winHeight, pxRatio);

  // nvgResetTransform(m_vg);
  // nvgScale(m_vg, winWidth / 100.0, winHeight / 100.0);
  return vfloat2_t(winWidth, winHeight);
}

void BackendGL::end_frame() {
  nvgEndFrame(m_vg);
  ImGui::Render();
  ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
  // Rendering
  glfwSwapBuffers(m_window);
}

GLFWwindow *BackendGL::create_window(int x_pixels, int y_pixels,
                                     const char *name) {
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  // glfwSetWindowRefreshCallback(m_handle, glfw_window_refresh_callback);
  if (!glfwInit()) {
    throw Exception("Could not initialise GLFW");
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  GLFWwindow *window = glfwCreateWindow(x_pixels, y_pixels, name, NULL, NULL);

  if (!window)
    throw Exception("Could not create GLFW window");
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync
  // gl3wInit();
  // gladLoadGL();
  if (!gladLoadGL()) {
    throw Exception("Could not load GL extensions");
  }
  std::cout << "OpenGL Version " << GLVersion.major << '.' << GLVersion.minor
            << std::endl;
  return window;
}

NVGcontext *BackendGL::init_nanovg(int x_pixels, int y_pixels) {
  NVGcontext *vg =
      nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
  if (!vg)
    throw Exception("trase: Cannot init NanoVG");
  auto filename = m_fm.find_font("Roboto-Regular", "");
  int fontNormal = nvgCreateFont(vg, "Roboto", filename.c_str());
  if (fontNormal == -1) {
    throw Exception("Could not add font Roboto");
  }
  filename = m_fm.find_font("Roboto-Bold", "");
  int fontBold = nvgCreateFont(vg, "Roboto-Bold", filename.c_str());
  if (fontBold == -1) {
    throw Exception("Could not add font bold");
  }
  filename = m_fm.find_font("NotoEmoji-Regular", "");
  int fontEmoji = nvgCreateFont(vg, "NotoEmoji-Regular", filename.c_str());
  if (fontEmoji == -1) {
    throw Exception("Could not add font emoji");
  }
  nvgAddFallbackFontId(vg, fontNormal, fontEmoji);
  nvgAddFallbackFontId(vg, fontBold, fontEmoji);
  return vg;
}

void BackendGL::init_imgui(GLFWwindow *window) {
  // Setup Dear ImGui binding
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard
  // Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable
  // Gamepad Controls
  ImGui_ImplGlfwGL3_Init(window, true);

  // Setup style
  ImGui::StyleColorsDark();
}

} // namespace trase
