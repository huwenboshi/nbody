/*
*  OpenGL Textured Cube
*  http://talkera.org.cp-in-1.webhostbox.net/opengl/  
*  http://openglsamples.sf.net
*
*  Compilation trough:
*     gcc cube2.c -lglut -lGL -lGLU -lSDL -lSDL_image
*
* Note lSDL_image is added for PNG support!
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL/SDL.h"
#include "SDL_image.h"

/* screen width, height, and bit depth */
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP     16

/* Setup useful booleans */
#define TRUE  1
#define FALSE 0

/* This is our SDL surface */
SDL_Surface *surface;

/* Used for rotating the cube */
GLfloat rotationXaxis; 
GLfloat rotationYaxis; 
GLfloat rotationZaxis; 

/* Storage For Textures */
GLuint texture[3]; 

/* Release resources and quit  */
void Quit( int returnCode ) {
    SDL_Quit( );
    exit( returnCode );
}

int NR_STARS = 1024;
float starx[1024],stary[1024],starz[1024];


/* Loads in a bitmap as a GL texture */
int LoadGLTextures( ) {
    int Status = FALSE;

    /* Create storage space for the texture */
    SDL_Surface *TextureImage[1]; 

    /* Load The Bitmap into Memory */
    if ((TextureImage[0] = IMG_Load("data/fire2.jpg"))) {
	    Status = TRUE;
	    glGenTextures( 1, &texture[0] );
	    glBindTexture( GL_TEXTURE_2D, texture[0] );
	    glTexImage2D( GL_TEXTURE_2D, 0, 3, TextureImage[0]->w,
			  TextureImage[0]->h, 0, GL_RGB,
			  GL_UNSIGNED_BYTE, TextureImage[0]->pixels );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        }

    /* Free up some memory */
    if ( TextureImage[0] )
	    SDL_FreeSurface( TextureImage[0] );


    // second texture
    /* Load The Bitmap into Memory */
    if ((TextureImage[0] = IMG_Load("data/fire.jpg"))) {
	    Status = TRUE;
	    glGenTextures( 1, &texture[1] );
	    glBindTexture( GL_TEXTURE_2D, texture[1] );
	    glTexImage2D( GL_TEXTURE_2D, 0, 3, TextureImage[0]->w,
			  TextureImage[0]->h, 0, GL_RGB,
			  GL_UNSIGNED_BYTE, TextureImage[0]->pixels );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        }

    /* Free up some memory */
    if ( TextureImage[0] )
	    SDL_FreeSurface( TextureImage[0] );

    return Status;
}

/* function to reset our viewport after a window resize */
int resizeWindow( int width, int height ) {
    /* Height / width ration */
    GLfloat ratio;
 
    /* Protect against a divide by zero */
    if ( height == 0 )
	height = 1;

    ratio = ( GLfloat )width / ( GLfloat )height;

    /* Setup our viewport. */
    glViewport( 0, 0, ( GLint )width, ( GLint )height );

    /*
     * change to the projection matrix and set
     * our viewing volume.
     */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    /* Set our perspective */
    gluPerspective( 45.0f, ratio, 0.1f, 100.0f );

    /* Make sure we're chaning the model view and not the projection */
    glMatrixMode( GL_MODELVIEW );

    /* Reset The View */
    glLoadIdentity( );

    return( TRUE );
}

/* function to handle key press events */
void handleKeyPress( SDL_keysym *keysym )
{
    switch ( keysym->sym )
	{
 	case SDLK_ESCAPE:
	    /* ESC key was pressed */
	    Quit( 0 );
	    break;
	case SDLK_F1:
	    /* F1 key was pressed
	     * this toggles fullscreen mode
	     */
	    SDL_WM_ToggleFullScreen( surface );
	    break;
	default:
	    break;
	}

    return;
}

/* OpenGL initialization function */
int initGL( GLvoid )
{

    /* Load in the texture */
    if ( !LoadGLTextures( ) )
	return FALSE;

    /* Enable Texture Mapping ( NEW ) */
    glEnable( GL_TEXTURE_2D );

    /* Enable smooth shading */
    glShadeModel( GL_SMOOTH );

    /* Set the background black */
    glClearColor( 0.0f, 0.0f, 0.0f, 0.5f );

    /* Depth buffer setup */
    glClearDepth( 1.0f );

    /* Enables Depth Testing */
    glEnable( GL_DEPTH_TEST );

    /* The Type Of Depth Test To Do */
    glDepthFunc( GL_LEQUAL );

    /* Really Nice Perspective Calculations */
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

    return( TRUE );
}

void initStars() {
  int i = 0;
  for (i = 0; i < NR_STARS; i++) {
    starx[i] = 0.1 * (rand() % 200 + 1);   
    stary[i] = 0.1 * (rand() % 200 + 1);   
    starz[i] = - ( 0.1 * (rand() % 3000 + 1));   
  
    stary[i] -= 5;
    starx[i] -= 10;
    printf("%f %f %f\n",starx[i],stary[i],starz[i]);
  }

}


