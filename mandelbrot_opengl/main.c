#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>


#define SHIFT_AMOUNT 21
#define SHIFT_MASK ((1 << SHIFT_AMOUNT) - 1)

#define MAX_ITER_INIT 256
#define SIZE 600

GLubyte pixels[SIZE][SIZE][3];
GLubyte black_rect[SIZE][13][3];
double zoom, center_x, center_y;
char info_string[100];
int max_iter;

int mandelbrot(double complex c)
{
    double complex z = 0.0 + 0*I;
    int i;
    for (i = 0; i != (max_iter-1) && cabsl(z) < 2.0; i++)
    {
        z = z*z + c;
    }
    return i;
}

void put_pixel(int x, int y, char r, char g, char b)
{
    pixels[y][x][0] = r;
    pixels[y][x][1] = g;
    pixels[y][x][2] = b;
}

void render_mandelbrot(double ctr_x, double ctr_y, double zoom) {
    
    double dx, dy, off_x, off_y;
    double complex c;
    int x, y, i;
    off_x = -2.0*zoom+ctr_x;
    off_y = -2.0*zoom+ctr_y;
    dx = 4.0*zoom/SIZE;
    dy = 4.0*zoom/SIZE;
    for (y = 0; y < SIZE; y++)
    {
        for (x = 0; x < SIZE; x++)
        {
            c = x*dx + off_x + (y*dy+off_y)*I;
            i = mandelbrot(c);
            if (max_iter < 256)
            {
                i = i * (256 / max_iter);
            }
            put_pixel(x, y, i % 256, i % 256, i % 256);
        }
    }
}

void init()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, SIZE, SIZE);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, SIZE, 0.0, SIZE);
    max_iter = MAX_ITER_INIT;
    zoom = 1.0;
    center_x = 0.0;
    center_y = 0.0;
    render_mandelbrot(center_x, center_y, zoom);
    memset(black_rect, 0, SIZE*13*3);
}

void display()
{
    // draw mandelbrot
    glRasterPos2i(0, 13);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(SIZE, SIZE, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    // draw black rectangle for text
    glRasterPos2i(0, 0);
    glDrawPixels(SIZE, 13, GL_RGB, GL_UNSIGNED_BYTE, black_rect);

    // draw text
    glRasterPos2i(2, 2);
    sprintf(info_string, "ZOOM:%lldX  X:%.10f  Y:%.10f  MAX_ITER:%d", (long long)(1.0/zoom), center_x, center_y, max_iter);
    int i;
    for (i = 0; info_string[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, info_string[i]);
    }
    
    // swap buffers
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        center_x = center_x + ((((float)x-SIZE/2.0)/SIZE)*4.0)*zoom;
        center_y = center_y - ((((float)y-SIZE/2.0)/SIZE)*4.0)*zoom;
        render_mandelbrot(center_x, center_y, zoom);
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        zoom = zoom / 2.0;
        render_mandelbrot(center_x, center_y, zoom);
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:
            exit(0);
            break;
        case 'r':
            zoom = 1.0;
            center_x = 0.0;
            center_y = 0.0;
            break;
        case 'z':
            if (zoom > 1)
            {
                zoom = zoom/2.0;
            }
            break;
        case 'x':
            zoom = zoom*2.0;
            break;
        case 'i':
            max_iter = max_iter * 2;
            break;
        case 'o':
            if (max_iter > 1)
            {
                max_iter = max_iter / 2;
            }
            break;
        default:
            break;
    }
    render_mandelbrot(center_x, center_y, zoom);
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(SIZE, SIZE+13);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_RGB);
    glutCreateWindow("Mandelbrot");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}
