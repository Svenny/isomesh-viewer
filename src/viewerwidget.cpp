#include "viewerwidget.h"

#include <QMouseEvent>
#include <QKeyEvent>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

ViewerWidget::ViewerWidget (QWidget *parent) :
	QOpenGLWidget (parent),
	m_VBO (QOpenGLBuffer::VertexBuffer),
	m_EBO (QOpenGLBuffer::IndexBuffer)
{}

void ViewerWidget::initializeGL () {
	initializeOpenGLFunctions ();
	qDebug () << "Started OpenGL version" << format ().version ();

	m_program.addCacheableShaderFromSourceFile (QOpenGLShader::Vertex, ":/shaders/main.vert");
	m_program.addCacheableShaderFromSourceFile (QOpenGLShader::Fragment, ":/shaders/main.frag");
	if (!m_program.link ())
		throw std::runtime_error ("Couldn't compile shaders");
	m_mvpLocation = m_program.uniformLocation ("MVP");

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
	glDrawElements (GL_TRIANGLES, m_meshIndicesCount, GL_UNSIGNED_INT, nullptr);
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

void ViewerWidget::mouseMoveEvent (QMouseEvent *) {
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
