/*
 * Copyright (c) 2007 University of Michigan, Ann Arbor.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of Michigan, Ann Arbor. The name of the University 
 * may not be used to endorse or promote products derived from this 
 * software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Author: Igor Guskov
 *
 */
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
using namespace std;

#include <png.h>
#include <jpeglib.h>
#include "image.h"

// Simple image class for texture data storage.

Image::Image(int width, int height, int channels) 
  : width_(width),
    height_(height),
    channels_(channels),
    pixels_(NULL)
{
  pixels_ = new unsigned char[width * height * channels];
}

Image::~Image() {
  delete[] pixels_;
}

// JPEG helper structures and functions.

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

Image*
ImageIO::LoadJPEG(const char* filename)
{
  struct jpeg_decompress_struct cinfo;
  /* We use our private extension JPEG error handler.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct my_error_mgr jerr;
  /* More stuff */
  FILE * infile;                /* source file */
  JSAMPARRAY buffer;            /* Output row buffer */
  int row_stride;               /* physical row width in output buffer */

  /* In this example we want to open the input file before doing anything else,
   * so that the setjmp() error recovery below can assume the file is open.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to read binary files.
   */

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return NULL;
  }

  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return NULL;
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

  (void) jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  cerr << "JPEG reader:" << endl;
  cerr << cinfo.output_components << " components." << endl;
  cerr << cinfo.output_width << " wide and " << cinfo.output_height << " high." << endl;

  if (cinfo.output_components!=3) {
    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return NULL;
  }

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
  Image* image = new Image(cinfo.output_width, cinfo.output_height, 
                           cinfo.output_components);

  int rowcount = 0;
  while (cinfo.output_scanline < cinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
    memcpy(image->mutable_row(cinfo.output_height-1-rowcount), buffer[0], 
           cinfo.output_width * cinfo.output_components);
    //xim.at(k, cinfo.output_height-1-rowcount).x() = buffer[0][3*k]/255.0f;
    ++rowcount;
  }

  /* Step 7: Finish decompression */

  (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */
  fclose(infile);

  return image;
}

Image*
ImageIO::LoadPNG(const char* filename)
{
  png_structp png_ptr;
  png_infop info_ptr;
  unsigned int sig_read = 0;
  FILE *fp;

  if ((fp = fopen(filename, "rb")) == NULL) {
    return NULL;
  }

  /* Create and initialize the png_struct with the desired error handler
   * functions.  If you want to use the default stderr and longjump method,
   * you can supply NULL for the last three parameters.  We also supply the
   * the compiler header file version, so that we know if the application
   * was compiled with a compatible version of the library.  REQUIRED
   */
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                   NULL, NULL, NULL);

  if (png_ptr == NULL) {
    fclose(fp);
    return NULL;
  }

  /* Allocate/initialize the memory for image information.  REQUIRED. */
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    fclose(fp);
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return NULL;
  }

  /* Set error handling if you are using the setjmp/longjmp method (this is
   * the normal method of doing things with libpng).  REQUIRED unless you
   * set up your own error handlers in the png_create_read_struct() earlier.
   */

  if (setjmp(png_jmpbuf(png_ptr))) {
    /* Free all of the memory associated with the png_ptr and info_ptr */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    /* If we get here, we had a problem reading the file */
    return NULL;
  }

  /* Set up the input control if you are using standard C streams */
  png_init_io(png_ptr, fp);

  /* If we have already read some of the signature */
  png_set_sig_bytes(png_ptr, sig_read);

  /*
   * If you have enough memory to read in the entire image at once,
   * and you need to specify only transforms that can be controlled
   * with one of the PNG_TRANSFORM_* bits (this presently excludes
   * dithering, filling, setting background, and doing gamma
   * adjustment), then you can read the entire image (including
   * pixels) into the info structure with this call:
   */
  //png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_ALPHA, NULL);
  png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  /* At this point you have read the entire image */

  cerr << "png read: " << png_get_image_width(png_ptr, info_ptr) << "x"
       << png_get_image_height(png_ptr, info_ptr) << endl;
  cerr << "channels: " << (unsigned int) png_get_channels(png_ptr, info_ptr)
       << ", bit_depth = " << (unsigned int) png_get_bit_depth(png_ptr, info_ptr)
       << endl;

  png_bytep *row_pointers = new png_bytep[png_get_image_height(png_ptr, info_ptr)];
  row_pointers = png_get_rows(png_ptr, info_ptr);

  if (png_get_bit_depth(png_ptr, info_ptr) !=8) {
    cerr << "not eight" << endl;
    return NULL;
  }

  Image* image = new Image(png_get_image_width(png_ptr, info_ptr),
                           png_get_image_height(png_ptr, info_ptr),
                           png_get_channels(png_ptr, info_ptr));
  
  for (int row=0; row<(int)png_get_image_height(png_ptr,info_ptr); ++row) {
    memcpy(image->mutable_row(png_get_image_height(png_ptr,info_ptr) - 1 - row),
           row_pointers[row], 
           png_get_image_width(png_ptr, info_ptr)*
           png_get_channels(png_ptr, info_ptr));
    //xim.at(col, info_ptr->height-1-row)(0) = float(row_pointers[row][3*col])/255.0f;
  }

  /* clean up after the read, and free any memory allocated - REQUIRED */
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

  /* close the file */
  fclose(fp);

  return image;
}

ImageIO::ImageFormatEnum
ImageIO::recognize_image_format(const char* filename)
{
  char png_sig[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
  char jpeg_sig[] = { 0xFF, 0xD8 };

  ifstream ist(filename, ios::binary);
  if (!ist.good()) {
    cerr << "WARNING: cannot open file " << filename 
         << " for reading" << endl;
    return FORMAT_NONE;
  }
  char sig[8];
  ist.read(sig, 8);
  if (ist.gcount()!=8) {
    cerr << "read " << ist.gcount() << " chars." << endl;
    return FORMAT_NONE;
  }
  if (memcmp(sig, png_sig, sizeof(png_sig))==0) {
    return FORMAT_PNG;
  } else if (memcmp(sig, jpeg_sig, sizeof(jpeg_sig))==0) {
    return FORMAT_JPEG;
  }
  return FORMAT_NONE;
}
