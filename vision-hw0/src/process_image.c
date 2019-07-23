#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    //a given pixel p(x,y) is located in the image buffer at : size_of_float * ( H * W * c + y * W + x)
    //performing x boundary check
    x = (x >= 0) ? ((x < im.w) ? x : im.w - 1) : (0);
    //performing y boundary check
    y = (y >= 0) ? ((y < im.h) ? y : im.h - 1) : (0);
    return im.data[im.h * im.w * c + im.w * y + x];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    //A given pixel p(x,y) is located in the image buffer at : size_of_float * ( H * W * c + y * W + x)
    //first we check that the desired pixel is withing the image's boundaries

    if ((x < im.w) && (y < im.h) && (c < im.c))
    {
        //the give coordinate are valid then we can read the pixel from the memory
        im.data[im.h * im.w * c + im.w * y + x] = v;
    }
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    //copying the image buffer to the newly created image
    memcpy(copy.data, im.data, sizeof(float) * im.w * im.h * im.c);
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    float gray_pixel;

    for (size_t y = 0; y < im.h; y++)
    {
        for (size_t x = 0; x < im.w; x++)
        {
            gray_pixel = 0.299 * get_pixel(im, x, y, 0) + 0.587 * get_pixel(im, x, y, 1) + 0.114 * get_pixel(im, x, y, 2);
            set_pixel(gray, x, y, 0, gray_pixel);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for (size_t x = 0; x < im.w; x++)
    {
        for (size_t y = 0; y < im.h; y++)
        {
            set_pixel(im, x, y, c, get_pixel(im, x, y, c) + v);
        }
    }
}

void clamp_image(image im)
{
    float pixel_val;
    for (size_t c = 0; c < im.c; c++)
    {
        for (size_t x = 0; x < im.w; x++)
        {
            for (size_t y = 0; y < im.h; y++)
            {
                pixel_val = get_pixel(im, x, y, c);
                pixel_val = (pixel_val >= 0) ? ((pixel_val <= 1.0) ? pixel_val : 1) : (0.0);
                set_pixel(im, x, y, c, pixel_val);
            }
        }
    }
}

// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in
    float red, green, blue;
    float hue, saturation, value, hue_prime, C;
    for (size_t x = 0; x < im.w; x++)
    {
        for (size_t y = 0; y < im.h; y++)
        {
            red = get_pixel(im, x, y, 0);
            green = get_pixel(im, x, y, 1);
            blue = get_pixel(im, x, y, 2);
            value = three_way_max(red, green, blue);
            C = value - three_way_min(red, green, blue);
            saturation = ((red == 0) && (green == 0) && (blue == 0)) ? (0) : ( C/value);
            
            if (C > 0)
            {
                if (value == red)
                {
                   hue_prime = (green - blue)/(C);
                }
                else
                {
                    if (value == green)
                    {
                        hue_prime = (blue - red)/(C) + 2;
                    }
                    else
                    {
                        if (value == blue)
                        {
                            hue_prime = (red - green)/(C) + 4;
                        }
                    }
                }
                hue = (hue_prime<0)?(hue_prime/(float)6 + 1):(hue_prime/6);
                hue = (hue<0)?(hue+1):(hue);
            }
            else
            {
                hue = 0;
            }

            set_pixel(im,x,y,0,hue);   
            set_pixel(im,x,y,1,saturation);
            set_pixel(im,x,y,2,value);
        }
    }
}

void hsv_to_rgb(image im)
{
    float hue_prime,C,big_X,R1,G1,B1;
    float m;
    for (size_t x = 0; x < im.w; x++)
    {
        for (size_t y = 0; y < im.h; y++)
        {
            C = get_pixel(im,x,y,1)*get_pixel(im,x,y,2);
            hue_prime = get_pixel(im,x,y,0)*(float)6;
            //printf("Hue prime value: %f\n",hue_prime);
            big_X = C*(1-fabs((int)hue_prime%2 - 1));
            if(get_pixel(im,x,y,0)==0)//the hue is nul
            {
                R1 = G1 = B1 = 0.0;
            }
            if ((hue_prime>=0)&&(hue_prime<=1))
            {
                R1 = C;
                G1 = big_X;
                B1 = 0;
            }
            if ((hue_prime>1)&&(hue_prime<=2))
            {
                R1 = big_X;
                G1 = C;
                B1 = 0;
            }
            if ((hue_prime>2)&&(hue_prime<=3))
            {
                R1 = 0;
                G1 = C;
                B1 = big_X;
            }
            if ((hue_prime>3)&&(hue_prime<=4))
            {
                R1 = 0;
                G1 = big_X;
                B1 = C;
            }
            if ((hue_prime>4)&&(hue_prime<=5))
            {
                R1 = big_X;
                G1 = 0;
                B1 = C;
            }
            if ((hue_prime>5)&&(hue_prime<=6))
            {
                R1 = C;
                G1 = 0;
                B1 = big_X;
            }
            m = get_pixel(im,x,y,2) - C;
            set_pixel(im,x,y,0,R1+m);
            set_pixel(im,x,y,1,G1+m);
            set_pixel(im,x,y,2,B1+m);
            
        }
        
    }
    
}
