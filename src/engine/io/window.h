#pragma once
#include <vector>
#include <string>

#include <glm/ext/vector_int2.hpp>

#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_float2.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "logger.h"
#include "input.h"


namespace Brainstorm {
	struct ViewportBounds {
		glm::vec2 offset = glm::vec2(), scale = glm::vec2(1.0f);
	};
	struct Runnable;

	enum class EventType {
		KEY, CHAR, MOUSE_BUTTON, MOUSE_MOVE, MOUSE_SCROLL, FRAMEBUFFER_RESIZE
	};
	struct Event {
		EventType type;
		unsigned int character = 0;
		
		KeyCode keyCode = KeyCode::UNKNOWN;
		KeyAction keyAction = KeyAction::UNKNOWN;
		
		MouseButton mouseButton = MouseButton::UNKNOWN;
		ButtonAction mouseButtonAction = ButtonAction::UNKNOWN;
		
		int mods = static_cast<int>(KeyMods::UNKNOWN);
		
		float mouseX = 0.0f;
		float mouseY = 0.0f;

		float mouseScrollDx = 0.0f;
		float mouseScrollDy = 0.0f;

		int framebufferWidth = 0;
		int framebufferHeight = 0;
	};

	typedef void (*EventCallback)(const Event& event);

	class Window {
	private:
		static bool created, closed;

		static unsigned int *keys, *buttons;
		static void* handle;

		static unsigned int currentFrame;
		static glm::vec2 lastMousePosition, mousePosition, mouseDelta, mouseScroll, mouseScrollCapture;

		static EventCallback eventCallback;
		static Event eventCache;

		static void resetMouse();
		static inline void resetEventCache();
	public:
		static std::vector<Runnable*> runnables;
		static ViewportBounds viewportBounds;

		static float aspect;

		static void create(int width, int height, const char* title);
		static void swapBuffers();
		static void pollEvents();
		static void close();
		
		static void setEventCallback(const EventCallback callback);

		static float getAspect();

		static void addRunnable(Runnable* runnable);
		static bool isRunning();

		static void setPosition(int x, int y);
		static void setPosition(const glm::ivec2& position);

		static void setX(int x);
		static void setY(int y);
		
		static glm::ivec2 getPosition();
		
		static int getX();
		static int getY();

		static void setSize(int width, int height);
		static void setSize(const glm::ivec2& size);

		static void setWidth(int width);
		static void setHeight(int height);

		static glm::ivec2 getSize();
		
		static int getWidth();
		static int getHeight();

		static glm::ivec2 getFrameBufferSize();
		
		static int getFrameBufferWidth();
		static int getFrameBufferHeight();

		static void setTitle(const char* title);

		static void enableVSync();
		static void disableVSync();

		static void grabMouse();
		static void releaseMouse();
		static void toggleMouse();

		static bool isMouseGrabbed();

		static void* getHandle();

		static bool isKeyPressed(KeyCode key);
		static bool isKeyJustPressed(KeyCode key);

		static bool isMouseButtonPressed(MouseButton button);
		static bool isMouseButtonJustPressed(MouseButton button);

		static glm::vec2 getMousePosition();
		static glm::vec2 getMouseDelta();
		static glm::vec2 getMouseScrollDelta();

		static float getMouseX();
		static float getMouseY();

		static float getMouseDx();
		static float getMouseDy();

		static float getMouseScrollDx();
		static float getMouseScrollDy();
	};

	struct Runnable {
		virtual ~Runnable() = default;

		virtual void onUpdate();
		virtual void onEvent(const Event& event);
	};
}