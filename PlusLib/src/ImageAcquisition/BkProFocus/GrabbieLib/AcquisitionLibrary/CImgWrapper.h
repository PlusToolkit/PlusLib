#pragma once

/// \brief This file is a wrapper for the CImg library (check out
/// CImg.h for more details). The wrapper is used to suppress a 
/// number of warnings. Include this file, not CImg.h
#pragma warning(push)
#pragma warning(disable : 4127 4244 4267 4310 4706 4793)
#pragma warning(disable : 6011 6031 6053 6211 6246 6255 6258 6262 6297 6326 6336 6386)
#include "CImg.h"
#pragma warning(pop)