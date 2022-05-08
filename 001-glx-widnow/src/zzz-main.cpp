/* gcc glxsample.c ysglfontdata.c ysglusefontbitmap.c /usr/lib/libX11.so /usr/lib/libXext.so.6 -lGLU -lGL -lm */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <memory>
#include <string>
using namespace ::std;

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xatom.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include "ysglfontdata.h"
#include "sml_linux_xlib.h"

static Display *ysXDsp;
static Window ysXWnd;
static Colormap ysXCMap;
static XVisualInfo *ysXVis;
static const int ysXEventMask = (KeyPress | KeyRelease | ButtonPress | ButtonRelease | ExposureMask | StructureNotifyMask);

static GLXContext ysGlRC;
static int ysGlxCfg[] = {/* GLX_DOUBLEBUFFER, */ GLX_RGBA, GLX_DEPTH_SIZE, 16, None};

int YsOpenWindow(int lupX, int lupY, int sizX, int sizY)
{
	const char *title = "GLX Sample";

	int n;
	char **m, *def;
	XSetWindowAttributes swa;
	Font font;

	ysXDsp = XOpenDisplay(NULL);

	if (ysXDsp != NULL)
	{
		if (glXQueryExtension(ysXDsp, NULL, NULL) != 0)
		{
			ysXVis = glXChooseVisual(ysXDsp, DefaultScreen(ysXDsp), ysGlxCfg);
			if (ysXVis != NULL)
			{
				ysXCMap = XCreateColormap(ysXDsp, RootWindow(ysXDsp, ysXVis->screen), ysXVis->visual, AllocNone);

				ysGlRC = glXCreateContext(ysXDsp, ysXVis, None, GL_TRUE);
				if (ysGlRC != NULL)
				{
					swa.colormap = ysXCMap;
					swa.border_pixel = 0;
					swa.event_mask = ysXEventMask;

					ysXWnd = XCreateWindow(ysXDsp, RootWindow(ysXDsp, ysXVis->screen),
										   lupX, lupY, sizX, sizY,
										   0,
										   ysXVis->depth,
										   InputOutput,
										   ysXVis->visual,
										   CWEventMask | CWBorderPixel | CWColormap, &swa);

					XStoreName(ysXDsp, ysXWnd, title);

					XWMHints wmHints;
					wmHints.flags = 0;
					wmHints.initial_state = NormalState;
					XSetWMHints(ysXDsp, ysXWnd, &wmHints);

					XSetIconName(ysXDsp, ysXWnd, title);
					XMapWindow(ysXDsp, ysXWnd);

					glXMakeCurrent(ysXDsp, ysXWnd, ysGlRC);

					
					glClearColor(0.5, 0.6, 0.7, 1.0);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glViewport(0, 0, sizX, sizY);
					
					glFlush();
					// glXSwapBuffers(ysXDsp,ysXWnd);
				}
				else
				{
					fprintf(stderr, "Cannot create OpenGL context.\n");
					exit(1);
				}
			}
			else
			{
				fprintf(stderr, "Double buffer not supported?\n");
				exit(1);
			}
		}
		else
		{
			fprintf(stderr, "This system doesn't support OpenGL.\n");
			exit(1);
		}
	}
	else
	{
		fprintf(stderr, "Cannot Open Display.\n");
		exit(1);
	}

	return 0;
}

int YsCloseWindow(void)
{
	XCloseDisplay(ysXDsp);
	return 0;
}

void YsSleep(int ms, bool automode)
{

	if(automode)
	{
fd_set set;
	struct timeval wait;
	wait.tv_sec = ms / 1000;
	wait.tv_usec = (ms % 1000) * 1000;
	FD_ZERO(&set);
	select(0, &set, NULL, NULL, &wait);
	}
	else
	{
printf("Press any key to cotinue..." "\n");
	getchar();
	}
}

void smlLoadFont(const char *fontName, shared_ptr<smlIGLLists>& gllo, shared_ptr<smlIDisplay>& displayo)
{
	shared_ptr<smlIGLLists> gll = shared_ptr<smlIGLLists>{CreatesmlIGLLists(),
														  [](smlIGLLists *p)
														  {
															  p->release();
														  }};


	gll->create(65536);

	shared_ptr<smlIDisplay> display = shared_ptr<smlIDisplay>{CreatesmlIDisplay(),
															  [](smlIDisplay *p)
															  {
																  p->release();
															  }};
	display->create(nullptr);
	display->loadFont(fontName, gll.get());	


	gllo = std::move(gll);
	displayo = std::move(display);
}

