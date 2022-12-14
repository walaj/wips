#ifndef TIFF_IMAGE_H
#define TIFF_IMAGE_H

#include <string>
#include <cassert>
#include <vector>
#include <tiffio.h>
#include <sstream>

#include "tiff_cp.h"
#include "tiff_utils.h"
#include "cell.h"

#define PIXEL_GRAY 999
#define PIXEL_RED 0
#define PIXEL_GREEN 1
#define PIXEL_BLUE 2
#define PIXEL_ALPHA 3

/*
#include <random>
int getRandomInt() {
    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    // Set the range of the generator to [0, 255]
    std::uniform_int_distribution<> dis(0, 255);

    // Generate and return a random integer
    return dis(gen);
    }*/

// copy tags from one TIFF to another
static int TiffTagCopy(TIFF* o, TIFF* d);

class TiffImage {

 public:

  // create a new image from a TIFF (libtiff)
  TiffImage(TIFF* tif);

  // create an empty iamge
  TiffImage() {}

  ~TiffImage() {
    clear_raster();
  }
  
  // access the data pointer
  void* data() const { return m_data; }

  // read the image tif to a 2D raster
  int ReadToRaster(TIFF *tif);

  // access a pixel as type T (uint8_t or uint32_t)
  template <typename T>  
  T pixel(uint64_t x, uint64_t y, int p) const;

  // access a pixel as type T (uint8_t or uint32_t) from the flattened index
  template <typename T>  
  T element(uint64_t e) const;

  // write the raster to the image using an already opened TIFF
  int write(TIFF* otif) const;

  // clear the raster and free the memory
  void clear_raster();

  // mean pixel value
  double mean(TIFF* tiff) const;

  // take three tiled images and convert to single RBG
  int MergeGrayToRGB(TIFF* r, TIFF* g, TIFF* b, TIFF* o);

  // take a three IFD image and convert to single RBG
  int MergeGrayToRGB(TIFF* in, TIFF* out) const;
  
  /// return the total number of pixels
  uint64_t numPixels() const { return m_pixels; }

  // print the mean number of pixels to std, for each channel
  int light_mean(TIFF* tif) const;

  // set the verbose output flag
  void setverbose(bool v) { verbose = v; }

  // return the number of directories
  // note that this resets the current directory to 0
  int DirCount(TIFF* in) const;

  // draw circles
  int DrawCircles(TIFF* out, const CellTable& table);
  
 private:

  bool verbose= false;
  
  // tif to read from
  TIFF* m_tif = NULL; 

  // raster of the image
  void* m_data = NULL; 

  size_t m_current_dir = 0;
  
  // flag for whether data is stored as 1 byte or 4 bytes
  uint32_t m_width = 0, m_height = 0;

  // total number of pixels in the image
  uint64_t m_pixels = 0;

  // various TIFF flags
  uint32_t m_tilewidth, m_tileheight;
  uint16_t m_photometric, m_planar, m_bits_per_sample, m_samples_per_pixel;
  uint16_t m_extra_samples;

  // allocate the memory for the image raster
  int __alloc(TIFF *tif);

  // sub method to ReadToRaster for tiled image
  int __tiled_read(TIFF *tif);

  // sub method to ReadToRaster for scanline image
  int __lined_read(TIFF* i_tif);
  
  // pass a new TIFF file
  int __give_tiff(TIFF *tif);

  // write the raster to a tiled image
  int __tiled_write(TIFF* otif) const;

  // write the raster to a lined image
  int __lined_write(TIFF* otif) const;
  
  // has the image had its raster stored to memory
  bool __is_rasterized() const;

  // has the image been assciated with a TIFF
  bool __is_initialized() const;

  // get the mode (gray 8-bit, RBG etc)
  size_t __get_mode(TIFF* tif) const;

  // check that the TIFF is valid pointer
  int __check_tif(TIFF* tif) const;

  int __tileind(TIFF* tif, uint32_t x, uint32_t y) const;

  // assert that an image is 8-bit and grayscale
  void __gray8assert(TIFF* in) const;
  
};

#endif
