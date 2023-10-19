#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
class GSCImage;

/******************** PIXEL CLASS ********************/
class Pixel {
public:
  virtual ~Pixel() = default;
};
/******************** END PIXEL CLASS ********************/

/******************** GSCPIXEL CLASS ********************/
class GSCPixel : public Pixel {
private:
  unsigned char value_;

public:
  GSCPixel() = default;
  GSCPixel(const GSCPixel &p) : value_(p.value_) {}
  GSCPixel(unsigned char value) : value_(value) {}

  unsigned char getValue() { return value_; }

  void setValue(unsigned char value) { value_ = value; }
};
/******************** END GSCIMAGE CLASS ********************/

/******************** RGBPIXEL CLASS ********************/
class RGBPixel : public Pixel {
private:
  unsigned char red_;
  unsigned char green_;
  unsigned char blue_;

public:
  RGBPixel() = default;
  RGBPixel(const RGBPixel &p)
      : red_(p.red_), green_(p.green_), blue_(p.blue_) {}
  RGBPixel(unsigned char r, unsigned char g, unsigned char b)
      : red_(r), green_(g), blue_(b) {}

  unsigned char getRed() const { return red_; }

  unsigned char getGreen() const { return green_; }

  unsigned char getBlue() const { return blue_; }

  void setRed(unsigned char r) { red_ = r; }

  void setGreen(unsigned char g) { green_ = g; }

  void setBlue(unsigned char b) { blue_ = b; }
};
/******************** END RGBPIXEL CLASS********************/

/******************** IMAGE CLASS ********************/
class Image {
protected:
  int width;
  int height;
  int max_luminocity;

public:
  int getWidth() const { return width; }
  int getHeight() const { return height; }
  int getMaxLuminocity() const { return max_luminocity; }
  void setWidth(int width) { this->width = width; }
  void setHeight(int height) { this->height = height; }
  void setMaxLuminocity(int lum) { this->max_luminocity = lum; }

  virtual Pixel &getPixel(int row, int col) const = 0;
  virtual Image &operator+=(int times) = 0;
  virtual Image &operator*=(double factor) = 0;
  virtual Image &operator!() = 0;
  virtual Image &operator~() = 0;
  virtual Image &operator*() = 0;
  friend std::ostream &operator<<(std::ostream &out, Image &image);
  virtual ~Image() = default;
};
/******************** END IMAGE CLASS ********************/

/******************** RGBImage CLASS ********************/
class RGBImage : public Image {
private:
  RGBPixel **pixels;

public:
  RGBImage() : pixels(nullptr) {
    // Initialize the class fields
    width = 0;
    height = 0;
    max_luminocity = 255;
    pixels = nullptr;
  }

  RGBImage(const RGBImage &img) : pixels(nullptr) {
    width = img.getWidth();
    height = img.getHeight();
    max_luminocity = img.getMaxLuminocity();

    // Allocate memory for pixels
    pixels = new RGBPixel *[height];
    for (int row = 0; row < height; row++) {
      pixels[row] = new RGBPixel[width];
      for (int col = 0; col < width; col++) {
        pixels[row][col] = img.pixels[row][col];
      }
    }
  }
  RGBImage(const GSCImage &gsc);

  RGBImage(int Width, int Height) : pixels(nullptr) {
    width = Width;
    height = Height;
    max_luminocity = 255;

    // Allocate memory for pixels
    pixels = new RGBPixel *[height];
    for (int row = 0; row < height; row++) {
      pixels[row] = new RGBPixel[width];
      for (int col = 0; col < width; col++) {
        pixels[row][col] = RGBPixel();
      }
    }
  }

  RGBImage(std::istream &stream) : pixels(nullptr) {
    stream >> width >> height >> max_luminocity;

    // Allocate memory for pixels
    pixels = new RGBPixel *[height];
    for (int row = 0; row < height; row++) {
      pixels[row] = new RGBPixel[width];
      for (int col = 0; col < width; col++) {
        int red, green, blue;
        stream >> red >> green >> blue;
        pixels[row][col] = RGBPixel(static_cast<unsigned char>(red),
                                    static_cast<unsigned char>(green),
                                    static_cast<unsigned char>(blue));
      }
    }
  }

  virtual RGBPixel &getPixel(int row, int col) const override {
    if (pixels == nullptr) {
      throw std::runtime_error("Image is not initialized.");
    }

    if (row < 0 || row >= height || col < 0 || col >= width) {
      throw std::out_of_range("Invalid pixel coordinates.");
    }

    return pixels[row][col];
  }

