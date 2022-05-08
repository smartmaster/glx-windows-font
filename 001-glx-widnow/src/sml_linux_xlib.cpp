#include "sml_linux_xlib.h"

#include <GL/glx.h>

namespace
{
class smlGLLists : public smlIGLLists
{
private:
    int _range{0};
    GLuint _gllist{0};

    // smlIGLLists interface
public:
    virtual void create(int range) override
    {
        _range = range;
        _gllist = glGenLists(range);
    }
    virtual void destroy() override
    {
        if(_range)
        {
            glDeleteLists(_gllist, _range);
            _range = 0;
        }
    }
    virtual void release() override
    {
        destroy();
        delete this;
    }
    virtual int range() const override
    {
        return _range;
    }
    virtual unsigned id() const override
    {
        return _gllist;
    }
};
}

smlIGLLists* CreatesmlIGLLists()
{
    return new smlGLLists{};
}


namespace{
class smlDisplay : public smlIDisplay
{
private:
    Display* _xdisplay{nullptr};

    // smlIDisplay interface
public:
    virtual void create(const char *name) override
    {
        _xdisplay = XOpenDisplay(name);
    }
    virtual void destroy() override
    {
        if(_xdisplay)
        {
            XCloseDisplay(_xdisplay);
            _xdisplay = nullptr;
        }
    }
    virtual void release() override
    {
        destroy();
        delete this;
    }
    virtual void loadFont(const char *name, smlIGLLists *gll) override
    {
        Display* current =  glXGetCurrentDisplay();
        if(nullptr == current)
        {
            current = _xdisplay;
        }
        XFontStruct* xfs = XLoadQueryFont(current, name);
        int firstFont = (int)xfs->min_byte1 * 256 + (int)xfs->min_char_or_byte2;
        int lastFont = (int)xfs->max_byte1 * 256 + (int)xfs->max_char_or_byte2;
        glXUseXFont(xfs->fid, firstFont, lastFont - firstFont + 1, gll->id() + firstFont);
        //glXUseXFont(xfs->fid, 32, 96, gll->id() + 32);
        //      for(int ii = 0; ii < gll->range(); ++ ii)
        //      {
        //          glXUseXFont(xfs->fid, ii, 1, gll->id() + ii);
        //      }

        XFreeFont(current, xfs);
    }

    virtual void queryGlVersion(int *major, int *minor) override
    {
        glXQueryVersion(_xdisplay, major, minor);
    }
};
}


smlIDisplay* CreatesmlIDisplay()
{
    return new smlDisplay{};
}
