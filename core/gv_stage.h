#ifndef __GV_STAGE_H__
#define __GV_STAGE_H__

#include "gv_displayobjectcontainer.h"
#include "gv_singleton.h"
#include "gv_layout.h"
#include "gv_driverinfo.h"
#include "gv_renderer.h"

struct GLFWwindow;
struct GLFWmonitor;

GV_NS_BEGIN

class Stage;

enum class NativeWindowDisplayState {
    NORMAL,
    MINIMIZED,
    MAXIMIZED,
};

class NativeWindowBoundsEvent : public Event {
public:
    NativeWindowBoundsEvent(const ptr<UniStr> &type, const Box2f &beforeBounds, const Box2f &afterBounds) noexcept;
    NativeWindowBoundsEvent(const NativeWindowBoundsEvent &x) noexcept;
    virtual ptr<Event> clone() override;

private:
    Box2f _beforeBounds;
    Box2f _afterBounds;
};

class NativeWindow : public EventDispatcher {
    friend class Stage;
    friend class Object;
public:
    bool active() const;
    void activate();

    Box2f bounds() const;
    void bounds(const Box2f &value);

    bool closed() const;
    void close();

    bool maximizable() const;
    bool minimizable() const;

    bool resizable() const;
    void resizable(bool value);

    void maximize();
    void minimize();
    void restore();

    const char *title() const;
    void title(const char *value);

    NativeWindowDisplayState displayState() const;

    Stage *stage() const {
        return _stage;
    }

    bool visible() const;
    void visible(bool value);

    int x() const;
    void x(int value);

    int y() const;
    void y(int value);

    unsigned width() const;
    void width(unsigned value);

    unsigned height() const;
    void height(unsigned height);
private:
    NativeWindow(Stage *stage);
    bool create();
    void onActive(bool value);
    void onPosChanged(int x, int y);
    void onSizeChanged(unsigned width, unsigned height);
private:
    Stage          *_stage;
    GLFWwindow     *_window;
    std::string     _title;
    Box2f           _bounds;
    bool            _resizable;
    bool            _visible;
}; 

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

enum class StageDisplayState {
    FULL_SCREEN,
    FULL_SCREEN_INTERACTIVE,
    NORMAL,
};

class Stage final : public DisplayObjectContainer, public singleton<Stage> {
    friend class Object;
    friend class NativeWindow;
public:
    bool run() noexcept;
    StageScaleMode scaleMode() const noexcept;
    void scaleMode(StageScaleMode value) noexcept;

    bool allowFullScreen() const;

    Align align() const {
        return _align;
    }
    void align(Align value);

    unsigned color() const{
        return _color;
    }
    void color(unsigned value);

    StageDisplayState displayState() const {
        return _displayState;
    }
    void displayState(StageDisplayState value);

    InteractiveObject *focus() const {
        return _focus;
    }
    void focus(ptr<InteractiveObject> value);

    float frameRate() const {
        return _frameRate;
    }
    void frameRate(float value);

    unsigned fullScreenHeight() const;
    unsigned fullScreenWidth() const;

    unsigned stageWidth() const {
        return _stageWidth;
    }
    void stageWidth(unsigned value);

    unsigned stageHeight() const {
        return _stageHeight;
    }
    void stageHeight(unsigned value);

    NativeWindow *nativeWindow() const {
        return _nativeWindow;
    }

    const DriverInfo &driverInfo() const noexcept {
        return *_driverInfo;
    }

    virtual void size(const Size2f &) override;
    virtual void width(float) override;
    virtual void height(float) override;
    virtual Vec3f position() const override;
    virtual void position(const Vec3f&) override;
    virtual float x() const override;
    virtual void x(float) override;
    virtual float y() const override;
    virtual void y(float) override;
    virtual float z() const override;
    virtual void z(float) override;
    virtual void rotation(const Vec3f&) override;
    virtual void rotationX(float) override;
    virtual void rotationY(float) override;
    virtual void rotationZ(float) override;
    virtual void scale(const Vec3f&) override;
    virtual void scaleX(float) override;
    virtual void scaleY(float) override;
    virtual void scaleZ(float) override;
    virtual void visible(bool) override;

    bool checkVisibility(const Box2f &bounds) noexcept;
protected:
    virtual bool init() override;

private:
    Stage() noexcept;

    void onActive(bool value);
    void onPosChanged(int x, int y);
    void onSizeChanged(unsigned width, unsigned height);
    void onFramebufferSizeChanged(unsigned width, unsigned height);
    void onClose();
    void updateViewPort();
    void render();
    virtual void draw(Renderer &renderer, const Matrix &mat) override;
private:
    ptr<NativeWindow>      _nativeWindow;
    ptr<DriverInfo>        _driverInfo;
    ptr<InteractiveObject> _focus;
    unsigned               _color;
    Align                  _align;
    StageDisplayState      _displayState;
    StageScaleMode         _scaleMode;
    float                  _frameRate;
    GLFWmonitor           *_monitor;
    unsigned               _stageWidth;
    unsigned               _stageHeight;
    bool                   _exit;
    owned_ptr<Matrix>      _projection;
    object<Renderer>       _renderer;
public:
    Box2f                  fullScreenSourceRect;
}; 

GV_NS_END

#endif
