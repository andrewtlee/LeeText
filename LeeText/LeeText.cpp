// LeeText.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "LeeText.h"
using namespace LeeText;

constexpr int MAX_LOADSTRING = 100;
constexpr int CHAR_HEIGHT = 15;
constexpr int CHAR_WIDTH = 8;
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
TextContainer userText;
std::wstring openFileName;
LeeTextStatics LTinfo;

//undef some WINAPI defines; prefer STL implementations
#undef max
#undef min

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void cursorUp(HWND hWnd, bool linewrap);
void cursorDown(HWND hWnd, bool linewrap);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ LPWSTR    lpCmdLine,
   _In_ int       nCmdShow)
{
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);

   // TODO: Place code here.


   // Initialize global strings
   LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
   LoadStringW(hInstance, IDC_LEETEXT, szWindowClass, MAX_LOADSTRING);
   MyRegisterClass(hInstance);

   // Perform application initialization:
   if (!InitInstance(hInstance, nCmdShow))
   {
      return FALSE;
   }

   HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LEETEXT));

   MSG msg;

   // Main message loop:
   while (GetMessage(&msg, nullptr, 0, 0))
   {
      if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   }

   return static_cast<int>(msg.wParam);
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
   WNDCLASSEXW wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);

   wcex.style = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc = WndProc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LEETEXT));
   wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LEETEXT);
   wcex.lpszClassName = szWindowClass;
   wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

   return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Store instance handle in our global variable


   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VSCROLL,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   HWND hwndFind = nullptr;
   switch (message)
   {
   case WM_COMMAND:
   {
      int wmId = LOWORD(wParam);
      // Parse the menu selections:
      switch (wmId)
      {
      case IDM_ABOUT:
         DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
         break;
      case IDM_EXIT:
         DestroyWindow(hWnd);
         break;
      case IDM_OPEN:
      {
         HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
            COINIT_DISABLE_OLE1DDE);
         if (SUCCEEDED(hr))
         {
            IFileOpenDialog* pFileOpen;

            // Create the FileOpenDialog object.
            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
               IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

            if (SUCCEEDED(hr))
            {
               // Show the Open dialog box.
               hr = pFileOpen->Show(NULL);

               // Get the file name from the dialog box.
               if (SUCCEEDED(hr))
               {
                  IShellItem* pItem;
                  hr = pFileOpen->GetResult(&pItem);
                  if (SUCCEEDED(hr))
                  {
                     PWSTR pszFilePath;
                     hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                     // Display the file name to the user.
                     if (SUCCEEDED(hr))
                     {
                        //MessageBox(NULL, pszFilePath, L"Saved", MB_OK);
                        std::wstring fname(pszFilePath);
                        openFileName = fname;
                        userText.loadFromFile(openFileName);
                        CoTaskMemFree(pszFilePath);
                     }
                     pItem->Release();
                  }
               }
               pFileOpen->Release();
            }
            CoUninitialize();
         }
         RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
      }
      break;
      case IDM_SAVE_AS:
      {
         HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
            COINIT_DISABLE_OLE1DDE);
         if (SUCCEEDED(hr))
         {
            IFileSaveDialog* pFileSave;

            // Create the FileOpenDialog object.
            hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
               IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

            if (SUCCEEDED(hr))
            {
               // Show the Open dialog box.
               hr = pFileSave->Show(NULL);

               // Get the file name from the dialog box.
               if (SUCCEEDED(hr))
               {
                  IShellItem* pItem;
                  hr = pFileSave->GetResult(&pItem);
                  if (SUCCEEDED(hr))
                  {
                     PWSTR pszFilePath;
                     hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                     // Display the file name to the user.
                     if (SUCCEEDED(hr))
                     {
                        //MessageBox(NULL, pszFilePath, L"Saved", MB_OK);
                        std::wstring fname(pszFilePath);
                        openFileName = fname;
                        auto result = userText.saveToFile(openFileName);
                        if (!result)
                        {
                           MessageBox(NULL, pszFilePath, L"Error! Failed to Save!", MB_OK);
                        }
                        CoTaskMemFree(pszFilePath);
                     }
                     pItem->Release();
                  }
               }
               pFileSave->Release();
            }
            CoUninitialize();
         }
      }
      break;
      case IDM_SAVE:
         if (!openFileName.empty())
         {
            auto result = userText.saveToFile(openFileName);
            if (!result)
            {
               //PWSTR pszFilePath = openFileName.c_str();
               MessageBox(NULL, openFileName.c_str(), L"Error!", MB_OK);
            }
         }
         else
         {
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
               COINIT_DISABLE_OLE1DDE);
            if (SUCCEEDED(hr))
            {
               IFileSaveDialog* pFileSave;

               // Create the FileOpenDialog object.
               hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
                  IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

               if (SUCCEEDED(hr))
               {
                  // Show the Open dialog box.
                  hr = pFileSave->Show(NULL);

                  // Get the file name from the dialog box.
                  if (SUCCEEDED(hr))
                  {
                     IShellItem* pItem;
                     hr = pFileSave->GetResult(&pItem);
                     if (SUCCEEDED(hr))
                     {
                        PWSTR pszFilePath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                        // Display the file name to the user.
                        if (SUCCEEDED(hr))
                        {
                           //MessageBox(NULL, pszFilePath, L"Saved", MB_OK);
                           std::wstring fname(pszFilePath);
                           openFileName = fname;
                           auto result = userText.saveToFile(openFileName);
                           if (!result)
                           {
                              MessageBox(NULL, pszFilePath, L"Error! Failed to Save!", MB_OK);
                           }
                           CoTaskMemFree(pszFilePath);
                        }
                        pItem->Release();
                     }
                  }
                  pFileSave->Release();
               }
               CoUninitialize();
            }
         }
         break;
      case IDM_TOGGLEWRAP:
         LTinfo.linewrap = !LTinfo.linewrap;
         RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
         break;
      case IDM_FIND:
         MessageBox(NULL, L"Find function coming soon!", L"Oops!", MB_OK);
         break;
      default:
         return DefWindowProc(hWnd, message, wParam, lParam);
      }
   }
   break;
   case WM_SIZE:
      LTinfo.cyClient = HIWORD(lParam);
      return 0;
   case WM_VSCROLL:
      switch (LOWORD(wParam))
      {
      case SB_LINEUP:
         LTinfo.iVscrollPos -= 1;
         break;

      case SB_LINEDOWN:
         LTinfo.iVscrollPos += 1;
         break;

      case SB_PAGEUP:
         LTinfo.iVscrollPos -= LTinfo.cyClient / CHAR_HEIGHT;
         break;

      case SB_PAGEDOWN:
         LTinfo.iVscrollPos += LTinfo.cyClient / CHAR_HEIGHT;
         break;

      case SB_THUMBPOSITION:
         LTinfo.iVscrollPos = HIWORD(wParam);
         break;

      default:
         break;
      }

      LTinfo.iVscrollPos = std::max(0, std::min(LTinfo.iVscrollPos, LTinfo.NUMLINES - 1));

      if (LTinfo.iVscrollPos != GetScrollPos(hWnd, SB_VERT))
      {
         SetScrollPos(hWnd, SB_VERT, LTinfo.iVscrollPos, TRUE);
         InvalidateRect(hWnd, NULL, TRUE);
      }
      return 0;
   case WM_PAINT:
   {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      // Retrieve a handle to the fixed stock font.  
      HFONT hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
      SelectObject(hdc, hFont);
      int linenum = 0;
      RECT rect;
      GetWindowRect(hWnd, &rect);
      int width = rect.right - rect.left;
      int height = rect.bottom - rect.top;
      int widthInChars = width / CHAR_WIDTH - 2;
      int heightInChars = height / CHAR_HEIGHT - 2;
      if (LTinfo.linewrap)
      {
         auto textToPrint = userText.getLines(LTinfo.iVscrollPos, heightInChars, widthInChars);
         for (auto& line : textToPrint)
         {
            TextOut(hdc, 0, linenum * CHAR_HEIGHT, line.c_str(), line.length());
            linenum++;
         }
         LTinfo.NUMLINES = userText.getNumLines(widthInChars);
      }
      else
      {
         auto textToPrint = userText.getLines(LTinfo.iVscrollPos, heightInChars);
         for (auto& line : textToPrint)
         {
            TextOut(hdc, 0, linenum * CHAR_HEIGHT, line.c_str(), line.length());
            linenum++;
         }
         LTinfo.NUMLINES = userText.getNumLines();
      }
      SetCaretPos(userText.getCursorCol() * CHAR_WIDTH, userText.getCursorRow() * CHAR_HEIGHT);
      SetScrollRange(hWnd, SB_VERT, 0, LTinfo.NUMLINES - 1, FALSE);
      SetScrollPos(hWnd, SB_VERT, LTinfo.iVscrollPos, TRUE);
      EndPaint(hWnd, &ps);
   }
   break;
   case WM_CHAR:
   {
      if (wParam == L'\t')
      {
         userText.insertText(L"    "); // we only use spaced tabs in this house.
      }
      else if (wParam == VK_RETURN)
      {
         userText.insertText(L'\n');
      }
      else if (isprint(wParam))
      {
         userText.insertText(wParam);
      }
      RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
   }
   break;
   case WM_KEYDOWN:
   {
      bool redraw = false;
      switch (wParam)
      {
      case VK_DELETE:
         userText.removeText(1);
         redraw = true;
         break;
      case VK_BACK:
         userText.removeText(-1);
         redraw = true;
         break;
      case VK_RIGHT:
         userText.moveCursor(1);
         redraw = true;
         break;
      case VK_LEFT:
         userText.moveCursor(-1);
         redraw = true;
         break;
      case VK_UP:
         cursorUp(hWnd, LTinfo.linewrap);
         redraw = true;
         break;
      case VK_DOWN:
         cursorDown(hWnd, LTinfo.linewrap);
         redraw = true;
         break;
      default:
         break;
      }
      if (redraw)
      {
         RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
      }
   }
   break;
   case WM_SETFOCUS:
      CreateCaret(hWnd, (HBITMAP)nullptr, 2, 15);
      SetCaretPos(userText.getCursorCol() * CHAR_WIDTH, userText.getCursorRow() * CHAR_HEIGHT);
      ShowCaret(hWnd);
      break;
   case WM_LBUTTONDOWN:
   /*{
      int cursorX = static_cast<int>(LOWORD(lParam)) / CHAR_WIDTH;
      int cursorY = static_cast<int>(HIWORD(lParam)) / CHAR_HEIGHT;
      
      bool up = userText.getCursorRow() > cursorY;
      for (int i=0; i < abs(userText.getCursorRow()-cursorY); i++)
      {   
         if (up)
         {
            cursorUp(hWnd, LTinfo.linewrap);
         }
         else
         {
            cursorDown(hWnd, LTinfo.linewrap);
         }
      }
      //userText.moveCursor(cursorX - userText.getCursorCol());
      RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
   }*/
   break;
   case WM_DESTROY:
      PostQuitMessage(0);
      break;
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void cursorUp(HWND hWnd, bool linewrap)
{
   std::vector<std::wstring> lines;
   RECT rect;
   GetWindowRect(hWnd, &rect);
   int width = rect.right - rect.left;
   int height = rect.bottom - rect.top;
   int widthInChars = width / 8 - 2;
   int heightInChars = height / CHAR_HEIGHT - 2;
   if (linewrap)
   {
      lines = userText.getLines(LTinfo.iVscrollPos, heightInChars, widthInChars);
   }
   else
   {
      lines = userText.getLines(LTinfo.iVscrollPos, heightInChars);
   }
   if (userText.getCursorRow())
   {
      auto currLine = lines.at(userText.getCursorRow());
      auto prevLine = lines.at(userText.getCursorRow() - static_cast<size_t>(1));
      auto move = static_cast<int>(std::max(currLine.length() - currLine.substr(userText.getCursorCol()).length() + 1, prevLine.length()));
      userText.moveCursor(-move);
   }
}

