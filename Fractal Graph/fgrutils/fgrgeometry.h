//This header file includes the geometric methods for drawing fgr objects
#pragma once
#ifndef __geometry_h__
#define __geometry_h__
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>

namespace fgr {
	//PI defined here
	const float PI = 3.14159265358979f;

	//Takes the arithmatic mean of any two number types
	template <class number>
	inline number mean(const number& num1, const number& num2) {
		return (num1 + num2) / 2;
	}

	//Takes the hypotenuse of the triangle with legs of length a and b
	inline float pyth(const float& a, const float& b) {
		return sqrt(pow(a, 2) + pow(b, 2));
	}

	//Boolean for whether X lies between A and B. Think A < X < B.
	inline bool between(float A, float X, float B) {
		//return ((A < X && X < B) || (B < X && X < A));
		//or...
		return (fminf(A, B) < X && X < fmaxf(A, B));
		//The second one looks better, but the first one might be barely faster due to a function call
	}

	//Stores an x and a y value, a two-dimensional co-ordinate
	class point {
	private:
		//Keep track of whether a value has changed
		bool needsUpdate;
		//Remembers its value to make some calculations faster
		float angCache;
		float magCache;
		float xCache;
		float yCache;
	public:
		//Checks if the cache-values for x and y are up-to-date, updates them if not
		void updateCache() {
			if (needsUpdate) {
				needsUpdate = false;
				angCache = atan2(yCache, xCache);
				magCache = pyth(xCache, yCache);
			}
		}
		//Const-qualified accessors
		float x() const {
			return xCache;
		}
		float y() const {
			return yCache;
		}
		//Set the x-coordinate of this point
		void x(float newval) {
			xCache = newval;
			needsUpdate = true;
		}
		//Set the y-coordinate of this point
		void y(float newval) {
			yCache = newval;
			needsUpdate = true;
		}
		//Increment the x-coordinate by some degree
		void xinc(float addon) {
			xCache += addon;
			needsUpdate = true;
		}
		//Increment the y-coordinate by some degree
		void yinc(float addon) {
			yCache += addon;
			needsUpdate = true;
		}
		//Decrement the x-coordinate by some degree
		void xdec(float subtract) {
			xCache -= subtract;
			needsUpdate = true;
		}
		//Decrement the y-coordinate by some degree
		void ydec(float subtract) {
			yCache -= subtract;
			needsUpdate = true;
		}
		//Multiply the x-coordinate by some degree
		void xmult(float factor) {
			xCache *= factor;
			needsUpdate = true;
		}
		//Multiply the y-coordinate by some degree
		void ymult(float factor) {
			yCache *= factor;
			needsUpdate = true;
		}
		//Divide the x-coordinate by some degree
		void xdiv(float divisor) {
			xCache /= divisor;
			needsUpdate = true;
		}
		//Divide the y-coordinate by some degree
		void ydiv(float divisor) {
			yCache /= divisor;
			needsUpdate = true;
		}
		//Constructor given y and x
		point(float xpos, float ypos) {
			needsUpdate = true;
			xCache = xpos;
			yCache = ypos;
			updateCache();
		}
		//Default constructor
		point() {
			needsUpdate = true;
			xCache = 0;
			yCache = 0;
			updateCache();
		}
		//Copy constructor
		point(const point& other) {
			*this = other;
			updateCache();
		}
		//Time to overload some operators!
		// += overload
		point& operator +=(const point& rhs) {
			xCache += rhs.x();
			yCache += rhs.y();
			needsUpdate = true;
			return *this;
		}
		// + overload
		friend point operator +(point lhs, const point& rhs) {
			lhs += rhs;
			return lhs;
		}
		// -= overload
		point& operator-= (const point& rhs) {
			xCache -= rhs.x();
			yCache -= rhs.y();
			needsUpdate = true;
			return *this;
		}
		// - overload
		friend point operator- (point lhs, const point& rhs) {
			lhs -= rhs;
			return lhs;
		}
		// *= overload
		point& operator*= (float rhs) {
			xCache *= rhs;
			yCache *= rhs;
			needsUpdate = true;
			return *this;
		}
		// /= overload
		point& operator/= (float rhs) {
			xCache /= rhs;
			yCache /= rhs;
			needsUpdate = true;
			return *this;
		}
		bool operator== (point& p) const {
			return ((xCache == p.x()) && (yCache == p.y()));
		}
		bool operator!= (point& p) const {
			return !(*this == p);
		}
		// * overload
		friend point operator* (point lhs, const float& rhs) {
			lhs *= rhs;
			return lhs;
		}
		//The pythagorean hypotenuse of thes point from the origin
		float magnitude() const {
			if (needsUpdate) {
				//std::cerr << "Heavy magnitude lifting at " << xCache << ", " << yCache << '!' << std::endl;
				return pyth(xCache, yCache);
			}
			return magCache;
		}
		//The angle formed by this point's std::vector from the origin with respect to the x-axis
		float angle() const {
			if (needsUpdate) {
				//std::cerr << "Heavy angle lifting at " << xCache << ", " << yCache << '!' << std::endl;
				return atan2(yCache, xCache);
			}
			return angCache;
		}
		//Returns a nice label for this point for debug/gui purposes
		std::string label() const {
			return "(" + std::to_string(xCache) + "," + std::to_string(yCache) + ")";
		}
	};

