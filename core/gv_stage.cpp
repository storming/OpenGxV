#include "opengxv.h"
#include "gv_stage.h"
#include "gv_log.h"
#include "glfw3.h"

#define GV_STAGE_DEFAULT_WIDTH  1280
#define GV_STAGE_DEFAULT_HEIGHT 800
#define GV_STAGE_DEFAULT_TITLE  "OpenGxV"

GV_NS_BEGIN

/* NativeWindowBoundsEvent */
NativeWindowBoundsEvent::NativeWindowBoundsEvent(const ptr<UniStr> &type, const Box2f &beforeBounds, const Box2f &afterBounds) noexcept
: Event(type),
  _beforeBounds(beforeBounds),
  _afterBounds(afterBounds)
{ }

NativeWindowBoundsEvent::NativeWindowBoundsEvent(const NativeWindowBoundsEvent &x) noexcept 
: Event(x),
  _beforeBounds(x._beforeBounds),
  _afterBounds(x._afterBounds)
{ }

ptr<Event> NativeWindowBoundsEvent::clone() {
    return object<NativeWindowBoundsEvent>(*this);
}

/* NativeWindow */
NativeWindow::NativeWindow(Stage *stage) 
: _stage(stage),
  _window(),
  _title(GV_STAGE_DEFAULT_TITLE),
  _bounds(0, 0, GV_STAGE_DEFAULT_WIDTH, GV_STAGE_DEFAULT_HEIGHT),
  _resizable(true),
  _visible(true)
{ }

bool NativeWindow::active() const {
    return _window && glfwGetWindowAttrib(_window, GLFW_FOCUSED); 
}

void NativeWindow::activate() {
    if (_window) {
        glfwShowWindow(_window); 
    }
}

bool NativeWindow::closed() const {
    return _stage->_exit;
}

void NativeWindow::close() {
    _stage->onClose();
}

bool NativeWindow::maximizable() const {
    return false;
}

bool NativeWindow::minimizable() const {
    return true;
}

bool NativeWindow::resizable() const {
    return _resizable;
}

void NativeWindow::resizable(bool value) {
    _resizable = value;
}

const char *NativeWindow::title() const {
    return _title.c_str();
}

void NativeWindow::title(const char *value) {
    if (_title != value) {
        _title = value;
        if (_window) {
            glfwSetWindowTitle(_window, value); 
        }
    }
}

NativeWindowDisplayState NativeWindow::displayState() const {
    if (_window) {
        if (glfwGetWindowAttrib(_window, GLFW_ICONIFIED)) {
            return NativeWindowDisplayState::MINIMIZED; 
        }
        else {
            return NativeWindowDisplayState::NORMAL; 
        }
    }
    return NativeWindowDisplayState::NORMAL; 
}

bool NativeWindow::visible() const {
    if (_window) {
        return glfwGetWindowAttrib(_window, GLFW_VISIBLE) != 0;
    }
    else {
        return _visible;
    }
}

void NativeWindow::visible(bool value) {
    if (_window) {
        if (value) {
            glfwShowWindow(_window);
        }
        else {
            glfwHideWindow(_window);
        }
    }
    else {
        _visible = value;
    }
}

Box2f NativeWindow::bounds() const {
    return _bounds;
}

void NativeWindow::bounds(const Box2f &value) {
    if (value != _bounds) {
        if (_window) {
            glfwSetWindowPos(_window, (int)value.x(), (int)value.y());
            glfwSetWindowSize(_window, (int)value.width(), (int)value.height());
        }
        else {
            _bounds = value;
        }
    }
}

int NativeWindow::x() const {
    return (int)_bounds.x();
}

void NativeWindow::x(int value) {
    if (value != _bounds.x()) {
        if (_window) {
            glfwSetWindowPos(_window, value, (int)_bounds.y());
        }
        else {
            _bounds.x((float)value);
        }
    }
}

int NativeWindow::y() const {
    return (int)_bounds.y();
}

void NativeWindow::y(int value) {
    if (value != _bounds.y()) {
        if (_window) {
            glfwSetWindowPos(_window, (int)_bounds.x(), value);
        }
        else {
            _bounds.y((float)value);
        }
    }
}

