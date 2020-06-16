#pragma once

#include "resource.h"
#include "TextContainer.h"
#include <algorithm>

namespace LeeText
{

struct LeeTextStatics
{
   bool linewrap = false;
   int cyClient = 0;
   int iVscrollPos = 0;
   int NUMLINES = 0;
};

} // namespace LeeText