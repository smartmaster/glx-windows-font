#ifndef SML_LINUX_XLIB_H
#define SML_LINUX_XLIB_H


struct smlIGLLists
{
    virtual void create(int name) = 0;
    virtual void destroy() = 0;
    virtual void release() = 0;

    virtual int range() const = 0;
    virtual unsigned id() const = 0;
};

smlIGLLists* CreatesmlIGLLists();

struct smlIDisplay
{
    virtual void create(const char* name) = 0;
    virtual void destroy() = 0;
    virtual void release() = 0;

    virtual void loadFont(const char* name, smlIGLLists* gll) = 0;
    virtual void queryGlVersion(int* major, int* minor) = 0;
};

smlIDisplay* CreatesmlIDisplay();

#endif // SML_LINUX_XLIB_H
