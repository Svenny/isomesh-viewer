#ifndef CAMERA_H
#define CAMERA_H

#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera {
public:
	Camera ();

	glm::mat4 getPVMatrix () const noexcept { return m_proj * m_view; }
	glm::mat4 getViewMatrix () const noexcept { return m_view; }

	float fovDegrees () const noexcept { return m_fovDegrees; }
	float aspectRatio () const noexcept { return m_width / m_height; }

	void setFovDegrees (float f) noexcept;
	void setScreenSize (int w, int h) noexcept;
	bool setKeyState (int key, bool pressed) noexcept;
	void resetKeyState() noexcept;

	void processMouseMovement (float dx, float dy) noexcept;
	void update () noexcept;
protected:
	glm::mat4 m_proj, m_view;
	glm::vec3 m_position;
	glm::quat m_orientation;

	float m_fovDegrees;
	float m_tanAX2, m_tanAY2;

	float m_width, m_height;

	float m_shara;
	int m_sharaga;

	std::chrono::high_resolution_clock::time_point m_lastUpdateTime;

	enum Key : int {
		KeyForward = 0,
		KeyBack,
		KeyLeft,
		KeyRight,
		KeyUp,
		KeyDown,
		KeyRollC,
		KeyRollCCW,

		KeyCount
	};
	bool m_keyPressed[Key::KeyCount];


	void recalcProjection ();
};

#endif // CAMERA_H
