//Dingheng Zhang
//Project 1 test


#ifdef WIN32
#include <windows.h>
#endif

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#else //linux
#include <GL/gl.h>
#include <GL/glut.h>
#endif

//other includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;

#define PI acos(-1)






int m,n; //m for number of polygon,n for each polygon's number of points
int s = 10; //assume the number of points of each polygon less than 10
int w[10]; //each poligon's number of points
int Ymax[10]; //value of Ymax
int Ymin[10]; //value of Ymin
double TransLa[2]; //translation
double Scale[2]; //scale
double Alpha; //rotation
int num_all_points=1000;
double **Side_P1;
double **Side_Q1;
double **All_P1;
double **All_Q1;
int Point_sign;
int *SidePointNum;
int *AllPointNum;
int *GravityP;
int *GravityQ;
int TransSign = 0;
int TransPoly;
int DDA_B;

float pixel_size;

int width;
int length;

int win_width;
int win_height;


double **p;
//for(int i=0;i<m;i++)
//p[i] = new double[s];
double **q;
//for(int i=0;i<m;i++)
//q[i] = new double[s];


float round_value(float v) //make a float num to integer
{
    return floor(v + 0.5);
}

void LineDDA(double X1,double X2,double Y1,double Y2,int I)
{
    double dx = (X2-X1);
    double dy = (Y2-Y1);
    double steps = 0;
    float XInc=0, YInc=0, x=X1,y=Y1;

    steps=(abs(dx)>abs(dy))?(abs(dx)):(abs(dy));
    XInc = dx/(float)steps;
    YInc = dy/(float)steps;
    
    
    
//    glBegin(GL_POINTS);
    
//    glVertex2d(x,y);
    int k;
    
    for (k=0;k<steps;k++)
    {
        x+=XInc;
        y+=YInc;
        Side_P1[I][Point_sign] = round_value(x);
        Side_Q1[I][Point_sign] = round_value(y);
        Point_sign++;
//        glVertex2d(round_value(x), round_value(y));
    }
//    glEnd();
    
//    glFlush();
}

void LineBresenham(int X1, int X2, int Y1, int Y2,int I)
{
     int dx = X2 - X1;
     int dy = Y2 - Y1;
     int ux = ((dx > 0) << 1) - 1;
     int uy = ((dy > 0) << 1) - 1;
     int x = X1, y = Y1, eps;

     eps = 0;dx = abs(dx); dy = abs(dy);
     // glBegin(GL_POINTS);
     if (dx > dy)
     {
         for (x = X1; x != X2; x += ux)
         {
            Side_P1[I][Point_sign] = round_value(x);
            Side_Q1[I][Point_sign] = round_value(y);
            Point_sign++;
            
              // glVertex2d(x,y);
              eps += dy;
              if ((eps << 1) >= dx)
              {
                   y += uy; eps -= dx;
              }
         }
     }
     else
     {
         for (y = Y1; y != Y2; y += uy)
         {
            Side_P1[I][Point_sign] = round_value(x);
            Side_Q1[I][Point_sign] = round_value(y);
            Point_sign++;
              // glVertex2d(x,y);
              eps += dx;
              if ((eps << 1) >= dy)
              {
                   x += ux; eps -= dy;
              }
         }
     }
     Side_P1[I][Point_sign] = round_value(X2);
     Side_Q1[I][Point_sign] = round_value(Y2);
     Point_sign++;
     // glVertex2d(X2,Y2);
     // glEnd();
     // glFlush();
}


