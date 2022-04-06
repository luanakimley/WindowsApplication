//
// Created by floodd on 23/03/2022.
//
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include "Image.h"



bool Image::load(string filename)
{
    std::ifstream ifs;
    ifs.open(filename, std::ios::binary);
    // need to spec. binary mode for Windows users


    if (ifs.fail()) {
        return false;
    }

    std::string header;
    int width, height, b;
    ifs >> header;
    if (strcmp(header.c_str(), "P6") != 0) return false;
    ifs >> width >> height >> b;

    this->w = width;
    this->h = height;
    this->pixels = new Rgb[width * height]; // this is throw an exception if bad_alloc
    ifs.ignore(256, '\n'); // skip empty lines in necessary until we get to the binary data
    unsigned char pix[3]; // read each pixel one by one and convert bytes to floats
    for (int i = 0; i < width * height; ++i) {
        ifs.read(reinterpret_cast<char *>(pix), 3);
        this->pixels[i].r = pix[0];
        this->pixels[i].g = pix[1];
        this->pixels[i].b = pix[2];
    }
    ifs.close();

    return true;
}
bool Image::loadRaw(string filename)
{
    ifstream in(filename);
    if(in.good())
    {
        in >> w >> h;

        for(int i=0; i< w*h; i++)
        {
            float r, g, b;
            in >> r >> g >> b;
            this->pixels[i].r = (unsigned char) (r * 255);
            this->pixels[i].g = (unsigned char) (g * 255);
            this->pixels[i].b = (unsigned char) (b * 255);

        }

        in.close();

        return true;
    }
    return false;
}
bool Image::savePPM(string filename)
{
    return false;
}


void Image::filterRed()
{

}
void Image::filterGreen()
{

}
void Image::filterBlue()
{

}
void Image::greyScale()
{

}
void Image::flipHorizontal()
{


}
void Image::flipVertically()
{


}
void Image::AdditionalFunction2()
{

}
void Image::AdditionalFunction3()
{

}
void Image::AdditionalFunction1()
{

}

/* Functions used by the GUI - DO NOT MODIFY */
int Image::getWidth()
{
    return w;
}

int Image::getHeight()
{
    return h;
}

Rgb* Image::getImage()
{
    return pixels;
}