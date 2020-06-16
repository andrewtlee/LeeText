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

size_t LeeText::TextContainer::getNumLines(int maxLen) const
{
   /* This function needs to be lightweight, since it's a utility. 
   We do that by replacing if/else with boolean arithmetic to aid branch prediction. 
   We explicitly say !condition in the multiplication to make common subexpression elimination easier on the compiler.*/
   int numCharsThisLine = 0;
   int numlines = 0;
   for (auto& c : this->text)
   {
      numCharsThisLine++;
      numlines += static_cast<int>((c == L'\n' || numCharsThisLine == maxLen)); 
      numCharsThisLine *= static_cast<int>(!(c == L'\n' || numCharsThisLine == maxLen));
   }
   numlines++;
   return numlines;
}

std::vector<std::wstring> TextContainer::getLines(int scrollPos, int winHeight, int maxLen)
{
   std::wstring currLine;
   std::vector<std::wstring> lines;
   int numCharsThisLine = 0;
   size_t totalChars = 0;
   this->cursorX = 0;
   this->cursorY = 0;
   int numUnprintedLines = 0;
   for (auto& c : this->text)
   {
      if (numUnprintedLines < scrollPos)
      {
         numCharsThisLine++;
         numUnprintedLines += static_cast<int>(c == L'\n' || numCharsThisLine == maxLen); // why are we doing arithmetic with bools? Because it's faster than an if/else, and we're in a loop that'll execute thousands of times per frame.
         numCharsThisLine *= static_cast<int>(!(c == L'\n' || numCharsThisLine == maxLen)); // reset on newline
      }
      else 
      {
         currLine.push_back(c);
         if (totalChars + 1 == this->cursorIdx)
         {
            this->cursorX = currLine.length();
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
            lines.push_back(currLine);
            currLine.clear();
            numCharsThisLine = 0;
         }
         if (lines.size() > winHeight)
         {
            break;
         }
      }
   }
   if (!currLine.empty())
   {
      lines.push_back(currLine);
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

void TextContainer::setCursorToEnd()
{
   this->cursorIdx = this->text.length();
}

void LeeText::TextContainer::setCursorToBeginning()
{
   this->cursorIdx = 0;
}