unsigned NativeWindow::width() const {
    return (unsigned)_bounds.width();
}

void NativeWindow::width(unsigned value) {
    if (value != _bounds.width()) {
        if (_window) {
            glfwSetWindowSize(_window, (int)value, (int)_bounds.height());
        }
        else {
            _bounds.width((float)value);
        }
    }
}

unsigned NativeWindow::height() const {
    return (unsigned)_bounds.height();
}

void NativeWindow::height(unsigned value) {
    if (value != _bounds.height()) {
        if (_window) {
            glfwSetWindowSize(_window, (int)_bounds.width(), (int)value);
        }
        else {
            _bounds.height((float)value);
        }
    }
}

void NativeWindow::restore() {
    if (_window) {
        glfwRestoreWindow(_window); 
    }
}

void NativeWindow::maximize() {
}

void NativeWindow::minimize() {
    if (_window) {
        glfwIconifyWindow(_window); 
    }
}

bool NativeWindow::create() {
    glfwWindowHint(GLFW_RESIZABLE, _resizable ? GL_TRUE : GL_FALSE); 
    glfwWindowHint(GLFW_VISIBLE, _visible ? GL_TRUE : GL_FALSE);
    _window = glfwCreateWindow((int)_bounds.width(), (int)_bounds.height(), _title.c_str(), nullptr, nullptr);
    if (!_window) {
        return false;
    }

    int x, y;
    int width, height;
    glfwSetWindowPos(_window, (int)_bounds.x(), (int)_bounds.y());
    glfwGetWindowPos(_window, &x, &y);
    glfwGetWindowSize(_window, &width, &height);
    _bounds = Box2f((float)x, (float)y, (float)width, (float)height);
    return true;
}

Stage::Stage() noexcept 
: _color(0),
  _align(Align::TOP_LEFT),
  _displayState(StageDisplayState::NORMAL),
  _scaleMode(StageScaleMode::SHOW_ALL),
  _frameRate(60),
  _monitor(),
  _stageWidth(),
  _stageHeight(),
  _exit(true)
{
    _projection = new Matrix;
}

bool Stage::init() {
    glfwInit();

    glfwSetErrorCallback([](int code, const char*msg) {
        gv_error("glfw: errno='%d', %s.", code, msg);
    });

    _monitor = glfwGetPrimaryMonitor();
    if (!_monitor) {
        return false;
    }

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(128, 128, GV_STAGE_DEFAULT_TITLE, nullptr, nullptr);
    if (!window) {
        return false;
    }
    glfwMakeContextCurrent(window); 

    GLenum n = glewInit();
    if (n != GLEW_OK) {
        glfwDestroyWindow(window); 
        gv_error("glew init failed, %s.", (char *)glewGetErrorString(n));
        return false;
    }
    _driverInfo = object<DriverInfo>();
    _nativeWindow = object<NativeWindow>(this);

    int x, y;
    glfwGetWindowPos(window, &x, &y);
    _nativeWindow->x(x);
    _nativeWindow->y(y);
    glfwDestroyWindow(window); 
    glfwMakeContextCurrent(nullptr);
    return true;
}

void draw2(const Matrix &mat) {
    static Vec3f pp1(-60.f, -40.f, 0.f);
    static Vec3f pp2(60.f, -40.f, 0.f);
    static Vec3f pp3(0.f, 60.f, 0.f);
    Vec3f p1, p2, p3;

    p1 = mat * pp1;
    p2 = mat * pp2;
    p3 = mat * pp3;
    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(p1.x(), p1.y(), p1.z());
    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(p2.x(), p2.y(), p2.z());
    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(p3.x(), p3.y(), p3.z());
    glEnd();
}

