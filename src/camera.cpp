#include "camera.h"

#include <cmath>

#include <QDebug>
#include <QVector3D>
#include <Qt>

const static float kDefaultFov = 65.0f;
const static float kDefaultAspectRatio = 16.0f / 9.0f;
const static float kMouseSensitivity = 0.5f;

const static float kForwardSpeed = 10.0f;
const static float kStrafeSpeed = 5.0f;
const static float kRollSpeed = 1.5f;

const static float kNearPlane = 0.01f;
const static float kFarPlane = 5000.0f;

const float pi = acosf (-1.0f);

Camera::Camera () :
	m_proj (1.0f), m_view (1.0f), m_position (0.0f), m_orientation (1.0f, 0.0f, 0.0f, 0.0f),
	m_fovDegrees (kDefaultFov), m_tanAX2 (tanf (kDefaultFov * 0.5f)), m_tanAY2 (m_tanAX2 / kDefaultAspectRatio),
	m_width (kDefaultAspectRatio), m_height (1.0f), m_avgFrameTime (0), m_framesRendered (0) {
	for (int i = 0; i < 8; i++)
		m_keyPressed[i] = false;
}

void Camera::setFovDegrees (float f) noexcept {
	m_fovDegrees = f;
	recalcProjection ();
}

void Camera::setScreenSize (int w, int h) noexcept {
	m_width = float (w);
	m_height = float (h);
	recalcProjection ();
}

bool Camera::setKeyState (int key, bool pressed) noexcept {
	switch (key) {
	case Qt::Key_W:
		m_keyPressed[0] = pressed;
		return true;
	case Qt::Key_S:
		m_keyPressed[1] = pressed;
		return true;
	case Qt::Key_A:
		m_keyPressed[2] = pressed;
		return true;
	case Qt::Key_D:
		m_keyPressed[3] = pressed;
		return true;
	case Qt::Key_C:
		m_keyPressed[4] = pressed;
		return true;
	case Qt::Key_Space:
		m_keyPressed[5] = pressed;
		return true;
	case Qt::Key_Q:
		m_keyPressed[6] = pressed;
		return true;
	case Qt::Key_E:
		m_keyPressed[7] = pressed;
		return true;
	}
	return false;
}

void Camera::resetKeyState() noexcept {
	for (size_t i = 0; i < Key::KeyCount; i++)
		m_keyPressed[i] = false;
}

glm::quat quatFromEulerAngles (float pitch, float yaw, float roll) {
	glm::vec3 eulerAngle (pitch * 0.5f, yaw * 0.5f, roll * 0.5f);
	glm::vec3 c = glm::cos (eulerAngle * 0.5f);
	glm::vec3 s = glm::sin (eulerAngle * 0.5f);

	glm::quat res;
	res.w = c.x * c.y * c.z + s.x * s.y * s.z;
	res.x = s.x * c.y * c.z - c.x * s.y * s.z;
	res.y = c.x * s.y * c.z + s.x * c.y * s.z;
	res.z = c.x * c.y * s.z - s.x * s.y * c.z;
	return res;
}

void Camera::processMouseMovement (float dx, float dy) noexcept {
	float yawAngle = kMouseSensitivity * atanf (2 * dx * m_tanAX2 / m_width);
	float pitchAngle = kMouseSensitivity * atanf (2 * dy * m_tanAY2 / m_height);
	auto rotQuat = quatFromEulerAngles (pitchAngle, yawAngle, 0.0f);
	m_orientation = glm::normalize (rotQuat * m_orientation);

	m_view = glm::mat4_cast (m_orientation) * glm::translate (glm::mat4 (1.0f), -m_position);
}

void Camera::update () noexcept {
	auto curTime = std::chrono::high_resolution_clock::now ();

	float deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>
		(curTime - m_lastUpdateTime).count ();
	m_lastUpdateTime = curTime;

	m_avgFrameTime = 0.25f * deltaTime + 0.75f * m_avgFrameTime;
	m_framesRendered++;
	if (m_framesRendered % 150 == 0)
		qDebug ().noquote () << QString::number (1.0 / double (m_avgFrameTime), 'f', 1) << "FPS";

	auto rotMat = glm::mat3_cast (m_orientation);
	glm::vec3 dir (rotMat[0][2], rotMat[1][2], rotMat[2][2]);
	glm::vec3 right (rotMat[0][0], rotMat[1][0], rotMat[2][0]);
	glm::vec3 up (rotMat[0][1], rotMat[1][1], rotMat[2][1]);

	float dx = 0;
	if (m_keyPressed[2]) dx -= kStrafeSpeed;
	if (m_keyPressed[3]) dx += kStrafeSpeed;
	m_position += dx * deltaTime * right;

	float dy = 0;
	if (m_keyPressed[4]) dy -= kStrafeSpeed;
	if (m_keyPressed[5]) dy += kStrafeSpeed;
	m_position += dy * deltaTime * up;

	float dz = 0;
	if (m_keyPressed[0]) dz -= kForwardSpeed;
	if (m_keyPressed[1]) dz += kForwardSpeed;
	m_position += dz * deltaTime * dir;


	float rollAngle = 0;
	if (m_keyPressed[6]) rollAngle -= kRollSpeed * deltaTime;
	if (m_keyPressed[7]) rollAngle += kRollSpeed * deltaTime;
	auto rotQuat = quatFromEulerAngles (0, 0, rollAngle);
	m_orientation = glm::normalize (rotQuat * m_orientation);

	m_view = glm::mat4_cast (m_orientation) * glm::translate (glm::mat4 (1.0f), -m_position);
}

void Camera::recalcProjection () {
	m_tanAX2 = tanf (m_fovDegrees * pi / 360.0f);
	m_tanAY2 = m_tanAX2 * m_height / m_width;

	m_proj = glm::perspectiveFov (glm::radians (m_fovDegrees), m_width, m_height, kNearPlane, kFarPlane);
}
