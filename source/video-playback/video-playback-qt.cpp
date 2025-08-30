#include "video-playback-qt.hpp"

OpenglWidget::OpenglWidget(MainWindow *parent)
{
	QSurfaceFormat qSurfaceFormat;

	qSurfaceFormat.setDepthBufferSize(24);
	qSurfaceFormat.setVersion(3, 3);
	qSurfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(qSurfaceFormat);
	q_opengl_context = new QOpenGLContext;

	q_opengl_context->setFormat(qSurfaceFormat);
	q_opengl_context->create();
	q_opengl_context->makeCurrent(q_surface);

	q_opengl_functions = q_opengl_context->functions();
}

OpenglWidget::~OpenglWidget()
{

}

void OpenglWidget::initializeGL()
{}

void OpenglWidget::resizeGL(int w, int h)
{}

void OpenglWidget::paintGL()
{
	glClearColor(1.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenglWidget::paintEvent(QPaintEvent *e)
{}

void OpenglWidget::resizeEvent(QResizeEvent *e)
{}

bool OpenglWidget::event(QEvent *e)
{
	return true;
}
