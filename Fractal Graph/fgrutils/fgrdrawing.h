/*This header defines methods for drawing .fgr graphics using the OpenGL API.
 *In order for it to work as expected, freeglut's include directories must be
 *recognizable by the application. */

#include "gl/glut.h"
#include "fgrgeometry.h"
#include "fgrclasses.h"

namespace  fgr {

	//Use an fgr::fcolor to set the gl rendering color
	void setcolor(const fcolor& col) {
		glColor4f(col.getLevel('r'), col.getLevel('g'), col.getLevel('b'), col.getLevel('a'));
	}

	//Draw specified text at a specified point (it's drawn pretty small)
	void drawText(const point& location, const std::string& text) {
		glRasterPos2f(location.x(), location.y());
		for (unsigned int i = 0; i < text.size(); i++) { //glutBitmapString() https://stackoverflow.com/questions/544079/how-do-i-use-glutbitmapstd::string-in-c-to-draw-text-to-the-screen
			//Draw each character    
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
		}
	}
	
	// FGR GEOMETRY RENDERING FUNCTIONS
	void glVertexPoint(const point& dot) {
		glVertex2f(dot.x(), dot.y());
	}
	void glVertexSegment(const segment& seg) {
		glVertexPoint(seg.p1);
		glVertexPoint(seg.p2);
	}
	void glVertexTriangle(const triangle& tri) {
		glVertexPoint(tri.p1);
		glVertexPoint(tri.p2);
		glVertexPoint(tri.p3);
	}
	void glVertexFermatPoint(const triangle& tri) {
		glVertexPoint(tri.fermatpoint(0));
	}

	void drawPoint(const point& dot, float size, bool label) {
		glPointSize(size);
		glBegin(GL_POINTS);
		glVertexPoint(dot);
		glEnd();
		if (label)
			drawText(dot, dot.label());
	}

	//Like glTranslatef, but it takes a single argument: a point
	void glTranslatePoint(const point& offset) {
		glTranslatef(offset.x(), offset.y(), 0.0f);
	}

	//Like glScalef, but it takes a single argument: a point
	void glScalePoint(const point& offset) {
		glTranslatef(offset.x(), offset.y(), 0.0f);
	}

	//Render a line segment using OpenGL
	void drawSegment(const segment& seg, float thickness, bool endpoints, bool labels) {
		glLineWidth(thickness);
		glBegin(GL_LINES);
		glVertexSegment(seg);
		glEnd();
		if (endpoints) {
			drawPoint(seg.p1, thickness * 2.0f, labels);
			drawPoint(seg.p2, thickness * 2.0f, labels);
		}
	}


	void drawTriangle(const triangle& tri, bool filled, bool vertices, float pointsize, bool labels,
		bool fermatPoint) {
		if (filled)
			glBegin(GL_TRIANGLES);
		else
			glBegin(GL_LINE_LOOP);
		glVertexTriangle(tri);
		glEnd();
		if (vertices) {
			glBegin(GL_POINTS);
			glVertexTriangle(tri);
			glEnd();
			drawPoint(tri.p1, pointsize, labels);
			drawPoint(tri.p2, pointsize, labels);
			drawPoint(tri.p3, pointsize, labels);
		}
		if (fermatPoint) {
			drawPoint(tri.fermatpoint(0), pointsize, labels);
		}
	}

	//bounds.p1 is bottom-left, bounds.p2 is top-right.
	void drawXYgrid(segment bounds, unsigned int levels) {
		unsigned int level = 1;
		while (level <= levels) {
			glLineWidth(float(levels) - float(level) + 1.0f);
			float lightness = 1.0f / float(level);
			glColor3f(lightness, lightness, lightness);
			glBegin(GL_LINES);
			for (unsigned int i = 1; i < level; i++) {
				bounds.p1 *= 2.0f; bounds.p1 *= 2.0f;
				bounds.p2 *= 2.0f; bounds.p2 *= 2.0f;
			}
			int xfirst, xlast, yfirst, ylast;
			if (bounds.p1.x() < 0)
				xfirst = int(floor(bounds.p1.x()));
			else
				xfirst = int(ceil(bounds.p1.x()));
			if (bounds.p1.y() < 0)
				yfirst = int(floor(bounds.p1.y()));
			else
				yfirst = int(ceil(bounds.p1.y()));
			if (bounds.p2.x() > 0)
				xlast = int(floor(bounds.p2.x()));
			else
				xlast = int(ceil(bounds.p2.x()));
			if (bounds.p2.y() > 0)
				ylast = int(floor(bounds.p2.y()));
			else
				ylast = int(ceil(bounds.p2.y()));
			for (unsigned int i = 1; i < level; i++) {
				bounds.p1 /= 2.0f; bounds.p1 /= 2.0f;
				bounds.p2 /= 2.0f; bounds.p2 /= 2.0f;
			}
			float lv = float(level);
			for (int i = xfirst; i <= xlast; i++) {
				float it = float(i);
				glVertex2f(it / powf(2, lv - 1), bounds.p1.y()); glVertex2f(it / powf(2, lv - 1), bounds.p2.y());
			}
			for (int i = yfirst; i <= ylast; i++) {
				float it = float(i);
				glVertex2f(bounds.p1.x(), it / powf(2, lv - 1)); glVertex2f(bounds.p2.x(), it / powf(2, lv - 1));
			}
			glEnd();
			level++;
		}
	}