	//Returns true if the points are at the exact same location
	inline bool converges(point PointA, point PointB) {
		return (PointA.x() == PointB.x() && PointA.y() == PointB.y());
	}

	//Returns the scalar product of the point and float passed in
	inline point scalarproduct(point &pointe, float &coefficient) {
		return pointe * coefficient;
	}

	//Returns the scalar product of the point and float passed in
	inline point scalarproduct(const point &pointe, const float &coefficient) {
		return pointe * coefficient;
	}

	//Returns the dot product of the point and float passed in
	inline float dotproduct(point& r1, point& r2) {
		return (r1.x() * r2.x() + r1.y() * r2.y());
	}

	//Returns a float, since this product will by definition be in the z-direction
	inline float flatcrossproduct(const point& r1, const point& r2) {
		return r1.x() * r2.y() - r1.y() * r2.x();
	}

	//Convert a point down into a unit std::vector - maintains direction, magnitude becomes 1
	inline point unitvector(const point po) {
		if (po.magnitude() == 0)
			return point(1.0f, 0.0f);
		return scalarproduct(po, (1 / po.magnitude()));
	}

	//Returns the unit std::vector corresponding to an angle in radians.
	inline point unitfromangle(float angle) {
		point unit;
		unit.x(cos(angle));
		unit.y(sin(angle));
		unit = unitvector(unit); //Just to be sure.
		return unit;
	}

	inline point combine(const point& point1, const point& point2) {
		return point(point1.x() + point2.x(), point1.y() + point2.y());
	}

	//Point 1 - Point 2
	inline point difference(const point& point1, const point& point2) {
		return point1 - point2;
	}

	inline point rotate90(point poi) {
		return point(0 - poi.y(), poi.x());
	}

	inline point pointSum(const std::vector<point>& points) {
		point result = point(0.0f, 0.0f);
		for (point bit : points) {
			result = combine(result, bit);
		}
		return result;
	}

	//Accepts two points and an angle, returns the first point rotated by the specified angle about the second.
	inline point rotateabout(point& arg, const point &axis, float angle) {
		arg -= axis;
		float length = arg.magnitude();
		float newTheta = arg.angle() + angle;
		point newp(length * cos(newTheta), length * sin(newTheta));
		return (newp + axis);
	}

	class segment {
	public:
		point p1;
		point p2;
		void define(const point& point1, const point& point2) {
			p1 = point1;
			p2 = point2;
		}
		const point midpoint() const {
			return point(mean(p1.x(), p2.x()), mean(p1.y(), p2.y()));
		}
		float length() {
			float leng;
			leng = pyth(p1.x() - p2.x(), p1.y() - p2.y());
			return leng;
		}
		//Height of box bounded by this segment. Can be negative.
		const float height() const {
			return (p2.y() - p1.y());
		}
		//Width of box bounded by this segment. Can be negative.
		const float width() const {
			return (p2.x() - p1.x());
		}
		segment() {
			p1 = point(0.0f, 0.0f);
			p2 = point(0.0f, 0.0f);
		}
		segment(float x1, float y1, float x2, float y2) {
			p1 = point(x1, y1);
			p2 = point(x2, y2);
		}
		segment(point P1, point P2) {
			p1 = P1;
			p2 = P2;
		}
		//Returns true if the specified point is within the bounds
		bool contains(const point& spot) const {
			return between(p1.x(), spot.x(), p2.x()) || between(p1.y(), spot.y(), p2.y());
		}
	};

