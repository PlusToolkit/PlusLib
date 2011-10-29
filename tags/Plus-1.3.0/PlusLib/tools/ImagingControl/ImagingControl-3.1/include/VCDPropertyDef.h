
#ifndef VCDPROPERTYDEF_H_INC_
#define VCDPROPERTYDEF_H_INC_

#pragma once

#include "IVCDPropertyInterfaces.h"

namespace _DSHOWLIB_NAMESPACE
{
    static const GUID VCDInterface_Range	=	IID_IVCDRangeProperty;
	static const GUID VCDInterface_Switch =	IID_IVCDSwitchProperty;
	static const GUID VCDInterface_Button =	IID_IVCDButtonProperty;
	static const GUID VCDInterface_MapStrings =	IID_IVCDMapStringsProperty;
	static const GUID VCDInterface_AbsoluteValue =	IID_IVCDAbsoluteValueProperty;
};

#endif // VCDPROPERTYDEF_H_INC_