float scale = 4.0f;


void drawStar() {

    glBegin(GL_QUADS);
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1.0f, -1.0f, 1.0f );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  1.0f, -1.0f, 1.0f );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  1.0f,  1.0f, 1.0f );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1.0f,  1.0f, 1.0f );
    glEnd( );
}

/* Here goes our drawing code */
int drawGLScene( GLvoid )
{
    /* Clear The Screen And The Depth Buffer */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* Move Into The Screen 5 Units, 2 left. */
    glLoadIdentity( );
    glTranslatef( -2.0f, -5.0f, -5.0f );

   // glRotatef( rotationXaxis, 1.0f, 0.0f, 0.0f); /* Rotate On The X Axis */
  //  glRotatef( rotationYaxis, 0.0f, 1.0f, 0.0f); /* Rotate On The Y Axis */
   // glRotatef( rotationZaxis, 0.0f, 0.0f, 1.0f); /* Rotate On The Z Axis */

    /* Select Our Texture */
    glBindTexture( GL_TEXTURE_2D, texture[1] );
//    glScalef(scale,scale,scale);

    // turn on alpha blending for transparency
    glEnable(GL_BLEND);     // Turn Blending On
    glDisable(GL_DEPTH_TEST);   // Turn Depth Testing Off
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);   
 
 
    int i = 0;
    for (i = 0; i < NR_STARS; i++) {
     glPushMatrix();
      glTranslatef( starx[i],stary[i],starz[i] );
      drawStar();
      glPopMatrix();

      starz[i] += 0.15f;
    }

//    drawStar();

    /* Draw it to the screen */
    SDL_GL_SwapBuffers( );

    /* Rotate  */
    rotationXaxis += 0.3f; 
    rotationYaxis += 0.2f;
    rotationZaxis += 0.2f;

    return( TRUE );
}

/* http://talkera.org.cp-in-1.webhostbox.net/opengl/ */
int main( int argc, char **argv )
{
    /* Flags to pass to SDL_SetVideoMode */
    int videoFlags;
    /* main loop variable */
    int done = FALSE;
    /* used to collect events */
    SDL_Event event;
    /* this holds some info about our display */
    const SDL_VideoInfo *videoInfo;
    /* whether or not the window is active */
    int isActive = TRUE;

    /* initialize SDL */
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
	    fprintf( stderr, "Video initialization failed: %s\n",
		     SDL_GetError( ) );
	    Quit( 1 );
	}

    /* Fetch the video info */
    videoInfo = SDL_GetVideoInfo( );

    if ( !videoInfo )
	{
	    fprintf( stderr, "Video query failed: %s\n",
		     SDL_GetError( ) );
	    Quit( 1 );
	}

    /* the flags to pass to SDL_SetVideoMode */
    videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */

    /* This checks to see if surfaces can be stored in memory */
    if ( videoInfo->hw_available )
	videoFlags |= SDL_HWSURFACE;
    else
	videoFlags |= SDL_SWSURFACE;

    /* This checks if hardware blits can be done */
    if ( videoInfo->blit_hw )
	videoFlags |= SDL_HWACCEL;

    /* Sets up OpenGL double buffering */
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    /* get a SDL surface */
    surface = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
				videoFlags );

    /* Verify there is a surface */
    if ( !surface )
	{
	    fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
	    Quit( 1 );
	}

    /* initialize OpenGL */
    initGL( );

    initStars();

    /* resize the initial window */
    resizeWindow( SCREEN_WIDTH, SCREEN_HEIGHT );

    /* wait for events */
    while ( !done )
	{
	    /* handle the events in the queue */

	    while ( SDL_PollEvent( &event ) )
		{
		    switch( event.type )
			{
			case SDL_ACTIVEEVENT:
			    if ( event.active.gain == 0 )
				isActive = FALSE;
			    else
				isActive = TRUE;
			    break;			    
			case SDL_VIDEORESIZE:
			    /* handle resize event */
			    surface = SDL_SetVideoMode( event.resize.w,
							event.resize.h,
							16, videoFlags );
			    if ( !surface )
				{
				    fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError( ) );
				    Quit( 1 );
				}
			    resizeWindow( event.resize.w, event.resize.h );
			    break;
			case SDL_KEYDOWN:
			    /* handle key presses */
			    handleKeyPress( &event.key.keysym );
			   
			    break;
			case SDL_QUIT:
			    /* handle quit requests */
			    done = TRUE;
			    break;
			default:
			    break;
			}
		}

	    /* draw the scene */
	    if ( isActive )
		drawGLScene( );
	}

    /* clean ourselves up and exit */
    Quit( 0 );

    /* Should never get here */
    return( 0 );
}
