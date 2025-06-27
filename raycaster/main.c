//#include <glad/glad.h>
#include <GLUT/glut.h>
//#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>

#define mapX  8      //map width
#define mapY  8      //map height
#define mapS 64 // map cube size

int map[] =
{
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,1,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,1,0,0,0,0,1,
    1,1,1,0,0,1,1,1,
    1,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,
};


#define PI 3.1415926535
#define P2 PI/2
#define P3 3 * PI / 2
#define DR 0.01745329 // one degree in rad

float px, py; // player coordinates
float pdx, pdy; // delta x and y of player
float pa; // player angle


void drawMap2D()
{
    int x, y;
    int xo, yo;
    
    for (y = 0; y < mapY; y++)
    {
        for (x = 0; x < mapX; x++)
        {
            if (map[y*mapX+x] == 1)
            {
                glColor3f(1, 1, 1);
            }
            else
            {
                glColor3f(0, 0, 0);
            }
            xo = x * mapS;
            yo = y * mapS;
            glBegin(GL_QUADS);
            glVertex2i(xo + 1, yo + 1);
            glVertex2i(xo + 1, yo + mapS - 1);
            glVertex2i(xo + mapS - 1, yo + mapS - 1);
            glVertex2i(xo +mapS - 1, yo + 1);
            glEnd();
            
        }
        
    }
}

float degToRad(int a)
{
    return a * M_PI / 180.0;
}

int fixAng(int a)
{
    if (a > 359)
    {
        a -= 360;
    }
    if (a < 0)
    {
        a += 360;
    }
    return a;
}

void drawPlayer()
{
    // draw the dot
    glColor3f(1, 1, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px, py);
    glEnd();
    
    // draw line of sight
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(px, py);
    glVertex2i(px + pdx * 5, py + pdy *5);
    glEnd();
    
}

void buttons(unsigned char key, int x, int y)
{
    if (key == 'a')
    {
        pa += 5;
        pa = fixAng(pa);
        pdx = cos(degToRad(pa));
        pdy = -sin(degToRad(pa));
    }
    if (key == 'd')
    {
        pa -= 5;
        pa = fixAng(pa);
        pdx = cos(degToRad(pa));
        pdy = -sin(degToRad(pa));
    }
    if (key == 'w')
    {
        px += pdx * 5;
        py += pdy * 5;
    }
    if (key == 's')
    {
        px -= pdx * 5;
        py -= pdy * 5;
    }
    glutPostRedisplay();
}


float dist(float ax, float ay, float bx, float by, float ang)
{
    return cos(degToRad(ang))*(bx-ax)-sin(degToRad(ang))*(by-ay);
}

void drawRays3D()
{
    int r, side;
    int mx, my, mp;
    int dof;
    float rx, ry, ra;
    float xo, yo;
    //float disT;
    
    ra = fixAng(pa + 30);
    
    for (r = 0; r < 60; r++)
    {
        
        // ====== check vertical lines =========
        dof = 0;
        float disV = 10000000;
        side = 0;
        float vx = px;
        float vy = py;
        float Tan = tan(degToRad(ra));
        if(cos(degToRad(ra)) > 0.001) //looking left
        {
            rx = (((int)px >>6) <<6) + 64;
            ry = (px - rx) * Tan + py;
            xo = 64;
            yo = -xo * Tan;
        }
        else if(cos(degToRad(ra)) < -0.001) //looking right
        {
            rx = (((int)px>>6)<<6) -0.0001;
            ry = (px - rx) * Tan + py;
            xo = -64;
            yo = -xo * Tan;
            
        }
        else
        {
            rx = px;
            ry = py;
            dof = 8;
        }
        while(dof<8)
        {
            mx = (int)(rx)>>6;
            my = (int)(ry)>>6;
            mp = my * mapX + mx;
            if(mp > 0 && mp < mapX * mapY && map[mp]==1) //hit
            {
                dof = 8;
                disV = cos(degToRad(ra)) * (rx - px) -sin(degToRad(ra)) * (ry - py);
                
            }
            else{ //check next horizontal
                rx += xo;
                ry += yo;
                dof += 1;
                
            }
        }
        vx = rx;
        vy = ry;
        //========= check horizontal lines ==========
        dof = 0;
        float disH = 100000;
        Tan = 1.0 / Tan;
        if(sin(degToRad(ra)) > 0.001) //looking up
        {
            ry = (((int)py >> 6) <<6) - 0.0001;
            rx = (py - ry) * Tan + px;
            yo = -64;
            xo = -yo * Tan;
            
        }
        else if(sin(degToRad(ra)) < -0.001) //looking down
        {
            ry = (((int)py >> 6) << 6) + 64;
            rx = (py - ry) * Tan + px;
            yo = 64;
            xo = -yo * Tan;
            
        }
        else //looking straight left or right
        {
            rx = px;
            ry = py;
            dof = 8;
            
        }
       
        while(dof<8)
        {
            mx = (int)(rx)>>6;
            my = (int)(ry)>>6;
            mp = my * mapX + mx;
            if(mp > 0 && mp < mapX * mapY && map[mp] == 1) //hit
            {
                dof = 8;
                disH = cos(degToRad(ra)) * (rx - px) - sin(degToRad(ra)) * (ry - py);
                
            }
            else //check next horizontal
            {
                rx += xo;
                ry += yo;
                dof += 1;
                
            }
        }
        if(disV < disH)
        {
            rx = vx;
            ry = vy;
            disH = disV;
            glColor3f(0,1,1);
            
        }
        else
        {
            glColor3f(0, 1, 1);
        }
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(px, py);
        glVertex2i(rx, ry);
        glEnd();
        
       
        
        //========= DRAW 3D WALLS ========
        /*float ca = pa - ra;
        if (ca < 0)
        {
            ca += 2 * PI;
        }
        if (ca > 2 * PI)
        {
            ca -= 2 * PI;
        }
        disT = disT * cos(ca); // remove upper lines to get fisheye effect
        */
        
        int lineH = (mapS * 320) / (disH);
        if(lineH > 320)
        {
            lineH = 320;
            
        }                     //line height and limit
        int lineOff = 160 - (lineH>>1);
        glLineWidth(8);
        glBegin(GL_LINES);
        glVertex2i(r * 8 + 530, lineOff);
        glVertex2i(r * 8 + 530, lineH + lineOff);
        glEnd();
        
        ra = fixAng(ra - 1);
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawPlayer();
    drawRays3D();
    glutSwapBuffers();
}


void init()
{
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, 1024, 512, 0);
    px = 300;
    py = 300;
    pdx = cos(pa) * 5;
}

void resize(int w, int h)
{
    glutReshapeWindow(1024, 512);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1024, 512);
    //glutInitWindowPosition(200, 200);
    glutCreateWindow("HELLO");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(buttons);
    glutMainLoop();
}
