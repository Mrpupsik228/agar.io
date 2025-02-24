#include "window.h"
#include "stb_image.h"

#define HWND static_cast<GLFWwindow*>(Window::handle)

namespace Brainstorm {
    bool Window::created = false, Window::closed = false;

    float Window::aspect;

    void* Window::handle = nullptr;
    unsigned int Window::currentFrame = 1;

    unsigned int* Window::keys = new unsigned int[static_cast<size_t>(KeyCode::LAST) + 2]();
    unsigned int* Window::buttons = new unsigned int[static_cast<size_t>(KeyCode::LAST) + 2]();

    glm::vec2 Window::lastMousePosition = {}, Window::mousePosition = {}, Window::mouseDelta = {}, Window::mouseScroll = {}, Window::mouseScrollCapture = {};
    std::vector<Runnable*> Window::runnables = {};

    ViewportBounds Window::viewportBounds = {};

    EventCallback Window::eventCallback = nullptr;
    Event Window::eventCache = {};

    void Runnable::onUpdate() {}
    void Runnable::onEvent(const Event& event) {}

    void Window::resetEventCache() {
        Window::eventCache.character = 0;
        Window::eventCache.keyCode = KeyCode::UNKNOWN;
        Window::eventCache.keyAction = KeyAction::UNKNOWN;
        Window::eventCache.mouseButton = MouseButton::UNKNOWN;
        Window::eventCache.mouseButtonAction = ButtonAction::UNKNOWN;
        Window::eventCache.mods = static_cast<int>(KeyMods::UNKNOWN);
        Window::eventCache.mouseX = 0.0f;
        Window::eventCache.mouseY = 0.0f;
        Window::eventCache.mouseScrollDx = 0.0f;
        Window::eventCache.mouseScrollDy = 0.0f;
        Window::eventCache.framebufferWidth = 0;
        Window::eventCache.framebufferHeight = 0;
    }

