#include "opengv.h"
#include "gv_stage.h"
#include "gv_log.h"

GV_NS_BEGIN

Stage::Stage() noexcept {
}

void Stage::glfwError(int id, const char* msg) {
    gv_error("glfw: errno='%d', %s.", id, msg);
}

void Stage::glfwWindowPos(GLFWwindow *windows, int x, int y) {

}

void Stage::glfwWindowSize(GLFWwindow *windows, int width, int height) {

}

void Stage::glfwFramebufferSize(GLFWwindow *windows, int width, int height) {
    glViewport(0, 0, width, height);
}

void Stage::glfwWindowClose(GLFWwindow *windows) {
    instance()->_exit = true;
}

bool Stage::init() {
    glfwSetErrorCallback(glfwError);
    glfwInit();
    /*
    GLenum n = glewInit();
    if (n != GLEW_OK) {
        gv_error("glew init failed, %s.", (char *)glewGetErrorString(n));
        return false;
    }*/

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    Env *env = Env::instance();
    int width = env->stageWidth, height = env->stageHeight;

    _monitor = glfwGetPrimaryMonitor();
    

    _window = glfwCreateWindow(env->stageWidth, env->stageHeight, Env::instance()->title.c_str(), nullptr, nullptr);
    if (!_window) {
        return nullptr;
    }
    glfwMakeContextCurrent(_window);

    glfwSetWindowPosCallback(_window, glfwWindowPos);
    glfwSetFramebufferSizeCallback(_window, glfwFramebufferSize);
    glfwSetWindowSizeCallback(_window, glfwWindowSize);
    glfwSetWindowCloseCallback(_window, glfwWindowClose);

    glfwGetFramebufferSize(_window, &width, &height);
    glViewport(0, 0, width, height);

    glfwGetFramebufferSize(_window, &width, &height);
    float ratio = width / (float)height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    return true;
}

void Stage::run() noexcept {
    _exit = false;
    while (!_exit) {
        float ratio;
        int width, height;
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef((float)glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(-0.6f, -0.4f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.6f, -0.4f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.6f, 0.f);
        glEnd();

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}

Box2f Stage::contentBounds() {
    return Box2f::Zero();
}

GV_NS_END
