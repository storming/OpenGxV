#ifndef __GV_STAGE_H__
#define __GV_STAGE_H__

/**
 * @file 
 */
#include "gv_displayobjectcontainer.h"
#include "gv_singleton.h"
#include "glfw3.h"

GV_NS_BEGIN

/** 
 * @file
 * @brief The StageScaleMode class provides values for the 
 *        Stage.scaleMode property.
 */
enum class StageScaleMode {
    /**
     * @brief The entire application is visible in the specified 
     *        area without trying to preserve the original aspect
     *        ratio. Distortion can occur, and the application may
     *        appear stretched or compressed.
     */
    EXACT_FIT,
    /**
     * @brief The entire application is visible in the specified 
     *        area without distortion while maintaining the original
     *        aspect ratio of the application. Borders can appear on
     *        two sides of the application.
     */
    SHOW_ALL,
    /**
     * @brief The entire application fills the specified area, 
     *        without distortion but possibly with some cropping,
     *        while maintaining the original aspect ratio of the
     *        application.
     */
    NO_BORDER,
    /**
     * @brief The entire application is fixed, so that it remains 
     *        unchanged even as the size of the player window
     *        changes. Cropping might occur if the player window is
     *        smaller than the content.
     */
    NO_SCALE,
};

class Stage final : public DisplayObjectContainer, public singleton<Stage> {
    friend class Object;
public:
    void run() noexcept;
    StageScaleMode scaleMode() const noexcept;
    void scaleMode(StageScaleMode value) noexcept;
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