  RGBImage &operator=(const RGBImage &img) {
    if (this == &img) {
      // Self-assignment check
      return *this;
    }

    // Free existing memory
    if (pixels != nullptr) {
      for (int row = 0; row < height; row++) {
        delete[] pixels[row];
      }
      delete[] pixels;
      pixels = nullptr;
    }

    // Assign new dimensions and maximum luminosity
    width = img.width;
    height = img.height;
    max_luminocity = img.max_luminocity;

    // Allocate new memory and copy pixels
    pixels = new RGBPixel *[height];
    for (int row = 0; row < height; row++) {
      pixels[row] = new RGBPixel[width];
      for (int col = 0; col < width; col++) {
        pixels[row][col] = img.getPixel(row, col);
      }
    }

    return *this;
  }

  virtual Image &operator+=(int times) override {
    // Calculate the effective rotation times
    int effectiveTimes = times % 4;
    if (effectiveTimes < 0)
      effectiveTimes += 4;

    // Rotate the image clockwise
    for (int i = 0; i < effectiveTimes; i++) {
      // Create a temporary image to store the rotated pixels
      RGBImage rotatedImage;
      rotatedImage.setWidth(getHeight());
      rotatedImage.setHeight(getWidth());
      rotatedImage.setMaxLuminocity(255);
      rotatedImage.pixels = new RGBPixel *[rotatedImage.getHeight()];
      for (int row = 0; row < rotatedImage.getHeight(); row++) {
        rotatedImage.pixels[row] = new RGBPixel[rotatedImage.getWidth()];
        for (int col = 0; col < rotatedImage.getWidth(); col++) {
          rotatedImage.pixels[row][col] = RGBPixel();
        }
      }

      // Perform the rotation by rearranging the pixels
      for (int row = 0; row < getHeight(); row++) {
        for (int col = 0; col < getWidth(); col++) {
          // Get the pixel from the original image
          RGBPixel &originalPixel = getPixel(row, col);

          // Calculate the new coordinates for the rotated pixel
          int rotatedRow = col;
          int rotatedCol = getHeight() - 1 - row;

          // Get the pixel from the rotated image and assign theoriginal pixel
          // value
          RGBPixel &rotatedPixel =
              rotatedImage.getPixel(rotatedRow, rotatedCol);
          rotatedPixel = originalPixel;
        }
      }

      // Assign the rotated image to the current image
      *this = rotatedImage;
      if (rotatedImage.pixels != nullptr) {
        for (int row = 0; row < rotatedImage.getHeight(); row++) {
          delete[] rotatedImage.pixels[row];
        }
        delete[] rotatedImage.pixels;
        rotatedImage.pixels = nullptr;
      }
    }

    return *this;
  }

  virtual Image &operator*=(double factor) override {
    // Calculate the new dimensions based on the factor
    int newWidth = static_cast<int>(getWidth() * factor);
    int newHeight = static_cast<int>(getHeight() * factor);

    // Create a temporary image with the new dimensions
    RGBImage resizedImage;
    resizedImage.setWidth(newWidth);
    resizedImage.setHeight(newHeight);
    resizedImage.setMaxLuminocity(255);
    resizedImage.pixels = new RGBPixel *[resizedImage.getHeight()];
    for (int row = 0; row < resizedImage.getHeight(); row++) {
      resizedImage.pixels[row] = new RGBPixel[resizedImage.getWidth()];
      for (int col = 0; col < resizedImage.getWidth(); col++) {
        resizedImage.pixels[row][col] = RGBPixel();
      }
    }

    // Scale the pixels from the original image to the resized image
    for (int row = 0; row < newHeight; row++) {
      for (int col = 0; col < newWidth; col++) {
        // Calculate the corresponding coordinates in the original image
        int r1 = std::min(static_cast<int>(std::floor(row / factor)),
                          getHeight() - 1);
        int r2 = std::min(static_cast<int>(std::ceil(row / factor)),
                          getHeight() - 1);
        int c1 = std::min(static_cast<int>(std::floor(col / factor)),
                          getWidth() - 1);
        int c2 =
            std::min(static_cast<int>(std::ceil(col / factor)), getWidth() - 1);

        // Get the pixels from the original image
        RGBPixel &p11 = getPixel(r1, c1);
        RGBPixel &p12 = getPixel(r1, c2);
        RGBPixel &p21 = getPixel(r2, c1);
        RGBPixel &p22 = getPixel(r2, c2);

        // Calculate the average brightness
        unsigned char avgRedBrightness =
            (p11.getRed() + p12.getRed() + p21.getRed() + p22.getRed()) / 4;
        unsigned char avgGreenBrightness = (p11.getGreen() + p12.getGreen() +
                                            p21.getGreen() + p22.getGreen()) /
                                           4;
        unsigned char avgBlueBrightness =
            (p11.getBlue() + p12.getBlue() + p21.getBlue() + p22.getBlue()) / 4;

        // Set the new pixel in the resized image
        resizedImage.getPixel(row, col).setRed(avgRedBrightness);
        resizedImage.getPixel(row, col).setGreen(avgGreenBrightness);
        resizedImage.getPixel(row, col).setBlue(avgBlueBrightness);
      }
    }

    // Assign the resized image to the current image
    *this = resizedImage;
    if (resizedImage.pixels != nullptr) {
      for (int row = 0; row < resizedImage.getHeight(); row++) {
        delete[] resizedImage.pixels[row];
      }
      delete[] resizedImage.pixels;
      resizedImage.pixels = nullptr;
    }
    return *this;
  }

