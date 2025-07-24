#ifndef VIDEO_PLAYBACK_QT_HPP
#define VIDEO_PLAYBACK_QT_HPP

#include <QOpenGLContext>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMainWindow>

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QMainWindow *parent = nullptr);
	~MainWindow();
protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	void paintEvent(QPaintEvent *e);
	void resizeEvent(QResizeEvent *e);
	bool event(QEvent *e);
};

class OpenglWidget : public QOpenGLWidget {
	Q_OBJECT;
	QOpenGLContext *q_opengl_context;
	QOpenGLFunctions *q_opengl_functions;
	QSurface *q_surface;
public:
	OpenglWidget(MainWindow *parent = nullptr);
	~OpenglWidget();
protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	void paintEvent(QPaintEvent *e);
	void resizeEvent(QResizeEvent *e);
	bool event(QEvent *e);
};

#endif
