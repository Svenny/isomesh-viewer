#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QSharedPointer>

#include <isomesh/data/mesh.hpp>

#include "camera.h"

class ViewerWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
	Q_OBJECT

public:
	explicit ViewerWidget (QWidget *parent = nullptr);
	virtual ~ViewerWidget () override = default;

	virtual void keyPressEvent (QKeyEvent *e) override;
	virtual void keyReleaseEvent (QKeyEvent *e) override;
	virtual void mouseMoveEvent (QMouseEvent *e) override;
	virtual void mousePressEvent (QMouseEvent *e) override;
	virtual void mouseReleaseEvent (QMouseEvent *e) override;
	virtual void focusOutEvent(QFocusEvent* event) override;
	virtual void wheelEvent(QWheelEvent *event) override;

	void setLightDirection(glm::vec3 dir);
	void enableLighting(bool enabled);
	void enableNormalColors(bool enabled);

public slots:
	void setMesh (QSharedPointer<isomesh::Mesh> mesh);

protected:
	virtual void initializeGL () override;
	virtual void paintGL () override;
	virtual void resizeGL (int w, int h) override;

private:
	QOpenGLShaderProgram m_program;
	QOpenGLVertexArrayObject m_VAO;
	QOpenGLBuffer m_VBO, m_EBO;
	int m_mvpLocation, m_meshIndicesCount;
	int m_lightDirLocation, m_useLightingLocation, m_useNormalColorLocation;
	bool m_wireframeEnabled = false;

	Camera m_camera;
	QPoint m_lastMousePos;
};

#endif // VIEWERWIDGET_H
