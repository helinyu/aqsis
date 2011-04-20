// Copyright (C) 2001, Paul C. Gregory and the other authors and contributors
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the software's owners nor the names of its
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// (This is the New BSD license)


#define GL_GLEXT_PROTOTYPES

#include <QtGui/QApplication>

#include <OpenEXR/ImathVec.h>
#include <OpenEXR/ImathGL.h>

#include "ptview.h"
#include "cornellbox.h"

using Imath::V3f;

inline float rad2deg(float r)
{
    return r*180/M_PI;
}

inline float dot(V3f a, V3f b)
{
    return a^b;
}

//------------------------------------------------------------------------------
PointView::PointView(QWidget *parent)
    : QGLWidget(parent),
    m_prev_x(0),
    m_prev_y(0),
    m_zooming(false),
    m_theta(0),
    m_phi(0),
    m_dist(5),
    m_centre(0),
    m_probePos(0),
    m_probeMoveMode(false),
    m_visMode(Vis_Points),
    m_points(),
    m_cloudCenter(0)
{
    setFocusPolicy(Qt::StrongFocus);
}


void PointView::setPoints(const boost::shared_ptr<const PointArray>& points)
{
    m_points = points;
    if(m_points)
    {
        m_cloudCenter = m_points->centroid();
        m_probePos = m_cloudCenter;
    }
}


void PointView::initializeGL()
{
    glLoadIdentity();
    // background colour
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    // set up Z buffer
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    switch(m_visMode)
    {
        case Vis_Points:
        {
            glDisable(GL_COLOR_MATERIAL);
            glDisable(GL_LIGHT0);
        }
        break;
        case Vis_Disks:
        {
            // Materials
            glShadeModel(GL_SMOOTH);
            glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
            glEnable(GL_COLOR_MATERIAL);
            // Lighting
            // light0
            GLfloat lightPos[] = {0, 0, 0, 1};
            GLfloat whiteCol[] = {1, 1, 1, 1};
            glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteCol);
            // whole-scene ambient
            GLfloat ambientCol[] = {0.1, 0.1, 0.1, 1};
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientCol);
            glEnable(GL_LIGHT0);
            glEnable(GL_RESCALE_NORMAL);
        }
        break;
    }

    glEnable(GL_MULTISAMPLE);
}


void PointView::resizeGL(int w, int h)
{
    // Draw on full window
    glViewport(0, 0, w, h);
    // Set camera projection
    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        const float n = 0.1f;
        const float hOn2 = 0.5f*n;
        const float wOn2 = hOn2*width()/height();
        glFrustum(-wOn2, wOn2, -hOn2, hOn2, n, 1000);
    glMatrixMode(GL_MODELVIEW);
}


void PointView::paintGL()
{
    //--------------------------------------------------
    // Draw main scene
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera -> world transform.
    glLoadIdentity();
    glScalef(1, 1, -1);
    glTranslatef(0, 0, m_dist);
    glRotatef(m_theta, 1, 0, 0);
    glRotatef(m_phi, 0, 1, 0);
    glTranslate(m_centre);
    // Center on the point cloud
    glTranslate(-m_cloudCenter);

    // Geometry
    drawAxes();
    drawLightProbe(m_probePos);
    if(m_points)
        drawPoints(*m_points, m_visMode);

    //--------------------------------------------------
    // Draw scene from position of light probe.
    // Clear area of framebuffer.
    glPushAttrib(GL_VIEWPORT_BIT | GL_SCISSOR_BIT | GL_ENABLE_BIT);
    glPushMatrix();

    // Set up & clear viewport
    glEnable(GL_SCISSOR_TEST);
    GLuint viewSize = 300;
    glScissor(0, 0, viewSize, viewSize);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, viewSize, viewSize);

    // Projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    const float n = 0.01f;
    const float wOn2 = 0.5f*n;
    glFrustum(-wOn2, wOn2, -wOn2, wOn2, n, 1000);
    glMatrixMode(GL_MODELVIEW);

    // Camera transform
    glLoadIdentity();
    glScalef(1, 1, -1);
    glRotatef(m_theta, 1, 0, 0);
    glRotatef(m_phi, 0, 1, 0);
    glTranslate(-m_probePos);

    // Geometry
    if(m_points)
        drawPoints(*m_points, m_visMode);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}