bool Stage::run() noexcept {
    if (!_nativeWindow->create()) {
        return false;
    }
    glfwMakeContextCurrent(_nativeWindow->_window); 

    GLenum n = glewInit();
    if (n != GLEW_OK) {
        gv_error("glew init failed, %s.", (char *)glewGetErrorString(n));
        return false;
    }

    _exit = false;

    int w, h;
    glfwGetFramebufferSize(_nativeWindow->_window, &w, &h);
    if (!_stageWidth) {
        _stageWidth = w;
    }
    if (!_stageHeight) {
        _stageHeight = h;
    }

    glfwSetWindowUserPointer(_nativeWindow->_window, this);
    glfwSetWindowFocusCallback(_nativeWindow->_window, [](GLFWwindow *window, int value) {
        ((Stage*)glfwGetWindowUserPointer(window))->onActive(value != 0);
    });
    glfwSetWindowPosCallback(_nativeWindow->_window, [](GLFWwindow *window, int x, int y) {
        ((Stage*)glfwGetWindowUserPointer(window))->onPosChanged(x, y);
    });
    glfwSetWindowSizeCallback(_nativeWindow->_window, [](GLFWwindow *window, int width, int height) {
        ((Stage*)glfwGetWindowUserPointer(window))->onSizeChanged(width, height);
    });
    glfwSetWindowCloseCallback(_nativeWindow->_window, [](GLFWwindow *window) {
        ((Stage*)glfwGetWindowUserPointer(window))->onClose();
    });

    glfwSetFramebufferSizeCallback(_nativeWindow->_window, [](GLFWwindow *window, int width, int height) {
        ((Stage*)glfwGetWindowUserPointer(window))->onFramebufferSizeChanged(width, height);
    });

    updateViewPort();
    //glfwSwapInterval(0);
    while (!_exit) {
        render();
        glfwSwapBuffers(_nativeWindow->_window);
        glfwPollEvents();
    }

    glfwDestroyWindow(_nativeWindow->_window);
    glfwMakeContextCurrent(nullptr);
    return true;
}

void Stage::draw(Renderer &renderer, const Matrix &mat) {
}

void Stage::render() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(_projection->data());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (ptr<DisplayObject> child : _container) {
        if (child->_iscontainer) {
            static_cast<DisplayObjectContainer*>(child.get())->render(*_renderer, *_matrix, (int)_matrixDirty);
        }
        else {
            child->render(*_renderer, *_matrix, (int)_matrixDirty);
        }
    }
    



    Matrix mat;
    mat.setIdentity();
    draw2(matrix() * mat);

    mat.setIdentity();
    mat.translate(Vec3f((float)_stageWidth, 0.f, 0.f));
    draw2(matrix() * mat);

    mat.setIdentity();
    mat.translate(Vec3f(0.f, (float)_stageHeight, 0.f));
    draw2(matrix() * mat);

    mat.setIdentity();
    mat.translate(Vec3f((float)_stageWidth, (float)_stageHeight, 0.f));
    draw2(matrix() * mat);
}

void Stage::updateViewPort() {
    if (!_nativeWindow->_window) {
        return;
    }
    int w, h;
    float frameWidth, frameHeight, width, height, x = 0, y = 0; 
    glfwGetFramebufferSize(_nativeWindow->_window, &w, &h);
    glViewport(0, 0, w, h);
    
    frameWidth = (float)w;
    frameHeight = (float)h;
    float sx = frameWidth / _stageWidth;
    float sy = frameHeight / _stageHeight;
    float s;

    Matrix &mat = matrix();
    mat.setIdentity();
    if (_displayState == StageDisplayState::NORMAL) {
        switch (_scaleMode) {
        case StageScaleMode::EXACT_FIT:
            mat = mat * Scaling(sx, sy, 0.f);
            break;
        case StageScaleMode::SHOW_ALL:
            s = sx < sy ? sx : sy;
            width = frameWidth;
            height = frameHeight;
            mat = mat * Scaling(s, s, 0.f);
            break; 
        case StageScaleMode::NO_BORDER:
            s = sx > sy ? sx : sy;
            width = frameWidth;
            height = frameHeight;
            mat = mat * Scaling(s, s, 0.f);
            break;
        case StageScaleMode::NO_SCALE:
            width = (float)_stageWidth;
            height = (float)_stageHeight;
            break;
        default:
            return;
        }

        switch (_align) {
        case Align::LEFT:
        case Align::TOP_LEFT:
        case Align::BOTTOM_LEFT:
            x = 0;
            break;
        case Align::RIGHT:
        case Align::TOP_RIGHT:
        case Align::BOTTOM_RIGHT:
            x = frameHeight - width;
            break;
        case Align::TOP:
        case Align::BOTTOM:
        case Align::CENTER:
            x = (frameHeight - width) / 2;
            break;
        default:
            return;
        }
        switch (_align) {
        case Align::TOP:
        case Align::TOP_LEFT:
        case Align::TOP_RIGHT:
            y = 0;
            break;
        case Align::BOTTOM:
        case Align::BOTTOM_LEFT:
        case Align::BOTTOM_RIGHT:
            y = frameHeight - height;
            break;
        case Align::LEFT:
        case Align::RIGHT:
        case Align::CENTER:
            y = (frameHeight - height) / 2;
            break;
        default:
            return;
        }

        if (_scaleMode == StageScaleMode::NO_SCALE) {
            mat.translate(Vec3f(x - frameWidth / 2, y - frameHeight / 2, 0.f));
        }
        else {
            mat.translate(Vec3f((float)_stageWidth / -2.f, (float)_stageHeight / -2.f, 0.f));
        }
        matrix(mat); 
        math::ortho(
            -frameWidth / 2, 
            frameWidth / 2, 
            -frameHeight / 2, 
            frameHeight / 2, 
            -1000,
            1000,
            *_projection);
    }

}

