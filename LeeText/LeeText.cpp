// LeeText.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "LeeText.h"
#include "TextContainer.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
TextContainer userText;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool CALLBACK       Save(HWND, UINT, WPARAM, LPARAM);

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

   return (int)msg.wParam;
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
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
   static bool linewrap = false;
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
      case IDM_SAVE:
         Save(hWnd, NULL, NULL, NULL);
         break;
      case IDM_TOGGLEWRAP:
         linewrap = !linewrap;
         RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
         break;
      default:
         return DefWindowProc(hWnd, message, wParam, lParam);
      }
   }
   break;
   case WM_PAINT:
   {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);
      // Retrieve a handle to the fixed stock font.  
      HFONT hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
      SelectObject(hdc, hFont);
      constexpr int CHAR_HEIGHT = 15;
      constexpr int CHAR_WIDTH = 8;
      int linenum = 0;
      int cursorX = 0;
      int cursorY = 0;
      if (linewrap)
      {
         RECT rect;
         GetWindowRect(hWnd, &rect);
         int width = rect.right - rect.left;
         int height = rect.bottom - rect.top;
         int widthInChars = width / 8 - 2;
         auto textToPrint = userText.getLines(widthInChars);
         for (auto& line : textToPrint)
         {
            TextOut(hdc, 0, linenum * CHAR_HEIGHT, line.c_str(), line.length());
            linenum++;
         }
      }
      else
      {
         auto textToPrint = userText.getLines();
         for (auto& line : textToPrint)
         {
            TextOut(hdc, 0, linenum * CHAR_HEIGHT, line.c_str(), line.length());
            linenum++;
         }
      }
      SetCaretPos(userText.getCursorCol() * CHAR_WIDTH, userText.getCursorRow() * CHAR_HEIGHT);
      
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
         userText.moveCursor(TextContainer::CURSORDIRECTION::RIGHT);
         redraw = true;
         break;
      case VK_LEFT:
         userText.moveCursor(TextContainer::CURSORDIRECTION::LEFT);
         redraw = true;
         break;
      case VK_UP:
         userText.moveCursor(TextContainer::CURSORDIRECTION::UP);
         redraw = true;
         break;
      case VK_DOWN:
         userText.moveCursor(TextContainer::CURSORDIRECTION::DOWN);
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
      CreateCaret(hWnd, (HBITMAP) NULL, 2, 15);
      SetCaretPos(0, 0);
      ShowCaret(hWnd);
      break;
   case WM_DESTROY:
      PostQuitMessage(0);
      break;
   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

bool CALLBACK Save(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   // TODO: this
   return 0;
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