void polygons(void)
{
//    glClear(GL_COLOR_BUFFER_BIT);
    if(DDA_B == 0)
    {
     for(int i=0;i<m;i++)
     {
         Point_sign = 0;
         if(w[i]==1)
         {
             LineDDA(p[i][0],p[i][1],q[i][0],q[i][1],i);
            
         }
         else
         {
             int k;
             for(k=0;k<w[i]-1;k++)
             {LineDDA(p[i][k], p[i][k+1], q[i][k], q[i][k+1],i);}
             LineDDA(p[i][w[i]-1],p[i][0], q[i][w[i]-1],q[i][0],i);
            
         }
         SidePointNum[i]=Point_sign;
     }
    }
    if(DDA_B==1)
    {
    for(int i=0;i<m;i++)
    {
       
        if(w[i]==1)
        {
            LineBresenham(p[i][0],p[i][1],q[i][0],q[i][1],i);
           
        }
        else
        {
            int k;
            for(k=0;k<w[i]-1;k++)
            {LineBresenham(p[i][k], p[i][k+1], q[i][k], q[i][k+1],i);}
            LineBresenham(p[i][0], p[i][w[i]-1], q[i][0], q[i][w[i]-1],i);
           
        }
        SidePointNum[i]=Point_sign;
    }
    }
}

void translation(int x)
{
    for(int i=0;i<w[x];i++)
    {
        p[x][i] = p[x][i]+TransLa[0];
        q[x][i] = q[x][i]+TransLa[1];
    }
//    TransLa[0] = TransLa[1] = 0;
}

void scale(int x)
{
    for(int i=0;i<w[x];i++)
    {
        p[x][i] = p[x][i]*Scale[0];
        q[x][i] = q[x][i]*Scale[1];
    }
//    Scale[0] = Scale[1] = 1;
}

void rotation(int x)
{
    Alpha = Alpha*PI/180;
    for(int i=0;i<w[x];i++)
    {
        double r = p[x][i];
        p[x][i] = cos(Alpha)*p[x][i] - sin(Alpha)*q[x][i];
        q[x][i] = sin(Alpha)*r + cos(Alpha)*q[x][i];
    }
//    Alpha = 0;
}

void findmaxmin(void)
{
    for(int i=0;i<m;i++)
    {
        Ymax[i]=0;
        Ymin[i]=2000;
        for(int j=0;j<w[i];j++)
        {
            if(Ymax[i]<q[i][j]) Ymax[i]=round_value(q[i][j]);
            if(Ymin[i]>q[i][j]) Ymin[i]=round_value(q[i][j]);
        }
    }
}

void findgravitypoint()
{
    for(int i=0;i<m;i++)
    {
        double sumP = 0;
        double sumQ = 0;
        for(int j=0;j<AllPointNum[i];j++)
        {
            sumP = sumP + All_P1[i][j];
            sumQ = sumQ + All_Q1[i][j];
        }
        GravityP[i] = sumP/AllPointNum[i];
        GravityQ[i] = sumQ/AllPointNum[i];
    }
    
}

void selfrotation(int x)
{
    TransLa[0] = -GravityP[x];
    TransLa[1] = -GravityQ[x];
    translation(x);
    rotation(x);
    TransLa[0] = GravityP[x];
    TransLa[1] = GravityQ[x];
    translation(x);
}


void selfscale(int x)
{
    TransLa[0] = -GravityP[x];
    TransLa[1] = -GravityQ[x];
    translation(x);
    scale(x);
    TransLa[0] = GravityP[x];
    TransLa[1] = GravityQ[x];
    translation(x);
}

