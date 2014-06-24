#ifndef __GV_STAGE_H__
#define __GV_STAGE_H__

#include "gv_displayobjectcontainer.h"
#include "gv_singleton.h"
#include "gv_env.h"
#include "glfw3.h"

GV_NS_BEGIN

enum class StageScaleMode {
    EXACT_FIT,
    SHOW_ALL,
    NO_BORDER,
    NO_SCALE,
};

class Stage final : public DisplayObjectContainer, public singleton<Stage> {
    GV_FRIEND_PTR();
    GV_FRIEND_SINGLETON();

public:
    void run() noexcept;

private:
    static void glfwError(int id, const char* msg);
    static void glfwWindowPos(GLFWwindow *windows, int x, int y);
    static void glfwWindowSize(GLFWwindow *windows, int width, int height);
    static void glfwFramebufferSize(GLFWwindow *windows, int width, int height);
    static void glfwWindowClose(GLFWwindow *windows);
    Stage() noexcept;
    bool init();
    virtual Box2f contentBounds() override;

private:
    GLFWwindow* _window;
    GLFWmonitor* _monitor;
    bool _exit;
};

GV_NS_END

#endif
