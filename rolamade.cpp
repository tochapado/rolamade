/*
  File: rolamade.cpp
  Date: 04/27/2025
  Revision: 0.01
  Creator: tochapado
  Notice: (C) Copyrigt 2025 by tochapado, inc. All Rights Reserved.
*/

#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

global_variable bool        Running;

global_variable BITMAPINFO  BitmapInfo;
global_variable void        *BitmapMemory;
global_variable int         BitmapWidth;
global_variable int         BitmapHeight;
global_variable int         BytesPerPixel = 4;

internal void RenderWeirdGradient(int XOffset, int YOffset)
{
  int Pitch = BitmapWidth * BytesPerPixel;
  uint8 *Row = (uint8 *)BitmapMemory;
  for(int Y = 0; Y < BitmapHeight; Y++)
  {
    uint32 *Pixel = (uint32 *)Row;
    for(int X = 0; X < BitmapWidth; X++)
    {
      uint8 Blue = (X + XOffset);
      uint8 Green = (Y + YOffset);

      *Pixel++ = ((Green << 8) | Blue);
    };
    Row = Row + Pitch;
  };
}

internal void Win32ResizeDIBSection(int Width, int Height)
{
  if(BitmapMemory)
  {
    VirtualFree(BitmapMemory, 0, MEM_RELEASE);
  };

  BitmapWidth = Width;
  BitmapHeight = Height;

  BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
  BitmapInfo.bmiHeader.biWidth = BitmapWidth;
  BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
  BitmapInfo.bmiHeader.biPlanes = 1;
  BitmapInfo.bmiHeader.biBitCount = 32;
  BitmapInfo.bmiHeader.biCompression = BI_RGB;
  BitmapInfo.bmiHeader.biSizeImage = 0;
  BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
  BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
  BitmapInfo.bmiHeader.biClrUsed = 0;
  BitmapInfo.bmiHeader.biClrImportant = 0;

  int BitmapMemorySize = (BitmapWidth * BitmapHeight) * BytesPerPixel;
  BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void Win32UpdateWindow(
  HDC   DeviceContext,
  RECT  *ClientRect,
  int   X,
  int   Y,
  int   Width,
  int   Height)
{
  int WindowWidth = ClientRect->right - ClientRect->left;
  int WindowHeight = ClientRect->bottom - ClientRect->top;

  StretchDIBits(
    DeviceContext,
    0, 0, BitmapWidth, BitmapHeight,
    0, 0, WindowWidth, WindowHeight,
    BitmapMemory,
    &BitmapInfo,
    DIB_RGB_COLORS,
    SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(
  HWND    WindowHandle,
  UINT    Message,
  WPARAM  WParam,
  LPARAM  LParam)
{
  LRESULT Result = 0;

  switch(Message)
  {
    case WM_SIZE:
    {
      RECT ClientRect;
      GetClientRect(WindowHandle, &ClientRect);
      int Width = ClientRect.right - ClientRect.left;
      int Height = ClientRect.bottom - ClientRect.top;
      Win32ResizeDIBSection(Width, Height);
    } break;

    case WM_DESTROY:
    {
      Running = false;
    } break;

    case WM_CLOSE:
    {
      Running = false;
    } break;

    case WM_ACTIVATEAPP:
    {
      OutputDebugStringA("WM_ACTIVATEAPP\n");
    } break;

    case WM_PAINT:
    {
      PAINTSTRUCT Paint;
      HDC DeviceContext = BeginPaint(WindowHandle, &Paint);

      int X = Paint.rcPaint.left;
      int Y = Paint.rcPaint.top;
      int Width = Paint.rcPaint.right - Paint.rcPaint.left;
      int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

      RECT ClientRect;
      GetClientRect(WindowHandle, &ClientRect);

      Win32UpdateWindow(
        DeviceContext,
        &ClientRect,
        X, Y,
        Width, Height);

      EndPaint(WindowHandle, &Paint);
    } break;

    default:
    {
      OutputDebugStringA("DefaultrOLA\n");
      Result = DefWindowProc(WindowHandle, Message, WParam, LParam);
    } break;
  };

  return(Result);
}

int CALLBACK WinMain(
  HINSTANCE Instance,
  HINSTANCE PrevInstance,
  LPSTR     CommandLine,
  int       ShowCode)
{
  WNDCLASS WindowClass = {};

  /*
  typedef struct tagWNDCLASSA {
    UINT      style;
    WNDPROC   lpfnWndProc;
    int       cbClsExtra;
    int       cbWndExtra;
    HINSTANCE hInstance;
    HICON     hIcon;
    HCURSOR   hCursor;
    HBRUSH    hbrBackground;
    LPCSTR    lpszMenuName;
    LPCSTR    lpszClassName;
  } WNDCLASSA, *PWNDCLASSA, *NPWNDCLASSA, *LPWNDCLASSA;
  */

  WindowClass.style = CS_OWNDC;
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = Instance;
//  WindowClass.hIcon;
  WindowClass.lpszClassName = "RolamadeWindowClass";

  if(RegisterClass(&WindowClass))
  {
    HWND Window = CreateWindowEx(
      0,
      WindowClass.lpszClassName,
      "Rolamade Hero",
      WS_OVERLAPPEDWINDOW|WS_VISIBLE,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      0,
      0,
      Instance,
      0
    );

    if(Window)
    {
      Running = true;
      MSG Message;
      int XOffset = 0;
      int YOffset = 0;
      while(Running)
      {
        while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
          if(Message.message == WM_QUIT)
          {
            Running = false;
          };
          TranslateMessage(&Message);
          DispatchMessageA(&Message);
        };
        RenderWeirdGradient(XOffset, YOffset);

        HDC DeviceContext = GetDC(Window);

        RECT ClientRect;
        GetClientRect(Window, &ClientRect);
        int WindowWidth = ClientRect.right - ClientRect.left;
        int WindowHeight = ClientRect.bottom - ClientRect.top;
        Win32UpdateWindow(
          DeviceContext,
          &ClientRect,
          0, 0,
          WindowWidth, WindowHeight);

        ReleaseDC(Window, DeviceContext);

        ++XOffset;
        ++YOffset;
      };
    };
  }
  else
  {

  };

  return(0);
}