void fillinpoly(void)
{
    polygons();
    findmaxmin();
    for (int i=0;i<m;i++)
    {
        Point_sign = 0;
        int y_depth;
        y_depth = Ymax[i] - Ymin[i] + 1;
        int *NumPointScan = new int [y_depth];
        for(int j=Ymin[i];j<=Ymax[i];j++)
        {
            int min_x=100;
            int max_x=0;
            for(int k=0;k<SidePointNum[i];k++)
            {
                if(Side_Q1[i][k]==j)
                {

                    if(Side_P1[i][k]> max_x) max_x = Side_P1[i][k];
                    if(Side_P1[i][k]< min_x) min_x = Side_P1[i][k];
                }
            }
            if(DDA_B==0)
            {
            int sign_l1 = 0;
            for(int l1=min_x+1;l1<max_x;l1++)
            {
                int t_Sign = 0;
                NumPointScan[(j-Ymin[i])] = 0;
                for(int k1 =0;k1<SidePointNum[i];k1++)
                    {
                        if(Side_Q1[i][k1]==j)
                        {
                            if(l1==Side_P1[i][k1])
                            {
                                t_Sign = 1;
                            }
                        }
                    }
                if(t_Sign==0)
                {
                    int hori = 0;
                    int sign_l2 = 0;
                            for(int l2=l1;l2<=max_x;l2++)
                            {
                                
                                
                                for(int k2 = 0;k2<SidePointNum[i];k2++)
                                {
                                    if(Side_Q1[i][k2]==j)
                                    {
                                        if(l2 == Side_P1[i][k2])
                                        {
                                            if(l2 == sign_l2+1)
                                                hori = 1;
                                            if((l2!=(sign_l2+1))&&hori==0) NumPointScan[(j-Ymin[i])]++;
//                                            if((l2!=(sign_l2+1))&&hori==1)
//                                                NumPointScan[(j-Ymin[i])]=NumPointScan[(j-Ymin[i])]+2;
//                                            if((l2==sign_l2+1)&&(l2==max_x)&&(sign_l1==1)) NumPointScan[(j-Ymin[i])]++;
                                            sign_l2 = l2;
                                        }
                                    }
                                    
                                }
                                
                            }
                
                        
                    if(l1==min_x+1)
                    {
                        if((NumPointScan[(j-Ymin[i])]%2)!=1)
                        {
                            sign_l1 = 1;
                        }
                    }
                    
                    if(sign_l1==1)
                    {
                        NumPointScan[(j-Ymin[i])]++;
                        if(hori==0) NumPointScan[(j-Ymin[i])] = 1;
                    }
                        
                    
                    if((NumPointScan[(j-Ymin[i])]%2)!=0)
                    {
                        All_P1[i][Point_sign] = l1;
                        All_Q1[i][Point_sign] = j;
                        Point_sign++;
                    }
                }
                
            }
        }
            for(int k=0;k<SidePointNum[i];k++)
            {
                if(j==Side_Q1[i][k])
                {
                    All_P1[i][Point_sign] = Side_P1[i][k];
                    All_Q1[i][Point_sign] = Side_Q1[i][k];
                    Point_sign++;
                }
            }
                
            
//            for(int k=min_x;k<=max_x;k++)
//            {
////                glBegin(GL_POINTS);
//
//                All_P1[i][Point_sign] = k;
//                All_Q1[i][Point_sign] = j;
//                Point_sign++;
////                glVertex2d(k,j);
//
////                glEnd();
//
////                glFlush();
//            }
        }
        AllPointNum[i] = Point_sign;
        delete [] NumPointScan;
    }
    findgravitypoint();
}

void transformation()
{
    printf("\n************************************\n");
        printf("Enter which polygon you want to transform:(Press '9' to transform all. Press '0' to skip transformation\n");
        scanf("%d",&TransPoly);
    if(TransPoly!=0)
    {
        TransPoly = TransPoly - 1;
        printf("\n************************************\n");
        printf("Enter which transform you want: '1' for translation,'2' for scaling,'3' for rotation,'4'for slef rotation, '5'for self scaling, or press other keys to cotinue \n");
        scanf("%d",&TransSign);
        if(TransSign==1)
        {
            printf("\n************************************\n");
            printf("Enter which direction you want to translate for x y:\n");
            scanf("%lf%lf",&TransLa[0],&TransLa[1]);
            if(TransPoly==8)
            {
                for(int i=0;i<m;i++)
                    translation(i);
            }
            else
                translation(TransPoly);
        }
        if(TransSign==2)
        {
            printf("\n************************************\n");
            printf("Enter how much you want to scaling for x y:\n");
            scanf("%lf%lf",&Scale[0],&Scale[1]);
            if(TransPoly==8)
            {
                for(int i=0;i<m;i++)
                    scale(i);
            }
            else
                scale(TransPoly);
        }
        if(TransSign==3)
        {
            printf("\n************************************\n");
            printf("Enter the angle of rotation\n");
            scanf("%lf",&Alpha);
            if(TransPoly==8)
            {
                for(int i=0;i<m;i++)
                    rotation(i);
            }
            else
                rotation(TransPoly);
        }
        if(TransSign==4)
        {
            printf("\n************************************\n");
            printf("Enter the angle of self rotation\n");
            scanf("%lf",&Alpha);
            if(TransPoly==8)
            {
                for(int i=0;i<m;i++)
                    selfrotation(i);
            }
            else
                selfrotation(TransPoly);
        }
        if(TransSign==5)
        {
            printf("\n************************************\n");
            printf("Enter how much you want to self scaling for x y:\n");
            scanf("%lf%lf",&Scale[0],&Scale[1]);
            if(TransPoly==8)
            {
                for(int i=0;i<m;i++)
                    selfscale(i);
            }
            else
                selfscale(TransPoly);
        }
    }
    fillinpoly();
}