void cursorDown(HWND hWnd, bool linewrap)
{
   std::vector<std::wstring> lines;
   RECT rect;
   GetWindowRect(hWnd, &rect);
   int width = rect.right - rect.left;
   int height = rect.bottom - rect.top;
   int widthInChars = width / 8 - 2;
   int heightInChars = height / CHAR_HEIGHT - 2;
   if (linewrap)
   {
      lines = userText.getLines(LTinfo.iVscrollPos, heightInChars, widthInChars);
   }
   else
   {
      lines = userText.getLines(LTinfo.iVscrollPos, heightInChars);
   }
   if (userText.getCursorRow() < static_cast<int>(lines.size()) - 1)
   {
      auto currLine = lines.at(userText.getCursorRow());
      auto nextLine = lines.at(userText.getCursorRow() + static_cast<size_t>(1));
      auto move = static_cast<int>(std::min(currLine.length(),
         (nextLine.length() + currLine.substr(userText.getCursorCol()).length() - 1) + static_cast<int>(nextLine.back() != L'\n')));
      /*This kind of witchcraft bears explaining:
      some lines end with newline characters that are not displayed. However, they occupy  an index in the userText.text field.
      Since they are indexed, we have to count them when traversing lines. How do we count them? In the default case, we say a line is one character shorter than its display
      (assuming a newline is printed). However, if the last character on a line is not a newline, that is, nextLine.back() != '\n',
      we don't subtract out a newline character. Since the boolean expression returns one, if a newline character exists,
      we add 0 to the calculation and it proceeds; if no newline character exists, we add a character to our count, casting the bool to an int.*/
      userText.moveCursor(move);
   }
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
   UNREFERENCED_PARAMETER(lParam);
   switch (message)
   {
   case WM_INITDIALOG:
      return (INT_PTR)TRUE;

   case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
      {
         EndDialog(hDlg, LOWORD(wParam));
         return (INT_PTR)TRUE;
      }
      break;
   }
   return (INT_PTR)FALSE;
}