	inline segment rotate90about(int pointID, segment& seg) {
		segment nseg;
		if (pointID == 1)
			nseg = seg;
		else
			nseg.define(seg.p2, seg.p1);
		//point analog; //UNcomment if fermatpoint stops working
		//analog.x() = nseg.p2.x() - nseg.p1.x(); //UNcomment if fermatpoint stops working
		//analog.y() = nseg.p2.y() - nseg.p1.y(); //UNcomment if fermatpoint stops working
		point analog = nseg.p2 - nseg.p1; //comment OUT if fermatpoint stops working
		analog = rotate90(analog);
		nseg.p2 = combine(nseg.p1, analog);
		return nseg;
	}

	inline segment equilateralBisector(segment seg) { //Bisector protrudes from left, if p1 is bottom and p2 is top
		segment perp;
		perp.p1 = seg.midpoint();
		point diffp = difference(perp.p2, perp.p1);
		perp.p2 = combine(perp.p1, scalarproduct(unitvector(difference(rotate90about(1, seg).p2, seg.p1)), sqrt(3.0f) / 2));
		return perp;
	}

	//Returns the point in two-dimensional space at which two lines intersect (treated as full lines, not segments)
	inline point intersection(const segment& sega, const segment& segb) { //Figured this out using Cramer's Rule 
		float dxa = sega.p2.x() - sega.p1.x();
		float dxb = segb.p2.x() - segb.p1.x();
		float dya = sega.p2.y() - sega.p1.y();
		float dyb = segb.p2.y() - segb.p1.y();
		float xa = sega.p1.x();
		float xb = segb.p1.x();
		float ya = sega.p1.y();
		float yb = segb.p1.y();
		float pa = dxa * ya - dya * xa;
		float pb = dxb * yb - dyb * xb;
		float det = dya * dxb - dyb * dxa;
		float dety = dya * pb - dyb * pa;
		float detx = dxa * pb - dxb * pa;
		float Y = dety / det;
		float X = detx / det;
		point solution(X, Y);
		return solution;
	}

	//This is showing some REALLY weird behavior when one of the segments is perfectly vertical or horizontal
	inline bool isintersect(const segment& sega, const segment& segb) {
		point ints = intersection(sega, segb);
		bool eval = true;
		bool xaeval = (ints.x() < sega.p1.x()) == (ints.x() > sega.p2.x());
		if (sega.p1.x() == sega.p2.x())
			xaeval = false;
		bool xbeval = (ints.x() < segb.p1.x()) == (ints.x() > segb.p2.x());
		if (segb.p1.x() == segb.p2.x())
			xbeval = false;
		bool yaeval = (ints.y() < sega.p1.y()) == (ints.y() > sega.p2.y());
		if (sega.p1.y() == sega.p2.y())
			yaeval = false;
		bool ybeval = (ints.y() < segb.p1.y()) == (ints.y() > segb.p2.y());
		if (segb.p1.y() == segb.p2.y())
			ybeval = false;
		if (!xaeval && !yaeval)
			eval = false;
		if (!xbeval && !ybeval)
			eval = false;
		//if (!yaeval)
		//	eval = false;
		//if (!ybeval)
		//	eval = false;
		/*if (eval) {
			cerr << "SOMETHING IS WRONG" << endl
				<< (eval? "TRUE" : "FALSE") << endl
				<< ((onSegment(sega.p1, ints, sega.p2) && onSegment(segb.p1, ints, segb.p2)) ? "TRUE" : "FALSE") << endl
				<< "Seg 1: (" << sega.p1.x() << ", " << sega.p1.y() << "), ("  << sega.p2.x() << ", " << sega.p2.y() << ")" << endl
				<< "Seg 2: (" << segb.p1.x() << ", " << segb.p1.y() << "), (" << segb.p2.x() << ", " << segb.p2.y() << ")" << endl;
			exit(1);
		}*/
		return eval;
	}

	inline bool isperpintersect(const point& p, const segment& v) {
		point diffVp = difference(v.p1, p);
		point diffVv = difference(v.p2, v.p1);
		float numerator = dotproduct(diffVp, diffVv);
		float denominator = dotproduct(diffVv, diffVv);
		float t = -numerator / denominator;
		return  (0 <= t && t <= 1);
	}

	inline float distancetoline(const point& p, const segment& v) {
		point diffPv = difference(v.p1, p);
		point diffVv = difference(v.p2, v.p1);
		float numerator = abs(flatcrossproduct(diffVv, diffPv));
		float denominator = diffVv.magnitude();
		return numerator / denominator;
	}

	//Returns a position std::vector representing the shortest path from a point to a segment
	inline float distancetoseg(const point& dot, const segment& seg) {
		//First we see if the perpendicular line to the segment even hits it.
		if (!isperpintersect(dot, seg)) {
			return (std::min)(
				difference(dot, seg.p1).magnitude(),
				difference(dot, seg.p2).magnitude()
			);
		}
		//Otherwise we find the perpendicular path to the segment
		return distancetoline(dot, seg);
	}