  virtual Image &operator!() override {
    if (pixels == nullptr) {
      // Image is empty, nothing to invert
      return *this;
    }

    for (int row = 0; row < height; row++) {
      for (int col = 0; col < width; col++) {
        RGBPixel &pixel = getPixel(row, col);
        unsigned char red = pixel.getRed();
        unsigned char green = pixel.getGreen();
        unsigned char blue = pixel.getBlue();

        unsigned char invertedRed = max_luminocity - red;
        unsigned char invertedGreen = max_luminocity - green;
        unsigned char invertedBlue = max_luminocity - blue;

        pixel.setRed(invertedRed);
        pixel.setGreen(invertedGreen);
        pixel.setBlue(invertedBlue);
      }
    }

    return *this;
  }

  virtual Image &operator~() override;

  virtual Image &operator*() override {
    if (pixels == nullptr) {
      // Image is empty, nothing to reverse
      return *this;
    }

    for (int row = 0; row < height; row++) {
      for (int col = 0; col < width / 2; col++) {
        std::swap(pixels[row][col], pixels[row][width - 1 - col]);
      }
    }

    return *this;
  }

  friend std::ostream &operator<<(std::ostream &out, Image &image);

  ~RGBImage() {
    // Free the memory allocated for the pixels
    if (pixels != nullptr) {
      for (int row = 0; row < height; row++) {
        delete[] pixels[row];
        pixels[row] = nullptr; // Set the row pointer to nullptr
      }
      delete[] pixels;
      pixels = nullptr;
    }
  }
};
/******************** END RGBIMAGE CLASS********************/

/******************** GSCImage CLASS ********************/
class GSCImage : public Image {
private:
  GSCPixel **pixels;

public:
  GSCImage() : pixels(nullptr) {
    // Initialize the class fields
    width = 0;
    height = 0;
    max_luminocity = 255;
    pixels = nullptr;
  }

  GSCImage(const GSCImage &img) : pixels(nullptr) {
    width = img.getWidth();
    height = img.getHeight();
    max_luminocity = img.getMaxLuminocity();

    // Allocate memory for pixels
    pixels = new GSCPixel *[height];
    for (int row = 0; row < height; row++) {
      pixels[row] = new GSCPixel[width];
      for (int col = 0; col < width; col++) {
        pixels[row][col] = img.pixels[row][col];
      }
    }
  }

  GSCImage(const RGBImage &rgb) : pixels(nullptr) {
    width = rgb.getWidth();
    height = rgb.getHeight();
    max_luminocity = rgb.getMaxLuminocity();

    // Allocate memory for pixels
    pixels = new GSCPixel *[height];
    for (int row = 0; row < height; row++) {
      pixels[row] = new GSCPixel[width];
      for (int col = 0; col < width; col++) {
        const RGBPixel &rgbPixel = rgb.getPixel(row, col);
        unsigned char grayValue = static_cast<unsigned char>(
            rgbPixel.getRed() * 0.3 + rgbPixel.getGreen() * 0.59 +
            rgbPixel.getBlue() * 0.11);
        pixels[row][col] = GSCPixel(grayValue);
      }
    }
  }

