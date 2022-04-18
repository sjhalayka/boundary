// Code by: Shawn Halayka -- sjhalayka@gmail.com
// Code is in the public domain


#ifndef IMAGE_H
#define IMAGE_H


#include <vector>
using std::vector;

#include <fstream>
using std::ifstream;
using std::ofstream;

#include <ios>
using std::ios;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <cstring>


class RGB
{
public:
	unsigned char r, g, b;
};




RGB HSBtoRGB(unsigned short int hue_degree, unsigned char sat_percent, unsigned char bri_percent)
{
	float R = 0.0f;
	float G = 0.0f;
	float B = 0.0f;

	if (hue_degree > 359)
		hue_degree = 359;

	if (sat_percent > 100)
		sat_percent = 100;

	if (bri_percent > 100)
		bri_percent = 100;

	float hue_pos = 6.0f - ((static_cast<float>(hue_degree) / 359.0f) * 6.0f);

	if (hue_pos >= 0.0f && hue_pos < 1.0f)
	{
		R = 255.0f;
		G = 0.0f;
		B = 255.0f * hue_pos;
	}
	else if (hue_pos >= 1.0f && hue_pos < 2.0f)
	{
		hue_pos -= 1.0f;

		R = 255.0f - (255.0f * hue_pos);
		G = 0.0f;
		B = 255.0f;
	}
	else if (hue_pos >= 2.0f && hue_pos < 3.0f)
	{
		hue_pos -= 2.0f;

		R = 0.0f;
		G = 255.0f * hue_pos;
		B = 255.0f;
	}
	else if (hue_pos >= 3.0f && hue_pos < 4.0f)
	{
		hue_pos -= 3.0f;

		R = 0.0f;
		G = 255.0f;
		B = 255.0f - (255.0f * hue_pos);
	}
	else if (hue_pos >= 4.0f && hue_pos < 5.0f)
	{
		hue_pos -= 4.0f;

		R = 255.0f * hue_pos;
		G = 255.0f;
		B = 0.0f;
	}
	else
	{
		hue_pos -= 5.0f;

		R = 255.0f;
		G = 255.0f - (255.0f * hue_pos);
		B = 0.0f;
	}

	if (100 != sat_percent)
	{
		if (0 == sat_percent)
		{
			R = 255.0f;
			G = 255.0f;
			B = 255.0f;
		}
		else
		{
			if (255.0f != R)
				R += ((255.0f - R) / 100.0f) * (100.0f - sat_percent);
			if (255.0f != G)
				G += ((255.0f - G) / 100.0f) * (100.0f - sat_percent);
			if (255.0f != B)
				B += ((255.0f - B) / 100.0f) * (100.0f - sat_percent);
		}
	}

	if (100 != bri_percent)
	{
		if (0 == bri_percent)
		{
			R = 0.0f;
			G = 0.0f;
			B = 0.0f;
		}
		else
		{
			if (0.0f != R)
				R *= static_cast<float>(bri_percent) / 100.0f;
			if (0.0f != G)
				G *= static_cast<float>(bri_percent) / 100.0f;
			if (0.0f != B)
				B *= static_cast<float>(bri_percent) / 100.0f;
		}
	}

	if (R < 0.0f)
		R = 0.0f;
	else if (R > 255.0f)
		R = 255.0f;

	if (G < 0.0f)
		G = 0.0f;
	else if (G > 255.0f)
		G = 255.0f;

	if (B < 0.0f)
		B = 0.0f;
	else if (B > 255.0f)
		B = 255.0f;

	RGB rgb;

	rgb.r = static_cast<unsigned char>(R);
	rgb.g = static_cast<unsigned char>(G);
	rgb.b = static_cast<unsigned char>(B);

	return rgb;
}






// http://www.paulbourke.net/dataformats/tga/
class tga
{
public:

	tga(void)
	{
		idlength = 0;
		colourmaptype = 0;
		datatypecode = 0;
		colourmaporigin = 0;
		colourmaporigin = 0;
		colourmaplength = 0;
		colourmapdepth = 0;
		x_origin = 0;
		y_origin = 0;
		px = 0;
		py = 0;
		bitsperpixel = 0;
		imagedescriptor = 0;
	}

	unsigned char  idlength;
	unsigned char  colourmaptype;
	unsigned char  datatypecode;
	unsigned short int colourmaporigin;
	unsigned short int colourmaplength;
	unsigned char  colourmapdepth;
	unsigned short int x_origin;
	unsigned short int y_origin;
	unsigned short int px;
	unsigned short int py;
	unsigned char  bitsperpixel;
	unsigned char  imagedescriptor;

	vector<char> idstring;

	vector<unsigned char> pixel_data;
};

class float_grayscale
{
public:

	float_grayscale(void)
	{
		px = py = 0;
	}

	unsigned short int px;
	unsigned short int py;
	vector<float> pixel_data;
};


