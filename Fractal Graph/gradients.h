#pragma once
#ifndef __GRADIENTS_H__
#define __GRADIENTS_H__
#include <vector>
#include "fgrutils.h"

typedef std::vector < std::pair<long double, fgr::fcolor> > gradient;

//A gradient is assumed to have its frist t = 0
fgr::fcolor mapgradient(long double t, const gradient& colorscheme) {
	assert(t >= 0);
	unsigned int i = 0;
	while (i < colorscheme.size() && colorscheme[i].first <= t)
		++i;
	if (i >= colorscheme.size())
		return colorscheme.back().second;
	fgr::fcolor diffc(colorscheme[i].second - colorscheme[i - 1].second);
	long double difft = (colorscheme[i].first) - ( colorscheme[i - 1].first);
	t -= colorscheme[i - 1].first;
	long double frac = t / difft;
	fgr::fcolor retfc = colorscheme[i - 1].second + diffc * frac;
	return retfc;
}

#endif