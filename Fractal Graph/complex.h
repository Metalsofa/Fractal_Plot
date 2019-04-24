#pragma once
//Defines classes and functions for complex mathematics
#ifndef __COMPLEX_VARIABLES_H__
#define __COMPLEX_VARIABLES_H__
#include <iostream>
#include <utility>

//Colmpex long doubleing-point numer
class clong_double {
public:
	// REPRESENTATION
	long double real;
	long double imaginary;
	// Default constructor
	clong_double() { real = 0.0; imaginary = 0.0; }
	//Another constructor
	clong_double(long double real_, long double imaginary_) {
		real = real_; imaginary = imaginary_;
	}
	//Construct from a long double
	clong_double(long double real_) { real = real_; imaginary = 0.0; }
	friend std::ostream& operator<< (std::ostream dest, const clong_double& num) {
		dest << num.real << " + " << num.imaginary << "i ";
	}
	//Operators
	clong_double operator= (const clong_double& other) {
		real = other.real;
		imaginary = other.imaginary;
		return *this;
	}
	clong_double operator+ (const clong_double& other) const {
		return clong_double(real + other.real, imaginary + other.imaginary);
	}
	clong_double operator+ (long double other) const {
		return clong_double(real + other, imaginary);
	}
	clong_double operator- (const clong_double& other) const {
		return clong_double(real - other.real, imaginary + other.imaginary);
	}
	clong_double operator- (long double other) const {
		return clong_double(real - other, imaginary);
	}
	clong_double operator* (const clong_double& other) const {
		return clong_double(real * other.real - imaginary * other.imaginary, imaginary * other.real + real * other.imaginary);
	}
	clong_double operator* (long double other) const {
		return clong_double(real * other, imaginary * other);
	}
	friend clong_double operator/ (long double other, const clong_double& complex) {
		long double denom = complex.real * complex.real + complex.imaginary + complex.imaginary;
		return (clong_double(complex.real / denom, -complex.imaginary / denom) * other);
	}
	friend clong_double operator* (long double other, const clong_double& complex) {
		return complex * other;
	}
	friend clong_double operator+ (long double other, const clong_double& complex) {
		return complex + other;
	}
	friend clong_double operator- (long double other, const clong_double& complex) {
		return clong_double(other - complex.real, -complex.imaginary);
	}
	clong_double operator/ (long double other) const {
		return clong_double(real / other, imaginary / other);
	}
	clong_double operator+= (long double other) {
		*this = *this + other;
		return *this;
	}
	clong_double operator+= (clong_double other) {
		*this = *this + other;
		return *this;
	}
	clong_double operator-= (long double other) {
		*this = *this - other;
		return *this;
	}
	clong_double operator-= (clong_double other) {
		*this = *this - other;
		return *this;
	}
	clong_double operator/= (long double other) {
		*this = *this / other;
		return *this;
	}
	clong_double operator*= (long double other) {
		*this = *this * other;
		return *this;
	}
	clong_double operator*= (clong_double other) {
		*this = *this * other;
		return *this;
	}
	bool operator== (long double other) const {
		return (real == other && imaginary == 0.0);
	}
	friend bool operator== (long double other, const clong_double& complex) {
		return complex == other;
	}
	clong_double& operator= (long double other) {
		real = other;
		imaginary = 0.0;
		return *this;
	}
	long double magnitude() const {
		return sqrt(real * real + imaginary * imaginary);
	}
	long double angle() const {
		return atan2(imaginary, real);
	}
};

//Checks if a given complex number is part of the julia set for a given function (approximately and recursively)
std::pair<bool, int> is_lace(clong_double(*func)(clong_double), clong_double arg, long double threshold, int depth) {
	//Test for convergence approximately (not airtight by any means)
	if ((arg.real * arg.real + arg.imaginary * arg.imaginary) < threshold)
		return std::make_pair(true, depth);
	if (depth == 0) {
		return std::make_pair(false, 0);
	}
	return is_lace(func, func(arg), threshold, depth - 1);
}

//Evaluate a complex fractal plot value for a given complex number
std::pair<bool, int> mandelbrot(clong_double arg, clong_double c, long double threshold, int depth) {
	std::size_t i = 0;
	while ( ( (arg.magnitude() < threshold && fractal_type % 4 < 2) || (fractal_type % 4 >= 2 && c.magnitude() < threshold)) && i < depth) {
		switch (fractal_type % 4) {
		case 0: //Mandelbrot
			arg = arg * arg + c;
			break;
		case 1: //Burning ship
			arg = clong_double(abs(arg.real), abs(arg.imaginary)) * clong_double(abs(arg.real), abs(arg.imaginary)) + c;
			break;
		case 2: //Julia for z^2 + c
			//Arg represents the chosen 'c' now, and c is the location
			c = c * c + arg;
			break;
		case 3:
			c = c * c * c + arg;
		}
		++i;
	}
	if (fractal_type % 4 >= 2) {
		std::swap(arg, c);
		if (arg.magnitude() > threshold) {
			return std::make_pair(true, depth - i);
		}
		return std::make_pair(false, 0);
	}
	//Test for convergence approximately (not airtight by any means)
	if (arg.magnitude() >= threshold)
		return std::make_pair(true, depth - i);
	assert(depth == i);
	return std::make_pair(false, 0);
	//return mandelbrot(z, c, threshold, depth - 1);
}



#endif