float int_rgb_to_float_grayscale(const unsigned char r, const unsigned char g, const unsigned char b)
{
	// http://www.itu.int/rec/R-REC-BT.709/en
	return	  0.2126f * (static_cast<float>(r) / 255.0f)\
		+ 0.7152f * (static_cast<float>(g) / 255.0f)\
		+ 0.0722f * (static_cast<float>(b) / 255.0f);
}

bool write_float_grayscale_to_tga(const char* const filename, const float_grayscale& l)
{
	unsigned char  idlength = 0;
	unsigned char  colourmaptype = 0;
	unsigned char  datatypecode = 2;
	unsigned short int colourmaporigin = 0;
	unsigned short int colourmaplength = 0;
	unsigned char  colourmapdepth = 0;
	unsigned short int x_origin = 0;
	unsigned short int y_origin = 0;

	unsigned short int px = l.px;
	unsigned short int py = l.py;
	unsigned char  bitsperpixel = 24;
	unsigned char  imagedescriptor = 0;
	vector<char> idstring;

	size_t num_bytes = static_cast<size_t>(3)* static_cast<size_t>(px)* static_cast<size_t>(py);
	vector<unsigned char> pixel_data(num_bytes);


	float greatest_abs = 0;

	for (size_t i = 0; i < px; i++)
	{
		for (size_t j = 0; j < py; j++)
		{
			size_t float_index = j * px + i;

			if (fabsf(l.pixel_data[float_index]) > greatest_abs)
				greatest_abs = fabsf(l.pixel_data[float_index]);
		}
	}


	for (size_t i = 0; i < px; i++)
	{
		for (size_t j = 0; j < py; j++)
		{
			size_t float_index = j * px + i;
			size_t fb_index = 3 * float_index;

			float gray_val = l.pixel_data[float_index];
			RGB rgb = HSBtoRGB(static_cast<unsigned short>(75.0 * gray_val), 75, 100);

			//if (gray_val == 1)
			//{
			//	pixel_data[fb_index] = 255;
			//	pixel_data[fb_index + 1] = 255;
			//	pixel_data[fb_index + 2] = 255;
			//}
			//else
			//{
			//	pixel_data[fb_index] = rgb.b;
			//	pixel_data[fb_index + 1] = rgb.g;
			//	pixel_data[fb_index + 2] = rgb.r;
			//}

			if (l.pixel_data[float_index] < 0)
			{
				pixel_data[fb_index] = static_cast<unsigned char>(-l.pixel_data[float_index]/greatest_abs * 255.0);
				pixel_data[fb_index + 1] = 0;// static_cast<unsigned char>(l.pixel_data[float_index] * 255.0);
				pixel_data[fb_index + 2] = 0;// static_cast<unsigned char>(l.pixel_data[float_index] * 255.0);
			}
			else
			{
				pixel_data[fb_index] = 0;// static_cast<unsigned char>(l.pixel_data[float_index] * 255.0);
				pixel_data[fb_index + 1] = 0;// static_cast<unsigned char>(l.pixel_data[float_index] * 255.0);
				pixel_data[fb_index + 2] = static_cast<unsigned char>(l.pixel_data[float_index]/greatest_abs * 255.0);
			}
		}
	}

	// Reverse row order
	short unsigned int num_rows_to_swap = py;
	vector<unsigned char> buffer(static_cast<size_t>(px) * 3);

	if (0 != py % 2)
		num_rows_to_swap--;

	num_rows_to_swap /= 2;

	for (size_t i = 0; i < num_rows_to_swap; i++)
	{
		size_t y_first = i * static_cast<size_t>(px) * 3;
		size_t y_last = (static_cast<size_t>(py) - 1 - i)* static_cast<size_t>(px) * 3;

		memcpy(&buffer[0], &pixel_data[y_first], static_cast<size_t>(px) * 3);
		memcpy(&pixel_data[y_first], &pixel_data[y_last], static_cast<size_t>(px) * 3);
		memcpy(&pixel_data[y_last], &buffer[0], static_cast<size_t>(px) * 3);
	}

	// Write Targa TGA file to disk
	ofstream out(filename, ios::binary);

	if (!out.is_open())
	{
		cout << "Failed to open TGA file for writing: " << filename << endl;
		return false;
	}

	out.write(reinterpret_cast<char*>(&idlength), 1);
	out.write(reinterpret_cast<char*>(&colourmaptype), 1);
	out.write(reinterpret_cast<char*>(&datatypecode), 1);
	out.write(reinterpret_cast<char*>(&colourmaporigin), 2);
	out.write(reinterpret_cast<char*>(&colourmaplength), 2);
	out.write(reinterpret_cast<char*>(&colourmapdepth), 1);
	out.write(reinterpret_cast<char*>(&x_origin), 2);
	out.write(reinterpret_cast<char*>(&y_origin), 2);
	out.write(reinterpret_cast<char*>(&px), 2);
	out.write(reinterpret_cast<char*>(&py), 2);
	out.write(reinterpret_cast<char*>(&bitsperpixel), 1);
	out.write(reinterpret_cast<char*>(&imagedescriptor), 1);

	out.write(reinterpret_cast<char*>(&pixel_data[0]), num_bytes);

	out.close();

	return true;
}


bool convert_tga_to_float_grayscale(const char* const filename, tga& t, float_grayscale& l, const bool make_black_border, const bool reverse_rows, const bool reverse_pixel_byte_order)
{
	// http://www.paulbourke.net/dataformats/tga/
	ifstream in(filename, ios::binary);

	if (!in.is_open())
	{
		cerr << "Failed to open TGA file: " << filename << endl;
		return false;
	}

	// Read in header, including variable length image descriptor
	in.read(reinterpret_cast<char*>(&t.idlength), 1);
	in.read(reinterpret_cast<char*>(&t.colourmaptype), 1);
	in.read(reinterpret_cast<char*>(&t.datatypecode), 1);
	in.read(reinterpret_cast<char*>(&t.colourmaporigin), 2);
	in.read(reinterpret_cast<char*>(&t.colourmaplength), 2);
	in.read(reinterpret_cast<char*>(&t.colourmapdepth), 1);
	in.read(reinterpret_cast<char*>(&t.x_origin), 2);
	in.read(reinterpret_cast<char*>(&t.y_origin), 2);
	in.read(reinterpret_cast<char*>(&t.px), 2);
	in.read(reinterpret_cast<char*>(&t.py), 2);
	in.read(reinterpret_cast<char*>(&t.bitsperpixel), 1);
	in.read(reinterpret_cast<char*>(&t.imagedescriptor), 1);

	if (0 != t.idlength)
	{
		t.idstring.resize(static_cast<size_t>(t.idlength) + 1, '\0'); // Terminate this ``C style'' string properly
		in.read(&t.idstring[0], t.idlength);
	}

	// Read pixels, convert to floating point
	if (2 != t.datatypecode)
	{
		cerr << "TGA file must be in uncompressed/non-RLE 24-bit RGB format." << endl;
		return false;
	}
	else
	{
		if (24 != t.bitsperpixel)
		{
			cerr << "TGA file must be in uncompressed/non-RLE 24-bit RGB format." << endl;
			return false;
		}

		// Read all pixels at once
		size_t num_bytes = static_cast<size_t>(t.px)* static_cast<size_t>(t.py) * 3;
		t.pixel_data.resize(num_bytes);
		in.read(reinterpret_cast<char*>(&t.pixel_data[0]), num_bytes);

		if (true == reverse_rows)
		{
			// Reverse row order
			short unsigned int num_rows_to_swap = t.py;
			vector<unsigned char> buffer(static_cast<size_t>(t.px) * 3);

			if (0 != t.py % 2)
				num_rows_to_swap--;

			num_rows_to_swap /= 2;

			for (size_t i = 0; i < num_rows_to_swap; i++)
			{
				size_t y_first = i * static_cast<size_t>(t.px) * 3;
				size_t y_last = (static_cast<size_t>(t.py) - 1 - i)* static_cast<size_t>(t.px) * 3;

				memcpy(&buffer[0], &t.pixel_data[y_first], static_cast<size_t>(t.px) * 3);
				memcpy(&t.pixel_data[y_first], &t.pixel_data[y_last], static_cast<size_t>(t.px) * 3);
				memcpy(&t.pixel_data[y_last], &buffer[0], static_cast<size_t>(t.px) * 3);
			}
		}

		if (true == make_black_border)
		{
			// Make border pixels black
			for (size_t x = 0; x < t.px; x++)
			{
				for (size_t y = 0; y < t.py; y++)
				{
					if (x == 0 || x == t.px - 1 || y == 0 || y == t.py - 1)
					{
						size_t index = y * static_cast<size_t>(t.px) * 3 + x * 3;
						t.pixel_data[index] = 0;
						t.pixel_data[index + 1] = 0;
						t.pixel_data[index + 2] = 0;
					}
				}
			}
		}

		// Fill floating point grayscale image.
		l.px = t.px;
		l.py = t.py;
		l.pixel_data.resize(num_bytes / 3, 0);

		for (size_t index = 0; index < num_bytes; index += 3)
		{
			if (reverse_pixel_byte_order)
			{
				// Swap red and blue pixels.
				unsigned char temp = t.pixel_data[index];
				t.pixel_data[index] = t.pixel_data[index + 2];
				t.pixel_data[index + 2] = temp;
			}

			// Convert to luma.
			l.pixel_data[index / 3] = int_rgb_to_float_grayscale(t.pixel_data[index], t.pixel_data[index + 1], t.pixel_data[index + 2]);
		}
	}

	return true;
}




#endif