  GSCImage(std::istream &stream) : pixels(nullptr) {
    stream >> width >> height >> max_luminocity;

    // Allocate memory for pixels
    pixels = new GSCPixel *[height];
    for (int row = 0; row < height; row++) {
      pixels[row] = new GSCPixel[width];
      for (int col = 0; col < width; col++) {
        int pixelValue;
        stream >> pixelValue;
        pixels[row][col] = GSCPixel(static_cast<unsigned char>(pixelValue));
      }
    }
  }

  virtual GSCPixel &getPixel(int row, int col) const override {
    if (pixels == nullptr) {
      throw std::runtime_error("Image is not initialized.");
    }

    if (row < 0 || row >= height || col < 0 || col >= width) {
      throw std::out_of_range("Invalid pixel coordinates.");
    }

    return pixels[row][col];
  }

  virtual Image &operator+=(int times) override {
    // Calculate the effective rotation times
    int effectiveTimes = times % 4;
    if (effectiveTimes < 0)
      effectiveTimes += 4;

    // Rotate the image clockwise
    for (int i = 0; i < effectiveTimes; i++) {
      // Create a temporary image to store the rotated pixels
      GSCImage rotatedImage;
      rotatedImage.setWidth(getHeight());
      rotatedImage.setHeight(getWidth());
      rotatedImage.setMaxLuminocity(255);
      rotatedImage.pixels = new GSCPixel *[rotatedImage.getHeight()];
      for (int row = 0; row < rotatedImage.getHeight(); row++) {
        rotatedImage.pixels[row] = new GSCPixel[rotatedImage.getWidth()];
        for (int col = 0; col < rotatedImage.getWidth(); col++) {
          rotatedImage.pixels[row][col] = GSCPixel();
        }
      }

      // Perform the rotation by rearranging the pixels
      for (int row = 0; row < getHeight(); row++) {
        for (int col = 0; col < getWidth(); col++) {
          // Get the pixel from the original image
          GSCPixel &originalPixel = getPixel(row, col);

          // Calculate the new coordinates for the rotated pixel
          int rotatedRow = col;
          int rotatedCol = getHeight() - 1 - row;

          // Get the pixel from the rotated image and assign theoriginal pixel
          // value
          GSCPixel &rotatedPixel =
              rotatedImage.getPixel(rotatedRow, rotatedCol);
          rotatedPixel = originalPixel;
        }
      }

      // Assign the rotated image to the current image
      *this = rotatedImage;
      if (rotatedImage.pixels != nullptr) {
        for (int row = 0; row < rotatedImage.getHeight(); row++) {
          delete[] rotatedImage.pixels[row];
        }
        delete[] rotatedImage.pixels;
        rotatedImage.pixels = nullptr;
      }
    }

    return *this;
  }

  virtual Image &operator*=(double factor) override {
    // Calculate the new dimensions based on the factor
    int newWidth = static_cast<int>(getWidth() * factor);
    int newHeight = static_cast<int>(getHeight() * factor);

    // Create a temporary image with the new dimensions
    GSCImage resizedImage;
    resizedImage.setWidth(newWidth);
    resizedImage.setHeight(newHeight);
    resizedImage.setMaxLuminocity(255);
    resizedImage.pixels = new GSCPixel *[resizedImage.getHeight()];
    for (int row = 0; row < resizedImage.getHeight(); row++) {
      resizedImage.pixels[row] = new GSCPixel[resizedImage.getWidth()];
      for (int col = 0; col < resizedImage.getWidth(); col++) {
        resizedImage.pixels[row][col] = GSCPixel();
      }
    }

    // Scale the pixels from the original image to the resized image
    for (int row = 0; row < newHeight; row++) {
      for (int col = 0; col < newWidth; col++) {
        // Calculate the corresponding coordinates in the original image
        int r1 = std::min(static_cast<int>(std::floor(row / factor)),
                          getHeight() - 1);
        int r2 = std::min(static_cast<int>(std::ceil(row / factor)),
                          getHeight() - 1);
        int c1 = std::min(static_cast<int>(std::floor(col / factor)),
                          getWidth() - 1);
        int c2 =
            std::min(static_cast<int>(std::ceil(col / factor)), getWidth() - 1);

        // Get the pixels from the original image
        GSCPixel &p11 = getPixel(r1, c1);
        GSCPixel &p12 = getPixel(r1, c2);
        GSCPixel &p21 = getPixel(r2, c1);
        GSCPixel &p22 = getPixel(r2, c2);

        // Calculate the average brightness
        unsigned char avgBrightness = (p11.getValue() + p12.getValue() +
                                       p21.getValue() + p22.getValue()) /
                                      4;

        // Set the new pixel in the resized image
        GSCPixel &resizedPixel = resizedImage.getPixel(row, col);
        resizedPixel.setValue(avgBrightness);
      }
    }

    // Assign the resized image to the current image
    *this = resizedImage;
    if (resizedImage.pixels != nullptr) {
      for (int row = 0; row < resizedImage.getHeight(); row++) {
        delete[] resizedImage.pixels[row];
      }
      delete[] resizedImage.pixels;
      resizedImage.pixels = nullptr;
    }
    return *this;
  }