	void Window::create(int width, int height, const char* title) {
        if (Window::created) {
            Logger::error("Window already created!");
            return;
        }
        Window::created = true;

        if (!glfwInit()) {
            Logger::fatal("Could not initialize Brainstorm.");
            return;
        }

        stbi_set_flip_vertically_on_load(true);
        Logger::info("Brainstorm initialized.\n");

        if ((Window::handle = glfwCreateWindow(width, height, title, nullptr, nullptr)) == nullptr) {
            Logger::error("Could not create the window. Params: width=%d, height=%d, title=\"%s\".", width, height, title);
            return;
        }

        float aspect = static_cast<float>(width) / static_cast<float>(height);

        glfwSetFramebufferSizeCallback(HWND, [](GLFWwindow* window, int width, int height) -> void {
            ViewportBounds bounds = Window::viewportBounds;
            glViewport(
                static_cast<GLint>(bounds.offset.x * width), static_cast<GLint>(bounds.offset.y * height),
                static_cast<GLint>(bounds.scale.x * width), static_cast<GLint>(bounds.scale.y * height)
            );

            Window::aspect = static_cast<float>(width) / static_cast<float>(height);
            
            Window::resetEventCache();

            Window::eventCache.type = EventType::FRAMEBUFFER_RESIZE;
            Window::eventCache.framebufferWidth = width;
            Window::eventCache.framebufferHeight = height;

            if (Window::eventCallback != nullptr) Window::eventCallback(Window::eventCache);
            for (Runnable* runnable : Window::runnables) {
                runnable->onEvent(Window::eventCache);
            }
        });
        
        glfwSetKeyCallback(HWND, [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
            switch (action) {
            case GLFW_RELEASE:
                Window::keys[key + 1] = 0;
                break;
            case GLFW_PRESS:
                Window::keys[key + 1] = Window::currentFrame;
                break;
            }

            Window::resetEventCache();

            Window::eventCache.type = EventType::KEY;
            Window::eventCache.keyCode = KeyCode(key);
            Window::eventCache.keyAction = KeyAction(action);

            if (Window::eventCallback != nullptr) Window::eventCallback(Window::eventCache);
            for (Runnable* runnable : Window::runnables) {
                runnable->onEvent(Window::eventCache);
            }
        });
        glfwSetCharCallback(HWND, [](GLFWwindow* window, unsigned int codepoint) -> void {
            Window::resetEventCache();

            Window::eventCache.type = EventType::CHAR;
            Window::eventCache.character = codepoint;

            if (Window::eventCallback != nullptr) Window::eventCallback(Window::eventCache);
            for (Runnable* runnable : Window::runnables) {
                runnable->onEvent(Window::eventCache);
            }
        });
        glfwSetMouseButtonCallback(HWND, [](GLFWwindow* window, int button, int action, int mods) -> void {
            switch (action) {
                case GLFW_RELEASE: {
                    Window::buttons[button] = 0;
                    break;
                }
                case GLFW_PRESS: {
                    Window::buttons[button] = Window::currentFrame;
                    break;
                }
            }

            Window::resetEventCache();

            Window::eventCache.type = EventType::MOUSE_BUTTON;
            Window::eventCache.mouseButton = MouseButton(button);
            Window::eventCache.mouseButtonAction = ButtonAction(action);
            Window::eventCache.mods = mods;

            if (Window::eventCallback != nullptr) Window::eventCallback(Window::eventCache);
            for (Runnable* runnable : Window::runnables) {
                runnable->onEvent(Window::eventCache);
            }
        });
        glfwSetCursorPosCallback(HWND, [](GLFWwindow* window, double xpos, double ypos) -> void {
            Window::resetEventCache();

            Window::eventCache.type = EventType::MOUSE_MOVE;
            Window::eventCache.mouseX = static_cast<float>(xpos);
            Window::eventCache.mouseY = static_cast<float>(ypos);

            if (Window::eventCallback != nullptr) Window::eventCallback(Window::eventCache);
            for (Runnable* runnable : Window::runnables) {
                runnable->onEvent(Window::eventCache);
            }
        });
        glfwSetScrollCallback(HWND, [](GLFWwindow* window, double xoffset, double yoffset) -> void {
            Window::mouseScroll.x += static_cast<float>(xoffset);
            Window::mouseScroll.y += static_cast<float>(yoffset);

            Window::resetEventCache();

            Window::eventCache.type = EventType::MOUSE_SCROLL;
            Window::eventCache.mouseScrollDx = static_cast<float>(xoffset);
            Window::eventCache.mouseScrollDy = static_cast<float>(yoffset);

            if (Window::eventCallback != nullptr) Window::eventCallback(Window::eventCache);
            for (Runnable* runnable : Window::runnables) {
                runnable->onEvent(Window::eventCache);
            }
        });

        glfwMakeContextCurrent(HWND);
        Logger::info("Window created.", HWND);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            Logger::error("Could not initialize GLEW.");
            Window::close();

            return;
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	}
    void Window::close() {
        if (Window::closed) {
            Logger::error("Window already closed.");
            return;
        }
        Window::closed = true;

        delete[] Window::keys;
        delete[] Window::buttons;

        for (Runnable* runnable : Window::runnables) {
            delete runnable;
        }

        glfwDestroyWindow(HWND);
        Logger::info("Window destroyed.");
    }

    void Window::pollEvents() {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Window::currentFrame++;

        Window::mouseScrollCapture = Window::mouseScroll;
        Window::mouseScroll = glm::vec2();

        double mx, my;
        glfwGetCursorPos(HWND, &mx, &my);

        Window::mousePosition = glm::vec2(static_cast<float>(mx), static_cast<float>(my));
        Window::mouseDelta = Window::mousePosition - Window::lastMousePosition;
        Window::lastMousePosition = Window::mousePosition;

        for (Runnable* runnable : Window::runnables) {
            runnable->onUpdate();
        }
    }
    void Window::swapBuffers() {
        glfwSwapBuffers(HWND);
    }

    void Window::setEventCallback(const EventCallback callback) {
        Window::eventCallback = callback;
    }

    float Window::getAspect() {
        return Window::aspect;
    }

    bool Window::isRunning() {
        return !glfwWindowShouldClose(HWND);
    }

    void Window::addRunnable(Runnable* runnable) {
        Window::runnables.push_back(runnable);
    }

    void Window::setPosition(int x, int y) {
        glfwSetWindowPos(HWND, x, y);
    }
    void Window::setPosition(const glm::ivec2& position) {
        glfwSetWindowPos(HWND, position.x, position.y);
    }

    void Window::setX(int x) {
        Window::setPosition(x, Window::getY());
    }
    void Window::setY(int y) {
        Window::setPosition(Window::getX(), y);
    }