static void
CustomOnWmSize(HWND hWnd, UINT uWidth, UINT uHeight)
{
   SCROLLINFO si;

   si.cbSize = sizeof(SCROLLINFO);
   si.fMask = SIF_PAGE;

   si.nPage = uWidth;
   SetScrollInfo(hWnd, SB_HORZ, &si, FALSE);

   // FIX: Make sure uHeight has the right value:
   {
      RECT rc;
      GetClientRect(hWnd, &rc);
      uHeight = rc.bottom - rc.top;
   }

   si.nPage = uHeight;
   SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
}

/*static void
CustomHandleVScroll(HWND hwnd, int iAction)
{
   int nPos;
   int nOldPos;
   SCROLLINFO si;

   // Get current scrollbar state:
   si.cbSize = sizeof(SCROLLINFO);
   si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
   GetScrollInfo(pData->hwnd, SB_VERT, &si);

   nOldPos = si.nPos;

   // Compute new nPos.
   // Note we do not care where nPos falls between nMin and nMax. See below.
   switch (iAction) {
   case SB_TOP:            nPos = si.nMin; break;
   case SB_BOTTOM:         nPos = si.nMax; break;
   case SB_LINEUP:         nPos = si.nPos - 1; break;
   case SB_LINEDOWN:       nPos = si.nPos + 1; break;
   case SB_PAGEUP:         nPos = si.nPos - CustomLogicalPage(si.nPage); break;
   case SB_PAGEDOWN:       nPos = si.nPos + CustomLogicalPage(si.nPage); break;
   case SB_THUMBTRACK:     nPos = si.nTrackPos; break;
   default:
   case SB_THUMBPOSITION:  nPos = si.nPos; break;
   }

   // Update the scrollbar state (nPos) and repaint it. The function ensures
   // the nPos does not fall out of the allowed range between nMin and nMax
   // hence we ask for the corrected nPos again.
   SetScrollPos(hwnd, SB_VERT, nPos, TRUE);
   nPos = GetScrollPos(hwnd, SB_VERT);

   // Refresh the control (repaint it to reflect the new nPos). Note we
   // here multiply with some unspecified scrolling unit which specifies
   // amount of pixels corresponding to the 1 scrolling unit.
   // We will discuss ScrollWindowEx() more later in the article.
   ScrollWindowEx(hwnd, 0, (nOldPos - nPos) * scrollUnit
      NULL, NULL, NULL, NULL, SW_ERASE | SW_INVALIDATE);
}*/
