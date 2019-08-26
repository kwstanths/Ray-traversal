#ifndef _TEXTURE_INCLUDE
#define _TEXTURE_INCLUDE


#include <string>
#include <GL/glew.h>


using namespace std;


enum PixelFormat {TEXTURE_PIXEL_FORMAT_RGB, TEXTURE_PIXEL_FORMAT_RGBA};


// The texture class loads images an passes them to OpenGL
// storing the returned id so that it may be applied to any drawn primitives


class Texture
{

public:
	Texture();

	bool loadFromFile(const string &filename, PixelFormat format);
	void loadFromGlyphBuffer(unsigned char *buffer, int width, int height);

	void createEmptyTexture(int width, int height);
	void loadSubtextureFromGlyphBuffer(unsigned char *buffer, int x, int y, int width, int height);
	void generateMipmap();
	
	void setWrapS(GLint value);
	void setWrapT(GLint value);
	void setMinFilter(GLint value);
	void setMagFilter(GLint value);
	
	void use() const;

private:
	GLuint texId;
	GLint wrapS, wrapT, minFilter, magFilter;

};


#endif // _TEXTURE_INCLUDE

