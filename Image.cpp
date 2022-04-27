//
// Created by floodd on 23/03/2022.
//
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "Image.h"

using namespace std;

bool Image::load(string filename)
{
    ifstream ifs;
    ifs.open(filename, ios::binary);
    // need to spec. binary mode for Windows users


    if (ifs.fail()) {
        return false;
    }

    string header;
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
            float gamma = 1/2.2;
            in >> r >> g >> b;

            this->pixels[i].r = (unsigned char) (pow(r, gamma) * 255);
            this->pixels[i].g = (unsigned char) (pow(g, gamma) * 255);
            this->pixels[i].b = (unsigned char) (pow(b, gamma) * 255);
        }

        in.close();

        return true;
    }
    return false;
}
bool Image::savePPM(string filename)
{
    if(this->w == 0 || this-> h == 0) return false;

    ofstream ofs;

    ofs.open(filename, ios::binary);

    if(ofs.fail()) return false;

    ofs << "P6\n" << this->w << " " << this->h << "\n255\n";

    unsigned char r, g, b;

    for (int i=0; i< this->w * this->h; i++)
    {
        r = this->pixels[i].r;
        g = this->pixels[i].g;
        b = this->pixels[i].b;
        ofs << r << g << b;
    }

    ofs.close();

    return true;
}


void Image::filterRed()
{
    for(int i=0; i < this->w * this-> h; i++)
    {
        this->pixels[i].g = 0;
        this->pixels[i].b = 0;
    }
}

