#include "viewerwidget.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QImage>

#include <exception>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

ViewerWidget::ViewerWidget (QWidget *parent) :
	QOpenGLWidget (parent),
	m_VBO (QOpenGLBuffer::VertexBuffer),
	m_EBO (QOpenGLBuffer::IndexBuffer),
	m_texture (nullptr)
{}

void ViewerWidget::initializeGL () {
	initializeOpenGLFunctions ();
	qDebug () << "Started OpenGL version" << format ().version ();

	m_program.addCacheableShaderFromSourceFile (QOpenGLShader::Vertex, ":/shaders/main.vert");
	m_program.addCacheableShaderFromSourceFile (QOpenGLShader::Fragment, ":/shaders/main.frag");
	if (!m_program.link ())
		throw std::runtime_error ("Couldn't compile shaders");
	m_mvpLocation = m_program.uniformLocation ("MVP");
	m_useLightingLocation = m_program.uniformLocation("useLighting");
	m_lightDirLocation = m_program.uniformLocation("lightDir");
	m_useNormalColorLocation = m_program.uniformLocation("useNormalColor");
	m_useTextureLocation = m_program.uniformLocation("useTexture");
	m_textureScaleLocation = m_program.uniformLocation("textureScale");

	m_VAO.create ();
	m_VAO.bind ();
	{
		m_VBO.create ();
		m_VBO.bind ();
		glEnableVertexAttribArray (0);
		glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof (GLfloat), nullptr);
		glEnableVertexAttribArray (1);
		glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof (GLfloat), reinterpret_cast<void *>(3 * sizeof (GLfloat)));
		glEnableVertexAttribArray (2);
		glVertexAttribPointer (2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof (GLfloat), reinterpret_cast<void *> (6 * sizeof (GLfloat)));
		m_VBO.release ();

		m_EBO.create ();
		m_EBO.bind ();
	}
	m_VAO.release ();

	m_program.bind();
	glm::vec3 dir = glm::normalize(glm::vec3(0.3f, 0.9f, 0.3f));
	glUniform3f(m_lightDirLocation, dir.x, dir.y, dir.z);
	glUniform1i(m_useLightingLocation, true);
	glUniform1i(m_useNormalColorLocation, false);
	glUniform1i(m_useTextureLocation, false);
	glUniform1f(m_textureScaleLocation, 0.03125f);
	m_program.release();

	glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
	glEnable (GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
}

void ViewerWidget::resizeGL (int w, int h) {
	m_camera.setScreenSize (w, h);
}

void ViewerWidget::paintGL () {
	auto fun = QOpenGLContext::currentContext ()->versionFunctions<QOpenGLFunctions_3_3_Core> ();
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_wireframeEnabled)
		fun->glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	else
		fun->glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

	m_camera.update ();
	auto PV = m_camera.getPVMatrix ();
	auto M = glm::translate (glm::mat4 (1.0f), glm::vec3 (0, -1.0f, -1.0f));
	auto MVP = PV * M;

	m_program.bind ();
	glUniformMatrix4fv (m_mvpLocation, 1, GL_FALSE, glm::value_ptr (MVP));
	m_VAO.bind ();
	if (m_texture)
		m_texture->bind();
	glDrawElements (GL_TRIANGLES, m_meshIndicesCount, GL_UNSIGNED_INT, nullptr);
	if (m_texture)
		m_texture->release();
	m_VAO.release ();
	m_program.release ();

	update ();
}

void ViewerWidget::setMesh (QSharedPointer<isomesh::Mesh> mesh) {
	// Fill vertex buffer
	m_VBO.bind ();
	m_VBO.allocate (mesh->vertexData (), int (mesh->vertexBytes ()));
	m_VBO.release ();
	// Fill index buffer
	m_VAO.bind ();
	m_EBO.bind ();
	m_EBO.allocate (mesh->indexData (), int (mesh->indexBytes ()));
	m_VAO.release ();
	m_meshIndicesCount = int (mesh->indexCount ());
}

void ViewerWidget::enableLighting(bool enabled)
{
	m_program.bind();
	glUniform1i(m_useLightingLocation, enabled);
	m_program.release();
}

void ViewerWidget::enableNormalColors(bool enabled)
{
	m_program.bind();
	glUniform1i(m_useNormalColorLocation, enabled);
	m_program.release();
}

void ViewerWidget::enableTexture(bool enabled)
{
	m_program.bind();
	glUniform1i(m_useTextureLocation, enabled);
	m_program.release();
}

void ViewerWidget::setLightDirection(glm::vec3 dir)
{
	m_program.bind();
	dir = glm::normalize(dir);
	glUniform3f(m_lightDirLocation, dir.x, dir.y, dir.z);
	m_program.release();
}

void ViewerWidget::setTextureScale(float scale)
{
	m_program.bind();
	glUniform1f(m_textureScaleLocation, scale);
	m_program.release();
}

void ViewerWidget::setTexture(const QImage& image)
{
	if (m_texture)
		delete m_texture;

	m_program.bind();
	m_VAO.bind();
	m_texture = new QOpenGLTexture(image);
	m_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
	m_texture->setWrapMode(QOpenGLTexture::Repeat);
	m_VAO.release();
	m_program.release();
}


void ViewerWidget::keyPressEvent (QKeyEvent *e) {
	if (m_camera.setKeyState (e->key (), true))
		update ();
	else if (e->key () == Qt::Key_F)
		m_wireframeEnabled = !m_wireframeEnabled;
	else e->ignore ();
}

void ViewerWidget::keyReleaseEvent (QKeyEvent *e) {
	if (m_camera.setKeyState (e->key (), false))
		update ();
	else
		e->ignore ();
}

void ViewerWidget::mouseMoveEvent (QMouseEvent *e) {
	// Ignore, if left button not pressed
	if (!(e->buttons () & Qt::MouseButton::LeftButton)) {
		return e->ignore();
	}

	auto cur = cursor ();
	auto delta = cur.pos () - m_lastMousePos;
	cur.setPos (mapToGlobal (rect ().center ()));
	cur.setShape (Qt::CursorShape::BlankCursor);
	setCursor (cur);
	m_lastMousePos = cur.pos ();

	m_camera.processMouseMovement (delta.x (), delta.y ());
	update ();
}

void ViewerWidget::mousePressEvent (QMouseEvent *e) {
	if (e->button () == Qt::MouseButton::LeftButton) {
		auto cur = cursor ();
		cur.setPos (mapToGlobal (rect ().center ()));
		cur.setShape (Qt::CursorShape::BlankCursor);
		setCursor (cur);
		m_lastMousePos = cur.pos ();
	}
	else e->ignore ();
}

void ViewerWidget::mouseReleaseEvent (QMouseEvent *e) {
	if (e->button () == Qt::MouseButton::LeftButton) {
		unsetCursor ();
		update ();
	}
	else e->ignore ();
}

void ViewerWidget::wheelEvent(QWheelEvent* event)
{
	if (event->delta() > 0) {
		m_camera.setForwardSpeed(m_camera.forwardSpeed() * 1.1);
		m_camera.setStrafeSpeed(m_camera.strafeSpeed() * 1.1);
	} else {
		m_camera.setForwardSpeed(m_camera.forwardSpeed() / 1.1);
		m_camera.setStrafeSpeed(m_camera.strafeSpeed() / 1.1);
	}
}

void ViewerWidget::focusOutEvent(QFocusEvent* event)
{
	m_camera.resetKeyState();
	update();
}