void drawpoly()
{
    glClear(GL_COLOR_BUFFER_BIT);
    for(int i=0;i<m;i++)
        for(int j = 0;j<AllPointNum[i];j++)
        {
            glBegin(GL_POINTS);
//        if((All_P1[i][j]<=width)&&(All_P1[i][j]>0)&&(All_Q1[i][j]<=length)&&(All_Q1[i][j]>0))
//            {
                glColor3f(.2,.2,1.0);
                glVertex3f(All_P1[i][j],All_Q1[i][j],0);
//            }
            glEnd();
            glFlush();
        }
    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
}

void Init()
{
  /* Set clear color to white */
  glClearColor(1.0,1.0,1.0,0);
  /* Set fill color to black */
  glColor3f(0.0,0.0,0.0);
  /* glViewport(0 , 0 , 640 , 480); */
  glMatrixMode(GL_PROJECTION);
  /* glLoadIdentity(); */
  gluOrtho2D(0.0, width , 0.0 , length);
}

//called repeatedly when glut isn't doing anything else
void idle()
{
    //redraw the scene over and over again
    glutPostRedisplay();
}

void key(unsigned char ch, int x, int y)
{
    if(ch=='s')
    {
        transformation();
         glutPostRedisplay();
    }
    //redraw the scene after keyboard input
//    glutPostRedisplay();
}

void reshape(int reshape_width, int reshape_height)
{
    /*set up projection matrix to define the view port*/
    //update the ne window width and height
    win_width = reshape_width;
    win_height = reshape_height;
    
    //creates a rendering area across the window
    glViewport(0,0,reshape_width,reshape_height);
    // up an orthogonal projection matrix so that
    // the pixel space is mapped to the grid space
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,width,0,length,-10,10);
    
    //clear the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //set pixel size based on width, if the aspect ratio
    //changes this hack won't work as well
    pixel_size = reshape_width/(float)width;
    
    //set pixel size relative to the grid cell size
    glPointSize(pixel_size);
    //check for opengl errors
}