  virtual Image &operator!() override {
    if (pixels == nullptr) {
      // Image is empty, nothing to invert
      return *this;
    }

    for (int row = 0; row < height; row++) {
      for (int col = 0; col < width; col++) {
        GSCPixel &pixel = getPixel(row, col);
        unsigned char currentLuminosity = pixel.getValue();
        unsigned char invertedLuminosity = max_luminocity - currentLuminosity;
        pixel.setValue(invertedLuminosity);
      }
    }

    return *this;
  }

  virtual Image &operator*() override {
    if (pixels == nullptr) {
      // Image is empty, nothing to reverse
      return *this;
    }

    for (int row = 0; row < height; row++) {
      for (int col = 0; col < width / 2; col++) {
        std::swap(pixels[row][col], pixels[row][width - 1 - col]);
      }
    }

    return *this;
  }

  GSCImage &operator=(const GSCImage &img) {
    if (this == &img) {
      // Self-assignment check
      return *this;
    }

    // Free existing memory
    if (pixels != nullptr) {
      for (int row = 0; row < height; row++) {
        delete[] pixels[row];
      }
      delete[] pixels;
      pixels = nullptr;
    }

    // Assign new dimensions and maximum luminosity
    width = img.width;
    height = img.height;
    max_luminocity = img.max_luminocity;

    // Allocate new memory and copy pixels
    pixels = new GSCPixel *[height];
    for (int row = 0; row < height; row++) {
      pixels[row] = new GSCPixel[width];
      for (int col = 0; col < width; col++) {
        pixels[row][col] = img.getPixel(row, col);
      }
    }

    return *this;
  }

  virtual Image &operator~() override;

  friend std::ostream &operator<<(std::ostream &out, Image &image);

  ~GSCImage() {
    // Free the memory allocated for the pixels
    if (pixels != nullptr) {
      for (int row = 0; row < height; row++) {
        delete[] pixels[row];
      }
      delete[] pixels;
    }
  }
};
/******************** END GSCIMAGE CLASS ********************/