void PointView::mousePressEvent(QMouseEvent* event)
{
    m_zooming = event->button() == Qt::RightButton;
    m_prev_x = event->x();
    m_prev_y = event->y();
}


void PointView::mouseMoveEvent(QMouseEvent* event)
{
    float dx = float(m_prev_x - event->x())/width();
    float dy = float(m_prev_y - event->y())/height();
    m_prev_x = event->x();
    m_prev_y = event->y();
    if(m_probeMoveMode)
    {
        // Modify the position of the light probe.
        V3f camDir = V3f(cos(deg2rad(m_theta))*cos(deg2rad(m_phi+90)),
                         sin(deg2rad(m_theta)),
                         cos(deg2rad(m_theta))*sin(deg2rad(m_phi+90)));
        V3f camPos = -m_dist*camDir + m_centre - m_cloudCenter;
        V3f v = m_probePos - camPos;
        V3f up = V3f(cos(deg2rad(m_theta+90))*cos(deg2rad(m_phi+90)),
                     sin(deg2rad(m_theta+90)),
                     cos(deg2rad(m_theta+90))*sin(deg2rad(m_phi+90)));
        V3f right = camDir % up;
        // Distance along view direction to light probe
        float d = dot(v, camDir);
        if(m_zooming)
            m_probePos += dy*d*camDir;
        else
            m_probePos += dy*d*up + dx*d*right;
    }
    else if(m_zooming)
    {
        m_dist *= std::pow(0.9, 30*dy);
    }
    else
    {
        if(event->modifiers() & Qt::ControlModifier)
        {
            m_centre.y +=  m_dist*dy;
            m_centre.x += -m_dist*dx*std::cos(deg2rad(m_phi));
            m_centre.z += -m_dist*dx*std::sin(deg2rad(m_phi));
        }
        else
        {
            m_theta += 400*dy;
            m_phi   += 400*dx;
        }
    }
    repaint();
}


void PointView::wheelEvent(QWheelEvent* event)
{
    double degrees = event->delta()/8.0;
    double steps = degrees / 15.0;
    m_dist *= std::pow(0.9, steps);
    repaint();
}


void PointView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_V)
    {
        m_visMode = (m_visMode == Vis_Points) ? Vis_Disks : Vis_Points;
        initializeGL();
        repaint();
    }
    else if(event->key() == Qt::Key_Z)
        m_probeMoveMode = true;
    else
        event->ignore();
}

void PointView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Z)
        m_probeMoveMode = false;
    else
        event->ignore();
}

/// Draw a set of axes
void PointView::drawAxes()
{
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(1, 0, 0);
    glEnd();
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 1, 0);
    glEnd();
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 1);
    glEnd();
}

/// Draw position of the light probe
void PointView::drawLightProbe(const V3f& P)
{
    glColor3f(0.7,0.7,1);
    glPointSize(100);
    // Draw point at probe position
    glBegin(GL_POINTS);
        glVertex(P);
    glEnd();
    // Draw axis-aligned box around point
    float r = 0.1;
    glBegin(GL_LINE_LOOP);
        glVertex(P + r*V3f(1,-1,1));
        glVertex(P + r*V3f(1,-1,-1));
        glVertex(P + r*V3f(-1,-1,-1));
        glVertex(P + r*V3f(-1,-1,1));
    glEnd();
    glBegin(GL_LINE_LOOP);
        glVertex(P + r*V3f(1,1,1));
        glVertex(P + r*V3f(1,1,-1));
        glVertex(P + r*V3f(-1,1,-1));
        glVertex(P + r*V3f(-1,1,1));
    glEnd();
    glBegin(GL_LINES);
        glVertex(P + r*(V3f(1,1,1)));
        glVertex(P + r*(V3f(1,-1,1)));
        glVertex(P + r*(V3f(-1,1,1)));
        glVertex(P + r*(V3f(-1,-1,1)));
        glVertex(P + r*(V3f(1,1,-1)));
        glVertex(P + r*(V3f(1,-1,-1)));
        glVertex(P + r*(V3f(-1,1,-1)));
        glVertex(P + r*(V3f(-1,-1,-1)));
    glEnd();
    // Draw lines from origin to point
    glBegin(GL_LINE_STRIP);
        glVertex3f(P.x, P.y, P.z);
        glVertex3f(P.x, 0, P.z);
        glVertex3f(0, 0, P.z);
    glEnd();
    glBegin(GL_LINES);
        glVertex3f(0, 0, P.z);
        glVertex3f(0, 0, 0);
    glEnd();
}

