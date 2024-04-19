#ifndef IMAGE_H
#define IMAGE_H
#include <string>
class Image
{
public:
    std::string imageName;
    int imageWidth;
    int imageHeight;

    Image() = default;

    Image(const std::string& name) : imageName(name) {}

    Image operator=(const Image& other)
    {
        imageName = other.imageName;
        imageWidth = other.imageWidth;
        imageHeight = other.imageHeight;
        return *this;
    }

    bool operator==(const Image& other) const
    {
        return imageName == other.imageName && imageWidth == other.imageWidth && imageHeight == other.imageHeight;
    }
};
#endif