/******************** YUVIMAGE CLASS ********************/
class YUVImage {
private:
  int width;
  int height;
  std::vector<std::vector<int>> yuvImage;

public:
  YUVImage(const RGBImage &rgbImage) {
    width = rgbImage.getWidth();
    height = rgbImage.getHeight();
    yuvImage.resize(height, std::vector<int>(width, 0));

    // Convert RGB image to YUV
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        RGBPixel rgbPixel = rgbImage.getPixel(i, j);

        int y = ((66 * rgbPixel.getRed() + 129 * rgbPixel.getGreen() +
                  25 * rgbPixel.getBlue() + 128) >>
                 8) +
                16;
        int u = ((-38 * rgbPixel.getRed() - 74 * rgbPixel.getGreen() +
                  112 * rgbPixel.getBlue() + 128) >>
                 8) +
                128;
        int v = ((112 * rgbPixel.getRed() - 94 * rgbPixel.getGreen() -
                  18 * rgbPixel.getBlue() + 128) >>
                 8) +
                128;
        yuvImage[i][j] = (y << 16) | (u << 8) | v;
      }
    }
  }

  int getWidth() const { return width; }

  int getHeight() const { return height; }

  int getY(int row, int col) const { return (yuvImage[row][col] >> 16) & 0xFF; }

  int getU(int row, int col) const { return (yuvImage[row][col] >> 8) & 0xFF; }

  int getV(int row, int col) const { return yuvImage[row][col] & 0xFF; }

  void equalizeHistogram() {
    // Step 1: Calculate histogram
    std::vector<int> histogram(236, 0);
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        int y = getY(i, j);
        histogram[y]++;
      }
    }

    // Step 2: Calculate probability distribution
    std::vector<float> probDistribution(236, 0.0);
    int totalPixels = width * height;
    for (int i = 0; i < 236; ++i) {
      probDistribution[i] = static_cast<float>(histogram[i]) / totalPixels;
    }

    // Step 3: Calculate cumulative probability distribution
    std::vector<float> cumDistribution(236, 0.0);
    cumDistribution[0] = probDistribution[0];
    for (int i = 1; i < 236; ++i) {
      cumDistribution[i] = cumDistribution[i - 1] + probDistribution[i];
    }

    // Step 4: Choose maximum brightnes value
    int max = 235; // for color images

    // Step 5: Calculate brightnes change
    std::vector<int> brightnesChange(236, 0);
    for (int i = 0; i < 236; ++i) {
      brightnesChange[i] = static_cast<int>(cumDistribution[i] * max);
      if (cumDistribution[i] >= 1)
        brightnesChange[i]--; // might delete
    }

    // Step 6: Apply brightnes change to Y component
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        int y = getY(i, j);
        int newY = brightnesChange[y];
        yuvImage[i][j] = (newY << 16) | (getU(i, j) << 8) | getV(i, j);
      }
    }
  }

  RGBImage toRGB() const {
    RGBImage rgbImage(width, height);

    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        int c = getY(i, j) - 16;
        int d = getU(i, j) - 128;
        int e = getV(i, j) - 128;

        unsigned char r =
            std::max(0, std::min(255, (298 * c + 409 * e + 128) >> 8));
        unsigned char g = std::max(
            0, std::min(255, (298 * c - 100 * d - 208 * e + 128) >> 8));
        unsigned char b =
            std::max(0, std::min(255, (298 * c + 516 * d + 128) >> 8));

        RGBPixel rgbPixel(r, g, b);
        rgbImage.getPixel(i, j) = rgbPixel;
      }
    }

    return rgbImage;
  }
};
/******************** END YUVIMAGE CLASS ********************/

std::ostream &operator<<(std::ostream &out, Image &image) {

  if (dynamic_cast<GSCImage *>(&image) != nullptr) {
    // Black and white image (PGM format)
    out << "P2" << std::endl;
    out << image.getWidth() << " " << image.getHeight() << " "
        << image.getMaxLuminocity() << "\n";
    for (int row = 0; row < image.height; row++) {
      for (int col = 0; col < image.width; col++) {
        GSCPixel &pixel = dynamic_cast<GSCImage &>(image).getPixel(row, col);
        out << static_cast<int>(pixel.getValue());
        out << std::endl;
      }
    }
  } else if (dynamic_cast<RGBImage *>(&image) != nullptr) {
    // Color image (PPM format)
    out << "P3" << std::endl;
    out << image.getWidth() << " " << image.getHeight() << " "
        << image.getMaxLuminocity() << "\n";
    for (int row = 0; row < image.height; row++) {
      for (int col = 0; col < image.width; col++) {
        RGBPixel &pixel = dynamic_cast<RGBImage &>(image).getPixel(row, col);
        out << static_cast<int>(pixel.getRed()) << " ";
        out << static_cast<int>(pixel.getGreen()) << " ";
        out << static_cast<int>(pixel.getBlue());
        out << std::endl;
      }
    }
  }

  return out;
}
// Definition of operator~ for RGBImage
Image &RGBImage::operator~() {
  YUVImage yuvImage(*this);
  yuvImage.equalizeHistogram();
  *this = yuvImage.toRGB();
  return *this;
}

// Definition of operator~ for GSCImage
Image &GSCImage::operator~() {
  RGBImage *rgbImage = new RGBImage(*this);
  YUVImage yuvImage(*rgbImage);
  yuvImage.equalizeHistogram();
  *rgbImage = yuvImage.toRGB();
  this->width = rgbImage->getWidth();
  this->height = rgbImage->getHeight();
  this->max_luminocity = rgbImage->getMaxLuminocity();

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      this->pixels[row][col].setValue(rgbImage->getPixel(row, col).getRed());
    }
  }
  delete rgbImage;
  return *this;
}

RGBImage::RGBImage(const GSCImage &gsc) : pixels(nullptr) {
  width = gsc.getWidth();
  height = gsc.getHeight();
  max_luminocity = gsc.getMaxLuminocity();

  // Allocate memory for pixels
  pixels = new RGBPixel *[height];
  for (int row = 0; row < height; row++) {
    pixels[row] = new RGBPixel[width];
    for (int col = 0; col < width; col++) {
      pixels[row][col] = RGBPixel(gsc.getPixel(row, col).getValue(),
                                  gsc.getPixel(row, col).getValue(),
                                  gsc.getPixel(row, col).getValue());
    }
  }
}

