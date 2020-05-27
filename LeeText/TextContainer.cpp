#include "TextContainer.h"
using namespace LeeText;

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
   this->cursorX = 0;
   this->cursorY = 0;
   for (auto c : this->text)
   {
      line.push_back(c);
      if (totalChars+1 == this->cursorIdx)
      {
         this->cursorX = line.length();
         this->cursorY = lines.size();
         if (c == L'\n')
         {
            this->cursorX = 0;
            this->cursorY++;
         }
      }
      totalChars++;
      numCharsThisLine++;
      if (c == L'\n' || numCharsThisLine == maxLen)
      {
         lines.push_back(line);
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

bool LeeText::TextContainer::saveToFile(std::wstring filename) const
{
   bool save_successful = false;
   std::wofstream outfile;
   outfile.open(filename);
   if (outfile.is_open())
   {
      outfile << this->text;
      outfile.close();
      save_successful = true;
   }
   return save_successful;
}

void TextContainer::loadFromFile(std::wstring filename)
{
   std::wifstream infile;
   infile.open(filename);
   if (infile.is_open())
   {
      this->text.clear();
      this->cursorIdx = 0;
      this->cursorX = 0;
      this->cursorY = 0;
      std::wstring line;
      while (std::getline(infile, line))
      {
         this->text.append(line+L'\n');
      }
      infile.close();
   }
  
}

void TextContainer::moveCursor(int idxMove)
{
   if (idxMove < 0)
   {
      if (static_cast<long long>(this->cursorIdx) + idxMove <= 0)
      {
         this->cursorIdx = 0;
      }
      else
      {
         this->cursorIdx += idxMove;
      }
   }
   else
   {
      if (this->cursorIdx + idxMove >= this->text.size())
      {
         this->cursorIdx = this->text.size();
      }
      else 
      {
         this->cursorIdx += idxMove;
      }
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

