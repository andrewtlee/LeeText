#pragma once

// Manages text on screen. For now, let's keep all text in memory as vector. We'll get more clever, later.
// For now, TextContainer can only handle one file.
#include <string>
#include <vector>
#include <fstream>
using WCHAR = wchar_t;

namespace LeeText
{ 

class TextContainer
{
public:

   void insertText(WCHAR c);
   void insertText(std::wstring text);
   void removeText(int numchars);

   [[nodiscard]] std::vector<std::wstring> getLines(int maxLen=-1); // updates cursor row and col; negative maxlen indicates no linewrap.

   [[nodiscard]] bool saveToFile(std::wstring filename) const; // if saving fails, we'd better tell the user.
   void loadFromFile(std::wstring filename); // if loading fails, the user will know one way or another.
   
   void moveCursor(int idxMove);
   int getCursorRow() const;
   int getCursorCol() const;
   void setCursorToEnd();
   void setCursorToBeginning();
private:
   size_t cursorIdx=0;
   int cursorX = 0;
   int cursorY = 0;
   std::wstring text;
};

} // namespace LeeText