/******************** TOKEN CLASS ********************/
class Token {
private:
  std::string name;
  Image *ptr;

public:
  Token(const std::string &tokenName = "", Image *imagePtr = nullptr);
  std::string getName() const;
  Image *getPtr() const;
  void setName(const std::string &tokenName);
  void setPtr(Image *imagePtr);
};

Token::Token(const std::string &tokenName, Image *imagePtr) {
  name = tokenName;
  ptr = imagePtr;
}

std::string Token::getName() const { return name; }

Image *Token::getPtr() const { return ptr; }

void Token::setName(const std::string &tokenName) { name = tokenName; }

void Token::setPtr(Image *imagePtr) { ptr = imagePtr; }
/******************** END TOKEN CLASS ********************/

/******************** MAIN ********************/
std::vector<Token>::iterator findToken(std::vector<Token> &tokenList,
                                       const std::string &token) {
  return std::find_if(tokenList.begin(), tokenList.end(),
                      [&](const Token &t) { return t.getName() == token; });
}

bool fileExists(const std::string &filename) {
  std::ifstream file(filename);
  return file.good();
}

Image *readNetpbmImage(const char *filename) {
  std::ifstream f(filename);
  if (!f.is_open()) {
    std::cout << "[ERROR] Unable to open " << filename << std::endl;
  }
  Image *img_ptr = nullptr;
  std::string type;

  if (f.good() && !f.eof())
    f >> type;
  if (!type.compare("P3")) {
    img_ptr = new RGBImage(f);
  } else if (!type.compare("P2")) {
    img_ptr = new GSCImage(f);
  } else if (f.is_open()) {
    std::cout << "[ERROR] Invalid file format" << std::endl;
  }
  return img_ptr;
}

void exportImageToFile(const std::string &filename, Image &image) {
  // Open the file for writing
  std::ofstream file(filename);
  if (!file) {
    std::cout << "[ERROR] Unable to create file" << std::endl;
    return;
  }

  file << image;

  file.close();
}

void deleteToken(Token &token) {
  if (token.getPtr()) {
    delete token.getPtr();
    token.setPtr(nullptr);
  }
}

void invertColor(Image &image) { image = !image; }

void histogramEqualization(Image &image) { image = ~image; }

void invertImageInYAxis(Image &image) { image = *image; }

Image *rgbToGsc(Image &image, std::string name) {
  RGBImage *rgbImage = dynamic_cast<RGBImage *>(&image);
  if (rgbImage) {
    GSCImage *gscImage = new GSCImage(*rgbImage);
    delete &image;
    std::cout << "[OK] Grayscale " << name << "\n";
    return gscImage;
  } else {
    std::cout << "[NOP] Already grayscale " << name << "\n";
  }
  return &image;
}

void scale(Image &image, double factor) { image *= factor; }

void rotate(Image &image, int times) { image += times; }

