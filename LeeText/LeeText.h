#pragma once

#include "resource.h"
#include "TextContainer.h"
#include <algorithm>

namespace LeeText
{

struct LeeTextStatics
{
   bool linewrap = false;
   int cxChar, cxCaps, cyChar, cyClient, iVscrollPos;
   int NUMLINES = 0;
};

} // namespace LeeText