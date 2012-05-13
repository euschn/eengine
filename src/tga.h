//Taken from NeHe tutorial 33


#ifndef __TEXTURE_H__				// See If The Header Has Been Defined Yet
#define __TEXTURE_H__				// If Not, Define It.

// disable visual studio warning
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>				// Standard Header For File I/O
#include <GL3/gl3w.h>				// Standard Header For OpenGL

namespace tga{

typedef struct
{
        GLubyte* imageData;			// Hold All The Color Values For The Image.
        GLuint  bpp;				// Hold The Number Of Bits Per Pixel.
        GLuint width;				// The Width Of The Entire Image.
        GLuint height;				// The Height Of The Entire Image.
        GLuint texID;				// Texture ID For Use With glBindTexture.
        GLuint type;			 	// Data Stored In * ImageData (GL_RGB Or GL_RGBA)
} Texture;

typedef struct
{
        GLubyte Header[12];			// File Header To Determine File Type
} TGAHeader;

typedef struct
{
        GLubyte header[6];			// Holds The First 6 Useful Bytes Of The File
        GLuint bytesPerPixel;			// Number Of BYTES Per Pixel (3 Or 4)
        GLuint imageSize;			// Amount Of Memory Needed To Hold The Image
        GLuint type;				// The Type Of Image, GL_RGB Or GL_RGBA
        GLuint Height;				// Height Of Image
        GLuint Width;				// Width Of Image
        GLuint Bpp;				// Number Of BITS Per Pixel (24 Or 32)
} TGA;



bool LoadTGA(Texture * texture, const char * filename);
// Load An Uncompressed File
bool LoadUncompressedTGA(Texture *, const char *, FILE *, tga::TGAHeader&, tga::TGA&);
// Load A Compressed File
bool LoadCompressedTGA(Texture *, const char *, FILE *, tga::TGAHeader&, tga::TGA&);

}
#endif