int main() {
  std::vector<Token> tokenList;

  while (true) {
    std::string line;
    std::getline(std::cin, line);

    std::istringstream iss(line);
    std::string command;
    iss >> command;

    if (command == "i") {
      std::string photoFile;
      std::string as;
      std::string name;
      iss >> photoFile >> as >> name;

      if (photoFile.empty() || name.empty() || name[0] != '$' || as != "as") {
        std::cout << "\n-- Invalid command! --\n";
        continue;
      }

      if (!fileExists(photoFile)) {
        std::cout << "[ERROR] Unable to open " << photoFile << "\n";
        continue;
      }

      Image *imgPtr = readNetpbmImage(photoFile.c_str());
      auto it = findToken(tokenList, name);
      if (it != tokenList.end()) {
        std::cout << "[ERROR] Token " << name << " already exists!\n";
        continue;
      }

      Token token;
      token.setName(name);
      token.setPtr(imgPtr);
      tokenList.push_back(token); // Add the token to the list
      std::cout << "[OK] Import " << name << "\n";
    } else if (command == "e") {
      std::string photoFile;
      std::string as;
      std::string name;
      iss >> name >> as >> photoFile;

      if (photoFile.empty() || name.empty() || name[0] != '$' || as != "as") {
        std::cout << "\n-- Invalid command! --\n";
        continue;
      }

      auto it = findToken(tokenList, name);
      if (it == tokenList.end()) {
        std::cout << "[ERROR] Token " << name << " not found!\n";
        continue;
      }

      if (fileExists(photoFile)) {
        std::cout << "[ERROR] File exists\n";
        continue;
      }

      Token &token = *it;
      exportImageToFile(photoFile, *(token.getPtr()));
      std::cout << "[OK] Export " << name << "\n";
    } else if (command == "d") {
      std::string name;
      iss >> name;

      if (name.empty() || name[0] != '$') {
        std::cout << "\n-- Invalid command! --\n";
        continue;
      }

      auto it = findToken(tokenList, name);
      if (it == tokenList.end()) {
        std::cout << "[ERROR] Token " << name << " not found!\n";
        continue;
      }
      Token &token = *it;
      deleteToken(token);
      std::cout << "[OK] Delete " << token.getName() << "\n";
      tokenList.erase(it);
    } else if (command == "n") {
      std::string name;
      iss >> name;

      if (name.empty() || name[0] != '$') {
        std::cout << "\n-- Invalid command! --\n";
        continue;
      }

      auto it = findToken(tokenList, name);
      if (it == tokenList.end()) {
        std::cout << "[ERROR] Token " << name << " not found!\n";
        continue;
      }
      Token &token = *it;
      invertColor(*(token.getPtr()));
      std::cout << "[OK] Color Inversion " << token.getName() << "\n";
    } else if (command == "z") {
      std::string name;
      iss >> name;

      if (name.empty() || name[0] != '$') {
        std::cout << "\n-- Invalid command! --\n";
        continue;
      }

      auto it = findToken(tokenList, name);
      if (it == tokenList.end()) {
        std::cout << "[ERROR] Token " << name << " not found!\n";
        continue;
      }
      Token &token = *it;
      histogramEqualization(*(token.getPtr()));
      std::cout << "[OK] Equalize " << token.getName() << "\n";
    } else if (command == "m") {
      std::string name;
      iss >> name;

      if (name.empty() || name[0] != '$') {
        std::cout << "\n-- Invalid command! --\n";
        continue;
      }

      auto it = findToken(tokenList, name);
      if (it == tokenList.end()) {
        std::cout << "[ERROR] Token " << name << " not found!\n";
        continue;
      }
      Token &token = *it;
      invertImageInYAxis(*(token.getPtr()));
      std::cout << "[OK] Mirror " << token.getName() << "\n";
    } else if (command == "g") {
      std::string name;
      iss >> name;

      if (name.empty() || name[0] != '$') {
        std::cout << "\n-- Invalid command! --\n";
        continue;
      }

      auto it = findToken(tokenList, name);
      if (it == tokenList.end()) {
        std::cout << "[ERROR] Token " << name << " not found!\n";
        continue;
      }
      Token &token = *it;
      token.setPtr(rgbToGsc(*(token.getPtr()), token.getName()));
    } else if (command == "s") {
      std::string factor;
      std::string by;
      std::string name;
      iss >> name >> by >> factor;

      if (factor.empty() || name.empty() || name[0] != '$' || by != "by") {
        std::cout << "\n-- Invalid command! --";
        continue;
      }

      auto it = findToken(tokenList, name);
      if (it == tokenList.end()) {
        std::cout << "[ERROR] Token " << name << " not found!\n";
        continue;
      }

      if (std::stod(factor) > 2 || std::stod(factor) < 0) {
        std::cout << "[ERROR] Wrong factor!" << factor << "\n";
        continue;
      }

      Token &token = *it;
      scale(*(token.getPtr()), std::stod(factor));
      std::cout << "[OK] Scale " << token.getName() << "\n";
    } else if (command == "r") {
      std::string times;
      std::string clockwise;
      std::string name;
      iss >> name >> clockwise >> times;

      if (times.empty() || name.empty() || name[0] != '$' ||
          clockwise != "clockwise") {
        std::cout << "\n-- Invalid command! --\n";
        continue;
      }

      auto it = findToken(tokenList, name);
      if (it == tokenList.end()) {
        std::cout << "[ERROR] Token " << name << " not found!\n";
        continue;
      }

      Token &token = *it;
      rotate(*(token.getPtr()), std::stod(times));
      std::cout << "[OK] Rotate " << token.getName() << "\n";
    } else if (command == "q") {
      for (auto &token : tokenList) {
        deleteToken(token);
      }

      tokenList.clear(); // Clear the vector after deleting tokens
      break;
    }
  }
  return 0;
}
/******************** END MAIN ********************/