	//Returns the given point after refection about an axis defined by a given segment
	inline point reflection(point dot, const segment &mirror) {
		point transform = mirror.midpoint(); //Bring the process to the origin
		dot = difference(dot, transform);
		point mir = difference(mirror.p1, transform);
		float dang = mir.angle() - dot.angle();
		float nang = dot.angle() + dang * 2;
		float nx = dot.magnitude() * cos(nang);
		float ny = dot.magnitude() * sin(nang);
		point reflect(nx, ny);
		reflect = combine(reflect, transform);
		return reflect;
	}

	/*Returns the appropriate reflective bisector at a given intersection that reflects an incedent ray as if
	it reflected at that corner, given a point that lies on the incedent ray.*/
	inline segment reflectiveBisector(const point &dot, const segment &sega, const segment &segb) {
		point corner = intersection(sega, segb);
		point A = sega.p1;
		point B = segb.p1;
		point C = dot;
		A = difference(A, corner);
		B = difference(B, corner);
		C = difference(C, corner);
		A = unitvector(A);
		B = unitvector(B);
		C = unitvector(C);
		float angleA = A.angle();
		if (angleA < 0)
			angleA += PI * 2.0f;
		if (angleA > PI)
			angleA -= PI;
		float angleB = B.angle();
		if (angleB < 0)
			angleB += PI * 2.0f;
		if (angleB > PI)
			angleB -= PI;
		float angleC = C.angle();
		if (angleC < 0)
			angleC += PI * 2.0f;
		if (angleC > PI)
			angleC -= PI;
		float bisectorAngle1 = mean(angleA, angleB);
		bool useAngle1 = !between(angleA, angleC, angleB);
		angleB += PI;
		if (angleB > PI * 2.0f)
			angleB -= PI * 2.0f;
		float bisectorAngle2 = mean(angleA, angleB);
		if (bisectorAngle1 > PI)
			bisectorAngle1 -= PI;
		if (bisectorAngle2 > PI)
			bisectorAngle2 -= PI;
		float diff1 = abs(bisectorAngle1 - angleC);
		float diff2 = abs(bisectorAngle2 - angleC);
		//float properAngle = (diff1>diff2)? bisectorAngle1: bisectorAngle2;
		//New logic
		float properAngle = (useAngle1) ? bisectorAngle1 : bisectorAngle2;
		point bisectorUnit = unitfromangle(properAngle);
		point bisectorP1 = bisectorUnit;
		point bisectorP2 = scalarproduct(bisectorUnit, -1.0f);
		bisectorP1 = combine(bisectorP1, corner);
		bisectorP2 = combine(bisectorP2, corner);
		segment bisector(bisectorP1, bisectorP2);
		return bisector;
	}

	class circle {
	public:
		point center;
		float radius;
		circle() {
			center = point(0, 0);
			radius = 0.0f;
		}
		circle(point& centre, float rad) {
			center = centre;
			radius = rad;
		}
		float circumfrence() { return PI * radius * 2; }
		float area() { return PI * radius * radius; }
	};

	class triangle {
	public:
		point p1;
		point p2;
		point p3;
		segment s1() const {
			segment seg1(p2, p3);
			return seg1;
		}
		segment s2() const {
			segment seg1(p3, p1);
			return seg1;
		}
		segment s3() const {
			segment seg1(p1, p2);
			return seg1;
		}
		triangle(point a, point b, point c) {
			p1 = a;
			p2 = b;
			p3 = c;
		}
		//1, 2, or 3 for specific estimates, anything else for the average of all 3.
		point fermatpoint(int which) const {
			point fermA;
			point fermB;
			point fermC;
			point fermM;
			point A = equilateralBisector(s1()).p2;
			point B = equilateralBisector(s2()).p2;
			point C = equilateralBisector(s3()).p2;
			segment finder1;
			finder1.define(p1, A);
			segment finder2;
			finder2.define(p2, B);
			segment finder3;
			finder3.define(p3, C);
			fermA = intersection(finder2, finder3);
			fermB = intersection(finder3, finder1);
			fermC = intersection(finder1, finder2);
			fermM.x((fermA.x() + fermB.x() + fermC.x()) / 3.0f);
			fermM.y((fermA.y() + fermB.y() + fermC.y()) / 3.0f);
			if (which == 1)
				return fermA;
			else if (which == 2)
				return fermB;
			else if (which == 3)
				return fermC;
			else
				return fermM;
		}
	};
}

#endif