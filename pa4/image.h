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
#ifndef __IMAGE_H__
#define __IMAGE_H__

class Image {
 public:
  Image(int width, int height, int channels);
  ~Image();
  unsigned char* get_pixels() const {
    return pixels_;
  }
  unsigned char* mutable_row(int row) {
    return pixels_ + channels_ * width_ * row;
  }
  int width() const {
    return width_;
  }
  int height() const {
    return height_;
  }
  int channels() const {
    return channels_;
  }
  bool hasAlpha() const {
    return channels_==4;
  }
 private:
  int width_, height_;
  int channels_; // number of channels: 3 for RGB images, 4 for RGBA images
  unsigned char* pixels_;
};

class ImageIO {
 public:

  enum ImageFormatEnum {
    FORMAT_NONE = -1,
    FORMAT_PNG = 0,
    FORMAT_JPEG = 1
  };

  static Image* LoadJPEG(const char* filename);
  static Image* LoadPNG(const char* filename);
  static ImageFormatEnum recognize_image_format(const char* filename);
};

#endif  // __IMAGE_H__
