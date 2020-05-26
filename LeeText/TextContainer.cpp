#include "TextContainer.h"

void TextContainer::insertText(WCHAR c)
{
   if (c == L'\n')
   {
     
   }
   if (this->text.empty())
   {
      this->text.push_back(c);
   }
   else
   {
      this->text.insert(this->cursorIdx, 1, c);
   }
   this->cursorIdx++;
}

void TextContainer::insertText(std::wstring str)
{
   for (auto& c : str)
   {
      this->insertText(c);
   }
}

void TextContainer::removeText(int numchars)
{
   if (numchars < 0)
   {
      if (static_cast<int>(this->cursorIdx) >= abs(numchars))
      {
         this->cursorIdx += numchars;
      }
      else
      {
         numchars = static_cast<int>(cursorIdx);
      }
   }
   if (numchars < 0 || (this->cursorIdx + abs(numchars)) <= static_cast<int>(this->text.length()))
   {
      auto erasedText = this->text.substr(cursorIdx, abs(numchars));
      int len_to_prev_newline = 0;
      for (auto& c : erasedText)
      {
         if (c == L'\n')
         {
            for (int i = cursorIdx-1; i >= 0; i--)
            {
               if (this->text.at(i) == L'\n')
                  break;
               len_to_prev_newline++;
            }
         }
      }
      this->text.erase(cursorIdx, abs(numchars));
   }
}

std::vector<std::wstring> TextContainer::getLines(int maxLen)
{
   std::wstring line;
   std::vector<std::wstring> lines;
   int numCharsThisLine = 0;
   size_t totalChars = 0;
   for (auto c : this->text)
   {
      line.push_back(c);
      if (totalChars+1 == this->cursorIdx)
      {
         cursorX = line.length();
         cursorY = lines.size();
      }
      totalChars++;
      numCharsThisLine++;
      if (c == L'\n' || numCharsThisLine == maxLen)
      {
         lines.push_back(line);
         this->cursorX = 0;
         this->cursorY++;
         line.clear();
         numCharsThisLine = 0;
      }
   }
   if (!line.empty())
   {
      lines.push_back(line);
   }
   return lines;
}

void TextContainer::moveCursor(CURSORDIRECTION d)
{
   switch (d)
   {
   case CURSORDIRECTION::RIGHT:
      if (this->cursorIdx < this->text.size()) // if we aren't at the end of the text
      {
         if (this->text.at(this->cursorIdx) != L'\n') // if we aren't at the end of the line
         {
            cursorIdx++;
            cursorX++;
         }
      }
      break;
   case CURSORDIRECTION::LEFT:
      if (this->cursorIdx > 0) // if we aren't at the beginning of the text
      {
         if (this->text.at(this->cursorIdx - 1) != L'\n') // if we aren't at the beginning of the line
         {
            cursorIdx--;
            cursorX--;
         }
      }
      break;
   case CURSORDIRECTION::UP:

      break;
   default:
      break;
   }
}

int TextContainer::getCursorRow() const
{
   return this->cursorY;
}

int TextContainer::getCursorCol() const
{
   return this->cursorX;
}

