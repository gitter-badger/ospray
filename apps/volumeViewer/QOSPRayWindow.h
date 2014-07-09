#pragma once

#include <QGLWidget>
#include <ospray/ospray.h>

struct Viewport
{
    Viewport() : from(0,-1,0),
                 at(0,0,0),
                 up(0,0,1),
                 aspect(1.f),
                 fovY(60.f),
                 modified(true)
    {
        frame = osp::affine3f::translate(from) * osp::affine3f(embree::one);
    }

    osp::vec3f from;
    osp::vec3f at;
    osp::vec3f up;

    /*! aspect ratio (width / height) */
    float aspect;

    /*! vertical field of view (degrees) */
    float fovY;

    /*! this flag should be set every time the viewport is modified */
    bool modified;

    /*! camera frame in which the Y axis is the depth axis, and X
    and Z axes are parallel to the screen X and Y axis. The frame
    itself remains normalized. */
    osp::affine3f frame;

    /*! set 'up' vector. if this vector is (0,0,0) the window will
    *not* apply the up-vector after camera manipulation */
    void snapUp()
    {
        if(fabsf(dot(up,frame.l.vz)) < 1e-3f)
            return;

        frame.l.vx = normalize(cross(frame.l.vy,up));
        frame.l.vz = normalize(cross(frame.l.vx,frame.l.vy));
        frame.l.vy = normalize(cross(frame.l.vz,frame.l.vx));
    }
};


class QOSPRayWindow : public QGLWidget
{
public:

    QOSPRayWindow(OSPRenderer renderer);
    virtual ~QOSPRayWindow();

    void setRenderingEnabled(bool renderingEnabled);

    virtual void setWorldBounds(const osp::box3f &worldBounds);

    OSPFrameBuffer getFrameBuffer();

protected:

    virtual void paintGL();
    virtual void resizeGL(int width, int height);
    virtual void mousePressEvent(QMouseEvent * event);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent * event);

    /*! only render when this flag is true. this allows the window to be created before all required components are ospCommit()'d. */
    bool renderingEnabled_;

    osp::vec2i windowSize_;
    Viewport viewport_;
    osp::box3f worldBounds_;
    QPoint lastMousePosition_;

    OSPFrameBuffer frameBuffer_;
    OSPRenderer renderer_;
    OSPCamera camera_;
};