	//Use OpenGL to draw a point on the screen
	void drawPoint(const point &dot) {
		glColor3f(1, 1, 1);
		glTranslatef(dot.x(), dot.y(), 0.0f);
		glutSolidSphere(0.05, 10, 10);
		glTranslatef(-dot.x(), -dot.y(), 0.0f);
		glColor3f(0, 0, 0);
		glTranslatef(dot.x(), dot.y(), 0.0f);
		glutSolidSphere(0.025, 10, 10);
		glTranslatef(-dot.x(), -dot.y(), 0.0f);
	}

	//Use OpenGL to draw a segment on the screen
	void drawsegment(segment &seg) {
		glBegin(GL_LINES);
		glVertex2f(seg.p1.x(), seg.p1.y());
		glVertex2f(seg.p2.x(), seg.p2.y());
		glEnd();
	}

	//Some premade colors
	namespace colors {
		const fcolor Black(0.0f, 0.0f, 0.0f);
		const fcolor Red(1.0f, 0.0f, 0.0f);
		const fcolor Orange(1.0f, 0.5f, 0.0f);
		const fcolor Yellow(1.0f, 1.0f, 0.0f);
		const fcolor Lime(0.5f, 1.0f, 0.0f);
		const fcolor Green(0.0f, 1.0f, 0.0f);
		const fcolor Teal(0.0f, 1.0f, 0.5f);
		const fcolor Cyan(0.0f, 1.0f, 1.0f);
		const fcolor Indigo(0.0f, 0.5f, 1.0f);
		const fcolor Blue(0.0f, 0.5f, 1.0f);
		const fcolor Purple(0.5f, 0.0f, 1.0f);
		const fcolor Magenta(1.0f, 0.0f, 1.0f);
		const fcolor Violet(1.0f, 0.0f, 0.5f);
		const fcolor White(1.0f, 0.0f, 1.0f);
	}

	//Get a discrete hue from an integer 0-13 inclusive
	fcolor colorfromID(int colorID) {
		switch (colorID) {
		case 0:
			return colors::Black;
		case 1:
			return colors::Red;
		case 2:
			return colors::Orange;
		case 3:
			return colors::Yellow;
		case 4:
			return colors::Lime;
		case 5:
			return colors::Green;
		case 6:
			return colors::Teal;
		case 7:
			return colors::Cyan;
		case 8:
			return colors::Indigo;
		case 9:
			return colors::Blue;
		case 10:
			return colors::Purple;
		case 11:
			return colors::Magenta;
		case 12:
			return colors::Violet;
		default:
		case 13:
			return colors::White;
		}
	}


	//Use openGL to render a glyph at the origin of the matrix
	void draw(const fgr::glyph &obj) {
		glBegin(obj.mode);
			obj.applyToAll(glVertexPoint);
		glEnd();
	}

	//Use openGL to render a shape at the origin of the matrix
	void draw(const fgr::shape &obj) {
		setcolor(obj.color);
		glLineWidth(obj.lineThickness);
		glPointSize(obj.pointSize);
		glBegin(obj.mode);
			obj.applyToAll(glVertexPoint);
		glEnd();
	}

	//Use openGL to render a graphic at the origin of the matrix
	void draw(const fgr::graphic& obj) {
		obj.applyToAll(draw);
	}

	//Use openGL to render an animation at the correct frame
	void draw(const fgr::animation& obj) {
		draw(*(obj.currentframe));
	}

	//Use openGL to render an animation at the correct frame, and advance it
	void animate(fgr::animation& obj) {
		draw(obj.feed());
	}

	//Use openGL to render the current form of a spritesheet
	void draw(const fgr::spritesheet& obj) {

	}

	//Transform according to a fractal's instructions
	void fractalTransform(const fgr::fractal_mantle& instructions) {
		glTranslatePoint(instructions.location);
		glScalef(instructions.scale, instructions.scale, instructions.scale);
		glRotatef(instructions.rotation / PI * 180, 0.0f, 0.0f, 1.0f);
	}
	
	//Use openGL to render a fractal
	void draw(const fgr::fractal& pattern, int depth) {
		glMatrixMode(GL_MODELVIEW);
		if (depth == 0)
			return;
		//Draw the current level
		draw(pattern);
		//Draw every other level
		for (std::size_t i = 0; i < pattern.branchPoints.size(); ++i) {
			glPushMatrix();
				fractalTransform(pattern.branchPoints[i]);
				draw(pattern, depth - 1);
			glPopMatrix();
		}
		return;
	}
}


#pragma once