    glm::ivec2 Window::getPosition() {
        glm::ivec2 position{};
        glfwGetWindowPos(HWND, &position.x, &position.y);

        return position;
    }

    int Window::getX() {
        int x;
        glfwGetWindowPos(HWND, &x, nullptr);

        return x;
    }
    int Window::getY() {
        int y;
        glfwGetWindowPos(HWND, nullptr, &y);

        return y;
    }

    void Window::setSize(int width, int height) {
        glfwSetWindowSize(HWND, width, height);
    }
    void Window::setSize(const glm::ivec2& size) {
        glfwSetWindowSize(HWND, size.x, size.y);
    }

    void Window::setWidth(int width) {
        Window::setSize(width, Window::getHeight());
    }
    void Window::setHeight(int height) {
        Window::setSize(Window::getWidth(), height);
    }

    glm::ivec2 Window::getSize() {
        glm::ivec2 size{};
        glfwGetWindowSize(HWND, &size.x, &size.y);

        return size;
    }

    int Window::getWidth() {
        int width;
        glfwGetWindowSize(HWND, &width, nullptr);

        return width;
    }
    int Window::getHeight() {
        int height;
        glfwGetWindowSize(HWND, nullptr, &height);

        return height;
    }

    glm::ivec2 Window::getFrameBufferSize() {
        glm::ivec2 size{};
        glfwGetFramebufferSize(HWND, &size.x, &size.y);

        return size;
    }

    int Window::getFrameBufferWidth() {
        int width;
        glfwGetFramebufferSize(HWND, &width, nullptr);

        return width;
    }
    int Window::getFrameBufferHeight() {
        int height;
        glfwGetFramebufferSize(HWND, nullptr, &height);

        return height;
    }

    void Window::setTitle(const char* title) {
        glfwSetWindowTitle(HWND, title);
    }

    void* Window::getHandle() {
        return Window::handle;
    }

    void Window::grabMouse() {
        glfwSetInputMode(HWND, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        Window::resetMouse();
    }
    void Window::releaseMouse() {
        glfwSetInputMode(HWND, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        Window::resetMouse();
    }
    void Window::toggleMouse() {
        glfwSetInputMode(HWND, GLFW_CURSOR, Window::isMouseGrabbed() ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        Window::resetMouse();
    }

    bool Window::isMouseGrabbed() {
        return glfwGetInputMode(HWND, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
    }

    void Window::resetMouse() {
        Window::mouseDelta = {};

        double x, y;
        glfwGetCursorPos(HWND, &x, &y);

        Window::mousePosition = glm::vec2(static_cast<float>(x), static_cast<float>(y));
        Window::lastMousePosition = Window::mousePosition;
    }

    void Window::enableVSync() {
        glfwSwapInterval(1);
    }
    void Window::disableVSync() {
        glfwSwapInterval(0);
    }

    bool Window::isKeyPressed(KeyCode key) {
        return Window::keys[static_cast<size_t>(key) + 1] > 0;
    }
    bool Window::isKeyJustPressed(KeyCode key) {
        return Window::keys[static_cast<size_t>(key) + 1] == Window::currentFrame - 1;
    }

    bool Window::isMouseButtonPressed(MouseButton button) {
        return Window::buttons[static_cast<size_t>(button)] > 0;
    }
    bool Window::isMouseButtonJustPressed(MouseButton button) {
        return Window::buttons[static_cast<size_t>(button)] == Window::currentFrame - 1;
    }

    glm::vec2 Window::getMousePosition() {
        return Window::mousePosition;
    }
    glm::vec2 Window::getMouseDelta() {
        return Window::mouseDelta;
    }
    glm::vec2 Window::getMouseScrollDelta() {
        return Window::mouseScrollCapture;
    }

    float Window::getMouseX() {
        return Window::mousePosition.x;
    }
    float Window::getMouseY() {
        return Window::mousePosition.y;
    }

    float Window::getMouseDx() {
        return Window::mouseDelta.x;
    }
    float Window::getMouseDy() {
        return Window::mouseDelta.y;
    }

    float Window::getMouseScrollDx() {
        return Window::mouseScrollCapture.x;
    }
    float Window::getMouseScrollDy() {
        return Window::mouseScrollCapture.y;
    }
}