static void smlTestFont(const string& fontName, int index, bool pause)
{
	shared_ptr<smlIGLLists> gll;
	shared_ptr<smlIDisplay> display;

	smlLoadFont(fontName.c_str(), gll, display);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	glColor3f(0.0f, 0.9f, 0.9f);

	int hy = 20;
	glListBase(gll->id());
	glRasterPos2i(0, hy);
	string str = std::to_string(index) + ": Font Name: ";
	str += fontName;
	glCallLists(str.size(), GL_UNSIGNED_BYTE, str.c_str());

	hy += 36;
	glListBase(gll->id());
	glRasterPos2i(0, hy);
	str = "ASCII: - 012334abcxyz...567890ABCXYZ";
	glCallLists(str.size(), GL_UNSIGNED_BYTE, str.c_str());

	hy += 36;
	glListBase(gll->id());
	glRasterPos2i(0, hy);
	u16string str16 = u"DBCS: - 012334abcxyz...如何发生的核辐射...567890ABCXYZ";
	glCallLists(str16.size(), GL_SHORT, str16.c_str());

	glFlush();

	if(pause)
	{
		YsSleep(2000, true);
	}
}

extern const char* fontnames[];
extern int fontnamesCount;
static void smlTestFonts(bool pause)
{
	for(int ii =0; ii < fontnamesCount; ++ ii)
	{
		smlTestFont(fontnames[ii], ii, pause);
	}
}

static void smlMain()
{
	int viewport[4], wid, hei;

	YsOpenWindow(32, 32, 800, 600);
	glGetIntegerv(GL_VIEWPORT, viewport);
	wid = viewport[2];
	hei = viewport[3];

	glDepthFunc(GL_ALWAYS);
	glDepthMask(0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5, (GLdouble)wid - 0.5, (GLdouble)hei - 0.5, -0.5, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_CULL_FACE);

	smlTestFonts(true);


	YsCloseWindow();
	return;
}

int main(void)
{
	smlMain();
	return 0;
}

static int main_orig(void)
{
	int viewport[4], wid, hei;
	const char *str;

	//const char fontname[] = "-adobe-utopia-regular-r-normal--25-240-75-75-p-135-iso10646-1";
	//const char fontname[] = "lucidasans-bold-24";
	const char fontname[] = "olglyph-19";
	//const char fontname[] = "fixed";

	/*
	--> *225 *255  some 65534   21  13    4 -mutt-clearlyu pua-medium-r-normal--0-0-100-100-p-0-iso10646-1
--> *225 *255  some 65534   21  13    4 -mutt-clearlyu pua-medium-r-normal--17-120-100-100-p-110-iso10646-1
	*/
//const char fontname[] = "-mutt-clearlyu pua-medium-r-normal--0-0-100-100-p-0-iso10646-1";
//const char fontname[] = "-mutt-clearlyu pua-medium-r-normal--17-120-100-100-p-110-iso10646-1";
	shared_ptr<smlIGLLists> gll;
	shared_ptr<smlIDisplay> display;
	


	YsOpenWindow(32, 32, 800, 600);

	smlLoadFont(fontname, gll, display);

	// YsGlUseFontBitmap6x7(0x400);
	// YsGlUseFontBitmap6x8(0x500);
	// YsGlUseFontBitmap8x8(0x600);
	// YsGlUseFontBitmap8x12(0x700);
	// YsGlUseFontBitmap12x16(0x800);
	// YsGlUseFontBitmap16x20(0x900);
	// YsGlUseFontBitmap16x24(0xA00);
	// YsGlUseFontBitmap20x28(0xB00);
	// YsGlUseFontBitmap20x32(0xC00);

	glGetIntegerv(GL_VIEWPORT, viewport);
	wid = viewport[2];
	hei = viewport[3];

	glDepthFunc(GL_ALWAYS);
	glDepthMask(0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5, (GLdouble)wid - 0.5, (GLdouble)hei - 0.5, -0.5, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_CULL_FACE);

	glColor3f(0.0f, 0.9f, 0.0f);

	int height = 20;
	//glListBase(0x400);
	glListBase(gll->id());
	glRasterPos2i(0, height);
	str = "BYTE - ABCDEFG012345";
	glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);

	{
	height += 30;
	glListBase(gll->id());
	glRasterPos2i(0, height);
	char16_t str2[] = u"SHORT - ABCDEFG012345";
	glCallLists(sizeof(str2)/sizeof(str2[0]) - 1, GL_SHORT, str2);
	}
	
	{
	height += 30;
	glListBase(gll->id());
	glRasterPos2i(0, height);
	char16_t str2[] = u"SHORT - ABCDEFG分手快乐敌方角色012345";
	glCallLists(sizeof(str2)/sizeof(str2[0]) - 1, GL_SHORT, str2);
	}

	glFlush();

	YsSleep(10000, false);
	YsCloseWindow();

	return 0;
}
