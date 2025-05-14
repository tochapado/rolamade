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

global_variable bool Running;

struct win32_offscreen_buffer
{
  BITMAPINFO  Info;
  void        *Memory;
  int         Width;
  int         Height;
  int         Pitch;
  int         BytesPerPixel;
};

global_variable win32_offscreen_buffer Global_BackBuffer;

struct win32_window_dimension
{
  int Width;
  int Height;
};

win32_window_dimension Win32GetWindowDimension(HWND Window)
{
  win32_window_dimension Result;

  RECT ClientRect;
  GetClientRect(Window, &ClientRect);
  Result.Width = ClientRect.right - ClientRect.left;
  Result.Height = ClientRect.bottom - ClientRect.top;

  return(Result);
};

internal void RenderWeirdGradient(
  win32_offscreen_buffer  *Buffer,
  int                     XOffset,
  int                     YOffset)
{
  uint8 *Row = (uint8 *)Buffer->Memory;
  for(int Y = 0; Y < Buffer->Height; Y++)
  {
    uint32 *Pixel = (uint32 *)Row;
    for(int X = 0; X < Buffer->Width; X++)
    {
      uint8 Blue = (X + XOffset);
      uint8 Green = (Y + YOffset);

      *Pixel++ = ((Green << 8) | Blue);
    };
    Row = Row + Buffer->Pitch;
  };
}

internal void Win32ResizeDIBSection(
  win32_offscreen_buffer  *Buffer,
  int                     Width,
  int                     Height)
{
  if(Buffer->Memory)
  {
    VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
  };

  Buffer->Width = Width;
  Buffer->Height = Height;

  Buffer->BytesPerPixel = 4;

  Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
  Buffer->Info.bmiHeader.biWidth = Buffer->Width;
  Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
  Buffer->Info.bmiHeader.biPlanes = 1;
  Buffer->Info.bmiHeader.biBitCount = 32;
  Buffer->Info.bmiHeader.biCompression = BI_RGB;
  Buffer->Info.bmiHeader.biSizeImage = 0;
  Buffer->Info.bmiHeader.biXPelsPerMeter = 0;
  Buffer->Info.bmiHeader.biYPelsPerMeter = 0;
  Buffer->Info.bmiHeader.biClrUsed = 0;
  Buffer->Info.bmiHeader.biClrImportant = 0;

  Buffer->Pitch = Width * Buffer->BytesPerPixel;

  int BitmapMemorySize = (Width * Height) * Buffer->BytesPerPixel;
  Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void Win32DisplayBufferToWindow(
  win32_offscreen_buffer  *Buffer,
  HDC                     DeviceContext,
  int                     WindowWidth,
  int                     WindowHeight,
  int                     X,
  int                     Y,
  int                     Width,
  int                     Height)
{
  StretchDIBits(
    DeviceContext,
    0, 0, WindowWidth, WindowHeight,
    0, 0, Buffer->Width, Buffer->Height,
    Buffer->Memory,
    &Buffer->Info,
    DIB_RGB_COLORS,
    SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(
  HWND    Window,
  UINT    Message,
  WPARAM  WParam,
  LPARAM  LParam)
{
  LRESULT Result = 0;

  switch(Message)
  {
    case WM_SIZE:
    {

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
      HDC DeviceContext = BeginPaint(Window, &Paint);

      int X = Paint.rcPaint.left;
      int Y = Paint.rcPaint.top;
      int Width = Paint.rcPaint.right - Paint.rcPaint.left;
      int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;

      win32_window_dimension Dimension = Win32GetWindowDimension(Window);

      Win32DisplayBufferToWindow(
        &Global_BackBuffer,
        DeviceContext,
        Dimension.Width, Dimension.Height,
        X, Y,
        Width, Height);

      EndPaint(Window, &Paint);
    } break;

    default:
    {
      OutputDebugStringA("DefaultrOLA\n");
      Result = DefWindowProc(Window, Message, WParam, LParam);
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

  Win32ResizeDIBSection(&Global_BackBuffer, 1280, 720);

  WindowClass.style = CS_HREDRAW | CS_VREDRAW;
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = Instance;
  //  WindowClass.hIcon;
  WindowClass.lpszClassName = "Devia usar o x11";

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
        RenderWeirdGradient(&Global_BackBuffer, XOffset, YOffset);

        HDC DeviceContext = GetDC(Window);

        win32_window_dimension Dimension = Win32GetWindowDimension(Window);
        Win32DisplayBufferToWindow(
          &Global_BackBuffer,
          DeviceContext,
          Dimension.Width, Dimension.Height, 
          0, 0,
          Dimension.Width, Dimension.Height);

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
