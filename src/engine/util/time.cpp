#include "time.h"
#include <chrono>

namespace Brainstorm {
	Timer::Timer() {
		this->lastTime = 0;
		this->update();

		this->delta = 0.0f;
		this->time = 0.0f;
		this->realTime = 0.0f;
		this->scale = 1.0f;
	}

	void Timer::update() {
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		int64_t currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

		this->delta = (float)((double)(currentTime - this->lastTime) / 1000000000.0);
		this->lastTime = currentTime;

		this->time += this->delta * this->scale;
		this->realTime += this->delta;
	}

	float Timer::getDelta() const {
		return this->delta * this->scale;
	}
	float Timer::getRealDelta() const {
		return this->delta;
	}
	float Timer::getTime() const {
		return this->time;
	}
	float Timer::getRealTime() const {
		return this->realTime;
	}
}