void Image::filterGreen()
{
    for(int i=0; i < this->w * this-> h; i++)
    {
        this->pixels[i].r = 0;
        this->pixels[i].b = 0;
    }
}
void Image::filterBlue()
{
    for(int i=0; i < this->w * this-> h; i++)
    {
        this->pixels[i].g = 0;
        this->pixels[i].r = 0;
    }
}
void Image::greyScale()
{
    for(int i=0; i < this->w * this-> h; i++)
    {
        float y = (0.2126 * this->pixels[i].r) + (0.7152 * this->pixels[i].g) + (0.0722 * this->pixels[i].b);
        this->pixels[i].r = this->pixels[i].g = this->pixels[i].b = y;
    }
}
void Image::greyScale(Image* img)
{
    for(int i=0; i < img->w * img-> h; i++)
    {
        float y = (0.2126 * img->pixels[i].r) + (0.7152 * img->pixels[i].g) + (0.0722 * img->pixels[i].b);
        img->pixels[i].r = img->pixels[i].g = img->pixels[i].b = y;
    }
}
void Image::flipHorizontal()
{
    for(int x=0; x < this->h; x++)
    {
        for (int y=0; y < this->w/2; y++)
        {
            swap(this->pixels[(x * w + y)], this->pixels[(x * w + (w - y))]);
        }
    }
}
void Image::flipVertically()
{
    for(int x=0; x < this->w; x++)
    {
        for (int y=0; y < this->h/2; y++)
        {
            swap(this->pixels[(x + y * w)], this->pixels[(x + (h - 1 - y) * w)]);
        }
    }
}
void Image::AdditionalFunction1() // rotate 90 degrees clockwise
{
    Image* copy = new Image(this);

    for(int x=0; x<w; x++) {
        for(int y=0; y<h; y++) {
            this->pixels[y + x * h] = copy->pixels[x + (h - 1 - y) * w];
        }
    }

    swap(w, h);

    delete copy;
    copy = nullptr;
}
void Image::AdditionalFunction2() // gaussian blur
{
    // generate kernel
    int kernel_size = 5;
    double kernel[kernel_size * kernel_size];

    double sigma = 2.0;
    double r, s = 2.0 * sigma * sigma;

    double sum = 0.0;

    int i, j;
    double mean = kernel_size/2.0;
    for(i=0; i < kernel_size; i++)
    {
        for (j = 0; j < kernel_size; j++)
        {
            kernel[(i*kernel_size)+j] =exp( -0.5 * (pow((i-mean)/sigma, 2.0) + pow((j-mean)/sigma,2.0)) )
                                       / (2 * M_PI * sigma * sigma);
            sum += kernel[(i*kernel_size)+j];
        }
    }

    // normalising the Kernel
    for (int i = 0; i < kernel_size * kernel_size; ++i){
        kernel[i] /= sum;
    }


    for(int x=0; x < h; x++)
    {
        for(int y=0; y<w; y++)
        {
            if (x <= kernel_size/2 || x >= h-kernel_size/2 || w <= kernel_size/2 || y >= w-kernel_size/2)
                continue;

            int k_ind = 0;
            double rSum = 0.0;
            double gSum = 0.0;
            double bSum = 0.0;

            for(int k_row = -kernel_size/2; k_row <= kernel_size / 2; ++k_row)
            {
                for(int k_col = -kernel_size/2; k_col <= kernel_size / 2; ++k_col)
                {
                    rSum += kernel[k_ind] * (this->pixels[w * (x + k_row) + y + k_col].r) ;
                    gSum += kernel[k_ind] * (this->pixels[w * (x + k_row) + y + k_col].g) ;
                    bSum += kernel[k_ind] * (this->pixels[w * (x + k_row) + y + k_col].b) ;
                    k_ind++;
                }
            }

            this->pixels[x*w+y].r = rSum ;
            this->pixels[x*w+y].g = gSum ;
            this->pixels[x*w+y].b = bSum ;
        }
    }
}
void Image::gaussianBlur(Image* img, double sigma) // gaussian blur
{
    // generate kernel
    int kernel_size = 19;
    double kernel[kernel_size * kernel_size];

    double r, s = 2.0 * sigma * sigma;

    double sum = 0.0;

    int i, j;
    double mean = kernel_size/2.0;
    for(i=0; i < kernel_size; i++)
    {
        for (j = 0; j < kernel_size; j++)
        {
            kernel[(i*kernel_size)+j] =exp( -0.5 * (pow((i-mean)/sigma, 2.0) + pow((j-mean)/sigma,2.0)) )
                                       / (2 * M_PI * sigma * sigma);
            sum += kernel[(i*kernel_size)+j];
        }
    }

    // normalising the Kernel
    for (int i = 0; i < kernel_size * kernel_size; ++i){
        kernel[i] /= sum;
    }


    for(int x=0; x < img->h; x++)
    {
        for(int y=0; y<img->w; y++)
        {
            if (x <= kernel_size/2 || x >= img->h-kernel_size/2 || img->w <= kernel_size/2 || y >= img->w-kernel_size/2)
                continue;

            int k_ind = 0;
            double rSum = 0.0;
            double gSum = 0.0;
            double bSum = 0.0;

            for(int k_row = -kernel_size/2; k_row <= kernel_size / 2; ++k_row)
            {
                for(int k_col = -kernel_size/2; k_col <= kernel_size / 2; ++k_col)
                {
                    rSum += kernel[k_ind] * (img->pixels[img->w * (x + k_row) + y + k_col].r) ;
                    gSum += kernel[k_ind] * (img->pixels[img->w * (x + k_row) + y + k_col].g) ;
                    bSum += kernel[k_ind] * (img->pixels[img->w * (x + k_row) + y + k_col].b) ;
                    k_ind++;
                }
            }

            img->pixels[x*img->w+y].r = rSum;
            img->pixels[x*img->w+y].g = gSum;
            img->pixels[x*img->w+y].b = bSum;
        }
    }
}
void Image::AdditionalFunction3() // crop image to center 400px x 400px
{
    int cw = 400;
    int ch = 400;
    int cx = w / 2 - (cw / 2);
    int cy = h / 2 - (ch / 2);

    Image* crop = new Image(cw, ch);

    for(int y = 0; y < ch; y++) {
        if(y + cy >= h) break;
        for(int x = 0; x < cw; x++) {
            if(x + cx >= w) break;
            memcpy(&crop->pixels[(x + y * cw)], &pixels[(x + cx + (y + cy) * w)], 3);
        }
    }

    w = cw;
    h = ch;

    delete[] pixels;
    pixels = crop->pixels;
    crop = nullptr;
}

void Image::AdvancedFeature()
{
    Image* greyImage = new Image(this);

    greyScale(greyImage);

    Image* blurImage = new Image(greyImage);

    for(int i=0; i<w*h; i++)
    {
        blurImage->pixels[i].r = 255 - blurImage->pixels[i].r;
        blurImage->pixels[i].g = 255 - blurImage->pixels[i].g;
        blurImage->pixels[i].b = 255 - blurImage->pixels[i].b;
    }

    gaussianBlur(blurImage, 10.0);

    // colour dodge
    for(int i=0; i<w*h; i++)
    {
        if(greyImage->pixels[i].r == 255 && greyImage->pixels[i].g == 255 && greyImage->pixels[i].b == 255) {
            this->pixels[i].r = 255;
            this->pixels[i].g = 255;
            this->pixels[i].b = 255;
        }
        else
        {
            this->pixels[i].r = (unsigned char) min(255.0,(blurImage->pixels[i].r * 255.0 / (255 - greyImage->pixels[i].r)));
            this->pixels[i].g = (unsigned char) min(255.0,(blurImage->pixels[i].g * 255.0 / (255 - greyImage->pixels[i].g)));
            this->pixels[i].b = (unsigned char) min(255.0,(blurImage->pixels[i].b * 255.0 / (255 - greyImage->pixels[i].b)));
        }
    }

    greyImage = nullptr;
    blurImage = nullptr;

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