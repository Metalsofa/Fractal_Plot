/* This header file defines file operations for the FGR file format, reading them in a binaries. */
#pragma once

#ifndef __FILEOPS_H__
#define __FILEOPS_H__

#include "fgrclasses.h"
#include "fgrcolor.h"

namespace fgr {

	////////////////////////////////////////////////////////////////////////
	//		POINT BINARY STRUCTURE
	//
	//	| FLOAT X | FLOAT Y |
	//

	//Get a point or set of points from a filestream, and advance the internal position indicator by that many characters.
	point fgetpoint(FILE*& stream) {
		//This is the point that will be returned
		float buffer[2];
		fread(buffer, sizeof(float), 2, stream);
		return point(buffer[0], buffer[1]);
	}

	//Put a point into a filestream, and advance the internal position indicator by that many bytes
	void fputpoint(const point& obj, FILE*& stream) {
		//This is the point that will be returned
		float buffer[2] = { obj.x(), obj.y() };
		fwrite(buffer, sizeof(float), 2, stream);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//		GLYPH BINARY STRUCTURE
	//
	//	| GLmode GLMODE | STD::SIZE_T POINT_COUNT | FLOAT X_1 | FLOAT Y_1 | FLOAT X_2 | FLOAT Y_2 | ... | FLOAT X_N | FLOAT Y_N |
	//

	//Get a glyph from a file stream
	glyph fgetglyph(FILE*& stream) {
		//Read in the GLmode of this glyph
		GLmode GLMODE = glPoints;
		fread(&GLMODE, sizeof(GLmode), 1, stream);
		//Read in the number of points in the glyph
		std::size_t POINTC;
		fread(&POINTC, sizeof(std::size_t), 1, stream);
		//Read in every point, one at a time
		glyphContainer pointData;
		for (std::size_t i = 0; i < POINTC; ++i) {
			pointData.push_back(fgetpoint(stream));
		}
		//Construct and return the glyph object
		return glyph(GLMODE, pointData);
	}

	//Put a glyph into a file stream
	void fputglyph(const glyph& obj, FILE*& stream) {
		//Write in the GLmode of this glyph
		fwrite(&obj.mode, sizeof(GLmode), 1, stream);
		//Write in the number of points in the glyph
		std::size_t POINTC = obj.size();
		fwrite(&POINTC, sizeof(std::size_t), 1, stream);
		//Write in every point, one at a time
		for (glyphContainer::const_iterator itr = obj.begin(); itr != obj.end(); ++itr) {
			fputpoint(*itr, stream);
		}
	}


	//////////////////////////////////////////////////////////
	//		FCOLOR BINARY STRUCTURE
	//
	//	| FLOAT RED | FLOAT GREEN | FLOAT BLUE | FLOAT ALPHA |
	//

	//Get a shape from a file stream
	fgr::fcolor fgetfcolor(FILE*& stream) {
		//Read in the colors
		float input[4];
		fread(input, sizeof(float), 4, stream);
		//Construct and return the fcolor
		return fcolor(input[0], input[1], input[2], input[3]);
	}
	
	//Put an fcolor into a file stream
	void fputfcolor(const fcolor& col, FILE*& stream) {
		//Write in the colors
		float input[4] = { col.getLevel('r'), col.getLevel('g'), col.getLevel('b'), col.getLevel('a') };
		fwrite(input, sizeof(float), 4, stream);
	}


	//////////////////////////////////////////////////////////////////////
	//		SHAPE BINARY STRUCTURE
	//
	//	| FCOLOR COLOR | FLOAT LINETHICKNESS | FLOAT POINTSIZE | <GLYPH> |
	//

	//Get a shape from a file stream
	shape fgetshape(FILE*& stream) {
		//Read in the fcolor
		fcolor color = fgetfcolor(stream);
		//Read in the linethickness and pointsize
		float weightdata[2];
		fread(weightdata, sizeof(float), 2, stream);
		//Read in the point data
		glyph pointdata = fgetglyph(stream);
		//Construct and return the shape
		return shape(pointdata, color, weightdata[0], weightdata[1]);
	}
	
	//Put a shape into a file stream
	void fputshape(const shape& obj, FILE*& stream) {
		//Write in the fcolor
		fputfcolor(obj.color, stream);
		//Write in the linethickness and pointsize
		float weightdata[2] = { obj.lineThickness, obj.pointSize };
		fwrite(weightdata, sizeof(float), 2, stream);
		//Write in the point data
		fputglyph(obj, stream);
	}

	/////////////////////////////////////////
	//		GRAPHIC BINARY STRUCTURE
	//
	//	| STD::SIZE_T SHAPECOUNT | <SHAPES> |
	//

	//Get a shape from a file stream
	graphic fgetgraphic(FILE*& stream) {
		//Read in the size type
		std::size_t shapecount;
		fread(&shapecount, sizeof(std::size_t), 1, stream);
		//Read in the shapes
		graphicContainer shapedata;
		for (std::size_t i = 0; i < shapecount; ++i) {
			shapedata.push_back(fgetshape(stream));
		}
		//Construct and return the graphic object
		return graphic(shapedata);
	}

	//Put a graphic into a file stream
	void fputgraphic(const graphic& obj, FILE*& stream) {
		//Write in the size type
		std::size_t shapecount = obj.size();
		fwrite(&shapecount, sizeof(std::size_t), 1, stream);
		//Write in the shapes
		for (graphicContainer::const_iterator itr = obj.begin(); itr != obj.end(); ++itr) {
			fputshape(*itr, stream);
		}
	}


	///NOTE TO SELF: I'm slightly worried about signed/unsigned binary reading. If you run into problems, be sure to check on that.
	/////////////////////////////////////////////////////////////////////////////////
	//		FRAME BINARY STRUCTURE
	//
	//	| INT DELAY | <GRAPHIC> |
	//

	//Get a frame of an animtion from a file stream
	frame fgetframe(FILE*& stream) {
		//Read in the delay
		int delay;
		fread(&delay, sizeof(int), 1, stream);
		//Read in the graphic, construct and return the frame
		return frame(delay, fgetgraphic(stream));
	}
	
	//Put a frame of an animtion into a file stream
	void fputframe(const frame& obj, FILE*& stream) {
		//Write in the delay
		fwrite(&obj.delay, sizeof(int), 1, stream);
		//Write in the graphic, construct and return the frame
		fputgraphic(obj, stream);
	}


	/////////////////////////////////////////////////////////////////////////////////
	//		ANIMATION BINARY STRUCTURE
	//
	//	| BOOL CYCLE | STD::SIZE_T FRAMECOUNT | <FRAMES> |
	//

	//Get an animation from a file stream
	animation fgetanimation(FILE*& stream) {
		//Read in 'cycle'
		bool cycle;
		fread(&cycle, sizeof(bool), 1, stream);
		//Read in the number of frames
		std::size_t framec;
		fread(&framec, sizeof(std::size_t), 1, stream);
		//Read in all the frames
		animationContainer frameData;
		for (std::size_t i = 0; i < framec; ++i) {
			frameData.push_back(fgetframe(stream));
		}
		return animation(cycle, frameData);
	}

	//Put an animation into a file stream
	void fputanimation(const animation& obj, FILE*& stream) {
		//Write in 'cycle'
		fwrite(&obj.cycle, sizeof(bool), 1, stream);
		//Write in the number of frames
		std::size_t framec = obj.size();
		fwrite(&framec, sizeof(std::size_t), 1, stream);
		//Write in all the frames
		animationContainer frameData;
		for (animationContainer::const_iterator itr = obj.begin(); itr != obj.end(); ++itr) {
			fputframe(*itr, stream);
		}
	}


	///////////////////// SLIGHTLY MORE USER-FRIENDLY FILE READING/WRITING FUNCITONS //////////////////////////

	//Reads a glyph out of the specified path and assigns it to the glyph refrence passed in
	bool glyphFromFile(glyph& art, const std::string& path) {
		FILE* fgrfile;
		fopen_s(&fgrfile, path.c_str(), "rb");
		if (!fgrfile) return false;
		art = fgetglyph(fgrfile);
		fclose(fgrfile);
		return true;
	}

	//Writes a glyph object to a particular file path
	bool glyphToFile(const glyph& art, const std::string& path) {
		FILE* fgrfile;
		fopen_s(&fgrfile, path.c_str(), "wb");
		if (!fgrfile) return false;
		fputglyph(art, fgrfile);
		fclose(fgrfile);
		return true;
	}

	//Reads a shape out of the specified path and assigns it to the shape refrence passed in
	bool shapeFromFile(shape& art, const std::string& path) {
		FILE* fgrfile;
		fopen_s(&fgrfile, path.c_str(), "rb");
		if (!fgrfile) return false;
		art = fgetshape(fgrfile);
		fclose(fgrfile);
		return true;
	}

	//Writes a shape object to a particular file path
	bool shapeToFile(const shape& art, const std::string& path) {
		FILE* fgrfile;
		fopen_s(&fgrfile, path.c_str(), "wb");
		if (!fgrfile) return false;
		fputshape(art, fgrfile);
		fclose(fgrfile);
		return true;
	}

	//Reads a graphic out of the specified path and assigns it to the graphic refrence passed in
	bool graphicFromFile(graphic& art, const std::string& path) {
		FILE* fgrfile;
		fopen_s(&fgrfile, path.c_str(), "rb");
		if (!fgrfile) return false;
		art = fgetgraphic(fgrfile);
		fclose(fgrfile);
		return true;
	}

	//Writes a graphic object to a particular file path
	bool graphicToFile(const graphic& art, const std::string& path) {
		FILE* fgrfile;
		fopen_s(&fgrfile, path.c_str(), "wb");
		if (!fgrfile) return false;
		fputgraphic(art, fgrfile);
		fclose(fgrfile);
		return true;
	}

	//Reads an animation out of the specified path and assigns it to the animation refrence passed in
	bool animationFromFile(animation& art, const std::string& path) {
		FILE* fgrfile;
		fopen_s(&fgrfile, path.c_str(), "rb");
		if (!fgrfile) return false;
		art = fgetanimation(fgrfile);
		fclose(fgrfile);
		return true;
	}

	//Writes an animation object to a particular file path
	bool animationToFile(const animation& art, const std::string& path) {
		FILE* fgrfile;
		fopen_s(&fgrfile, path.c_str(), "wb");
		if (!fgrfile) return false;
		fputanimation(art, fgrfile);
		fclose(fgrfile);
		return true;
	}

}

#endif