int main(int argc, char **argv)
{
    
    char data[100];
    std::ifstream infile;
    infile.open("test_scene");
    infile>>data;
    m = atof(data);
    m = ((int)(m * 1000))/1000;
    ofstream outfile;
    outfile.open("Output_file.txt");
    outfile<<m<<'\t'<<"number of polygons"<<endl;
    printf("Enter the width and length of the window you want to create:\n");
    scanf("%d%d",&width,&length);
    printf("Enter LINE-DRAWING Method, DDA FOR 0, Bresenham for 1.\n");
    scanf("%d",&DDA_B);
    
    pixel_size = 5;
    
    win_height = length*pixel_size;
    win_width = width*pixel_size;

    p = new double*[m];
    q = new double*[m];
    Side_P1 = new double *[m];
    Side_Q1 = new double *[m];
    All_P1 = new double *[m];
    All_Q1 = new double *[m];
    SidePointNum = new int [m];
    AllPointNum = new int [m];
    GravityP = new int [m];
    GravityQ = new int [m];

    for(int i=0;i<m;i++)
        p[i] = new double[s];
    for(int i=0;i<m;i++)
        q[i] = new double[s];
    for(int i=0;i<m;i++)
        Side_P1[i] = new double[num_all_points];
    for(int i=0;i<m;i++)
        Side_Q1[i] = new double[num_all_points];
    for(int i=0;i<m;i++)
        All_P1[i] = new double[num_all_points];
    for(int i=0;i<m;i++)
        All_Q1[i] = new double[num_all_points];
    
    
    for(int i=0;i<m;i++)
    {
//        printf("\n************************************\n");
//        printf("Enter the number of sides of the polygon you want to create:\n");
//        scanf("%d",&n);
//        printf("\n************************************");
        if(i==0)
        {
            outfile<<'\t'<<"definition of 1st polygon"<<endl;
        }
        else if(i==1)
        {
            outfile<<'\t'<<"definition of 2nd polygon"<<endl;
        }
        else if(i==2)
        {
            outfile<<'\t'<<"definition of 3rd polygon"<<endl;
        }
        else
        {
            outfile<<'\t'<<"definition of "<<(i+1)<<"th polygon"<<endl;
        }
        infile>>data;
        n = atof(data);
        n = ((int)(n * 1000))/1000;
        w[i]=n;
        outfile<<n;
        if(i==0)
        {
            outfile<<'\t'<<"number of points of 1st polygon"<<endl;
        }
        else if(i==1)
        {
            outfile<<'\t'<<"number of points of 2nd polygon"<<endl;
        }
        else if(i==2)
        {
            outfile<<'\t'<<"number of points of 3rd polygon"<<endl;
        }
        else
        {
            outfile<<'\t'<<"number of points of "<<(i+1)<<"th polygon"<<endl;
        }
    //n=1 means just draw a line
        if (n==1)
        {
//            printf("\nEnter Point %d ( X %d , Y %d ):\n",1,1,1);
//            printf("\n************************************\n");
//            scanf("%lf%lf",&p[i][0],&q[i][0]);
//            printf("\nEnter Point %d ( X %d , Y %d ):\n",2,2,2);
//            printf("\n************************************\n");
//            scanf("%lf%lf",&p[i][1],&q[i][1]);
            
        }
    
    //draw the polygon
        else
        {
            int k;
            for(k=0;k<n;k++)
            {
//                printf("\nEnter Point %d ( X %d , Y %d ):\n",(k+1),(k+1),(k+1));
//                printf("\n************************************\n");
//                scanf("%lf%lf",&p[i][k],&q[i][k]);
                infile>>data;
                p[i][k] = atof(data);
                p[i][k] = ((int)(p[i][k] * 1000))/1000.0;
                infile>>data;
                q[i][k] = atof(data);
                q[i][k] = ((int)(q[i][k] * 1000))/1000.0;
                outfile<<p[i][k]<<" "<<q[i][k]<<'\t';
                if(k==0)
                {
                    outfile<<"coordinates of 1st point"<<endl;
                }
                else if(k==1)
                {
                    outfile<<"coordinates of 2nd point"<<endl;
                }
                else if(k==2)
                {
                    outfile<<"coordinates of 3rd point"<<endl;
                }
                else
                {
                    outfile<<"coordinates of "<<(k+1)<<"th point"<<endl;
                }
                
            }
        }
    }
    fillinpoly();
    
//    int i =1;
//    for(i=1;i<100;i++)
//    {
    transformation();
    
    
        
    /* Initialise GLUT library */
        glutInit(&argc,argv);
    /* Set the initial display mode */
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    /* Set the initial window position and size */
        glutInitWindowPosition(0,0);
        glutInitWindowSize(win_width,win_height);
        glutCreateWindow("Project1");
    /* Initialize drawing colors */
        Init();
    /* Call the displaying function */
        glutDisplayFunc(drawpoly);
    glutReshapeFunc(reshape);
    /* Keep displaying untill the program is closed */
    glutKeyboardFunc(key);
        glutMainLoop();
//    }
    
    for(int i=0;i<m;i++)
    {
        delete [] p[i];
        delete [] q[i];
        delete [] Side_P1[i];
        delete [] Side_Q1[i];
        delete [] All_P1[i];
        delete [] All_Q1[i];
    }
        
    delete [] p;
    delete [] q;
    delete [] Side_P1;
    delete [] Side_Q1;
    delete [] All_P1;
    delete [] All_Q1;
    delete [] SidePointNum;
    delete [] AllPointNum;
    delete [] GravityP;
    delete [] GravityQ;
    
}