/// Draw point cloud using OpenGL
void PointView::drawPoints(const PointArray& points, VisMode visMode)
{
    int ptStride = points.stride;
    int npoints = points.data.size()/ptStride;
    const float* ptData = &points.data[0];

    switch(visMode)
    {
        case Vis_Points:
        {
            // Draw points
            glPointSize(10);
            glColor3f(1,1,1);
            // Set distance attenuation for points, following the usual 1/z
            // law.
            GLfloat attenParams[3] = {0, 0, 1};
            glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, attenParams);
            glPointParameterf(GL_POINT_SIZE_MIN, 0);
            glPointParameterf(GL_POINT_SIZE_MAX, 100);
            // Draw all points at once using vertex arrays.
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            glVertexPointer(3, GL_FLOAT, ptStride*sizeof(float), ptData);
            glColorPointer(3, GL_FLOAT, ptStride*sizeof(float), ptData+7);
            glDrawArrays(GL_POINTS, 0, npoints);
        }
        break;
        case Vis_Disks:
        {
            // Compile radius 1 disk primitive
            V3f p0(0);
            V3f t1(1,0,0);
            V3f t2(0,1,0);
            V3f diskNormal(0,0,1);
            GLuint disk = glGenLists(1);
            glNewList(disk, GL_COMPILE);
                glBegin(GL_TRIANGLE_FAN);
                const int nSegs = 20;
                // Scale so that _min_ radius of polygon approx is 1.
                float scale = 1/cos(M_PI/nSegs);
                glVertex3f(0,0,0);
                for(int i = 0; i <= nSegs; ++i)
                {
                    float angle = 2*M_PI*float(i)/nSegs;
                    glVertex3f(scale*cos(angle), scale*sin(angle), 0);
                }
                glEnd();
            glEndList();
            // Draw points as disks.
            // TODO: Doing this in immediate mode is really slow!  Using
            // vertex shaders would probably be a much better method, or
            // perhaps just compile into a display list?
            glEnable(GL_LIGHTING);
            for(int i = 0; i < npoints; ++i)
            {
                const float* data = ptData + i*ptStride;
                V3f p(data[0], data[1], data[2]);
                V3f n(data[3], data[4], data[5]);
                float r = data[6];
                glColor3f(data[7], data[8], data[9]);
                glPushMatrix();
                // Translate disk to point location and scale
                glTranslate(p);
                glScalef(r,r,r);
                // Transform the disk normal (0,0,1) into the correct normal
                // direction for the current point.  The appropriate transform
                // is a rotation about a direction perpendicular to both
                // normals:
                V3f v = diskNormal % n;
                // via the angle given by the dot product:
                float angle = rad2deg(acosf(diskNormal^n));
                glRotatef(angle, v.x, v.y, v.z);
                // Instance the disk
                glCallList(disk);
                glPopMatrix();
            }
            glDeleteLists(disk, 1);
            glDisable(GL_LIGHTING);
        }
        break;
    }
}

//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Turn on multisampled antialiasing - this makes rendered point clouds
    // look much nicer.
    QGLFormat f = QGLFormat::defaultFormat();
    f.setSampleBuffers(true);
    QGLFormat::setDefaultFormat(f);

    PointViewerWindow window;
    window.pointView().setPoints(cornellBoxPoints(5));
    window.show();

    return app.exec();
}

// vi: set et:
