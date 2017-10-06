/*
 * Copyright (c) 2010 University of Michigan, Ann Arbor.
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
 * Authors:
 *        Ari Grant, grantaa@umich.edu
 *        Sugih Jamin, jamin@eecs.umich.edu
 */

#include <string>
#include <fstream>

using namespace std;

void printToTarga(string location, short width, short height, unsigned char *data) {
	location += ".tga";
	
	ofstream file;
	file.open(location.c_str(), ofstream::out | ofstream::binary);
	
	if( !file.is_open() ) {
		cout << "Unable to open file << " << location << "." << endl;
		return;
	}
	
	file << (unsigned char)0;	//no ID field
	file << (unsigned char)0;	//no color map
	file << (unsigned char)2;	//uncompressed, true-color image
	
	// no color map
	for( int i = 0; i < 5; i++ )
		file << (unsigned char)0;

	// origin data
	file << (unsigned char)0;
	file << (unsigned char)0;
	file << (unsigned char)0;
	file << (unsigned char)0;
	
	// size of image
	file.write( (char *)&width, 2 );
	file.write( (char *)&height, 2 );
	
	file << (unsigned char)32;	//bits-per-pixel
	
	file << (unsigned char)0x08;	//8 bits of alpha channel
	
	// switch from RGB (OpenGL) to BGR (targa)
	for(int i = 0; i < width*height*4; i += 4)
	{
		char temp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = temp;
	}

	// write pixel data
	file.write((char *)data, width*height*4);

	file.close();
}