void Stage::onActive(bool value) {
    _nativeWindow->dispatchEvent(object<Event>(Event::ACTIVATE));
}

void Stage::onPosChanged(int x, int y) {
    Box2f before(_bounds);
    _bounds.x((float)x);
    _bounds.y((float)y);
    _nativeWindow->dispatchEvent(object<NativeWindowBoundsEvent>(Event::MOVE, before, _nativeWindow->_bounds));
    updateViewPort();
}

void Stage::onSizeChanged(unsigned width, unsigned height) {
    Box2f before(_bounds);
    _bounds.width((float)width);
    _bounds.height((float)height);
    _nativeWindow->dispatchEvent(object<NativeWindowBoundsEvent>(Event::RESIZE, before, _nativeWindow->_bounds));
    updateViewPort();
}

void Stage::onFramebufferSizeChanged(unsigned width, unsigned height) {
    updateViewPort();
}

void Stage::onClose() {
    _exit = true;
}

void Stage::stageWidth(unsigned value) {
    if (StageScaleMode::NO_SCALE == _scaleMode || _stageWidth == value) {
        return;
    }
    updateViewPort(); 
}

void Stage::stageHeight(unsigned value) {
    if (StageScaleMode::NO_SCALE == _scaleMode || _stageHeight == value) {
        return;
    }
    updateViewPort(); 
}

void Stage::align(Align value) {
    if (_align != value) {
        _align = value;
        updateViewPort();
    }
}

void Stage::color(unsigned value) {
}

void Stage::displayState(StageDisplayState value) {
    if (_displayState != value) {
        _displayState = value;
        updateViewPort();
    }
}

void Stage::focus(ptr<InteractiveObject> value) {
}

void Stage::frameRate(float value) {
    _frameRate = value;
}

unsigned Stage::fullScreenHeight() const {
    return glfwGetVideoMode(_monitor)->height;
}

unsigned Stage::fullScreenWidth() const {
    return glfwGetVideoMode(_monitor)->width;
}

bool Stage::allowFullScreen() const {
    return true;
}

bool Stage::checkVisibility(const Box2f &bounds) noexcept {
    return true;
}

void Stage::size(const Size2f&) {
}
void Stage::width(float) {
}
void Stage::height(float) {
}
Vec3f Stage::position() const {
    return Vec3f::Zero();
}
void Stage::position(const Vec3f&) {
}
float Stage::x() const {
    return 0.f;
}
void Stage::x(float) {
}
float Stage::y() const {
    return 0.f;
}
void Stage::y(float) {
}
float Stage::z() const {
    return 0.f;
}
void Stage::z(float) {
}
void Stage::rotation(const Vec3f&) {
}
void Stage::rotationX(float) {
}
void Stage::rotationY(float) {
}
void Stage::rotationZ(float) {
}
void Stage::scale(const Vec3f&) {
}
void Stage::scaleX(float) {
}
void Stage::scaleY(float) {
}
void Stage::scaleZ(float) {
}
void Stage::visible(bool) {
}

GV_NS_END


