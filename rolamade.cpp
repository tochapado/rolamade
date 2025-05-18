/*
  File: rolamade.cpp
  Date: 04/27/2025
  Revision: 0.01
  Creator: tochapado
  Notice: (C) Copyrigt 2025 by tochapado, inc. All Rights Reserved.
*/

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <xinput.h>
#include <dsound.h>

#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t   uint8;
typedef uint16_t  uint16;
typedef uint32_t  uint32;
typedef uint64_t  uint64;

typedef int8_t    int8;
typedef int16_t   int16;
typedef int32_t   int32;
typedef int64_t   int64;

typedef int32     bool32;

global_variable bool GlobalRunning;

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

// // Loculrada BELOW!! OLDER C THAN C ITSELF

// #define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState);
// typedef X_INPUT_GET_STATE(x_input_get_state);
// X_INPUT_GET_STATE(XInputGetStateStub)
// {
//   return(ERROR_DEVICE_NOT_CONNECTED);
// };
// global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
// #define XInputGetState XInputGetState_;

// #define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration);
// typedef X_INPUT_SET_STATE(x_input_set_state);
// X_INPUT_SET_STATE(XInputSetStateStub)
// {
//   return(ERROR_DEVICE_NOT_CONNECTED);
// };
// global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
// #define XInputSetState XInputSetState_;

// internal void Win32LoadXInput(void)
// {
//   HMODULE XInputLibrary = LoadLibrary("xinput1_4.dll");
//   if(!XInputLibrary)
//   {
//      XInputLibrary = LoadLibrary("xinput1_3.dll");
//   };
//
//   if(XInputLibrary)
//   {
//      XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
//      XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
//      
//   }
//   else
//   {
//     // Diagnostic
//   }
// };

internal void Win32InitDSound(HWND Window)
{
  // Load de DirectSound Library
  HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
  if(DSoundLibrary)
  {
    // Get a DirectSound object
    LPDIRECTSOUND DirectSound;
    if(SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
    {
      // Create a primary buffer
    
      // Create a secondary buffer
    
      // Start it playing

    }
    else
    {
      // Diagnostics
    };

  }else
  {
    // Diagnostics
  };
};

internal win32_window_dimension Win32GetWindowDimension(HWND Window)
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
  int                     YOffset,
  int                     num)
{
  uint8 *Row = (uint8 *)Buffer->Memory;
  for(int Y = 0; Y < Buffer->Height; Y++)
  {
    uint32 *Pixel = (uint32 *)Row;
    for(int X = 0; X < Buffer->Width; X++)
    {
      uint8 Blue = (X + XOffset);
      uint8 Green = (Y + YOffset);

      *Pixel++ = ((Green << num) | Blue);
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
      GlobalRunning = false;
    } break;

    case WM_CLOSE:
    {
      GlobalRunning = false;
    } break;

    case WM_ACTIVATEAPP:
    {
    } break;

    case WM_SYSKEYDOWN:

    case WM_SYSKEYUP:

    case WM_KEYDOWN:

    case WM_KEYUP:
    {
      uint32 VKCode = WParam;

      bool WasDown = ((LParam & (1 << 30)) != 0);
      bool IsDown = ((LParam & (1 << 31)) == 0);

      if(VKCode == 'W')
      {
      }
      else if(VKCode == 'A')
      {

      }
      else if(VKCode == 'S')
      {

      }
      else if(VKCode == 'D')
      {

      }
      else if(VKCode == VK_ESCAPE)
      {

      }
      else if(VKCode == VK_SPACE)
      {

      };

      // Handles Alt+F4
      bool32 AltKeyWasDown = (LParam & (1 << 29));
      printf("%d", AltKeyWasDown);
      if((VKCode == VK_F4) && AltKeyWasDown)
      {
        GlobalRunning = false;
      };

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
  WNDCLASSA *WindowClass = {};

  Win32ResizeDIBSection(&Global_BackBuffer, 1280, 720);

  WindowClass->style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  WindowClass->lpfnWndProc = Win32MainWindowCallback;
  WindowClass->hInstance = Instance;
  //  WindowClass.hIcon;
  WindowClass->lpszClassName = "Devia usar o x11";

  if(RegisterClassA(WindowClass))
  {
    HWND Window = CreateWindowA(
      WindowClass->lpszClassName,
      "Rolamade Hero",
      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
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
      HDC DeviceContext = GetDC(Window);
      
      int XOffset = 0;
      int YOffset = 0;
      int num = 0;

      // Win32LoadXInput();
      Win32InitDSound(Window);

      GlobalRunning = true;
      while(GlobalRunning)
      {
      MSG Message;

      while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
          if(Message.message == WM_QUIT)
          {
            GlobalRunning = false;
          };
          TranslateMessage(&Message);
          DispatchMessageA(&Message);
        };

        for(DWORD ControllerIndex = 0;
            ControllerIndex < XUSER_MAX_COUNT;
            ++ControllerIndex)
        {
          XINPUT_STATE ControllerState;
          if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
          {
            // Plugged in
            XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

            bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
            bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
            bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
            bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
            bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
            bool Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
            bool LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
            bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
            bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
            bool BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
            bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
            bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);

            int16 StickX = (Pad->sThumbLX);
            int16 StickY = (Pad->sThumbLY);

            if(Up){++YOffset;};
            if(Down){--YOffset;};
            if(Left){++XOffset;};
            if(Right){--XOffset;};

            if(AButton){num = 0;};
            if(BButton){num = 8;};
            if(XButton){num = 16;};
            if(YButton){num = 4;};

          }
          else
          {
            // Not Plugged in?
          }
        };

        RenderWeirdGradient(&Global_BackBuffer, XOffset, YOffset, num);

        win32_window_dimension Dimension = Win32GetWindowDimension(Window);
        Win32DisplayBufferToWindow(
          &Global_BackBuffer,
          DeviceContext,
          Dimension.Width, Dimension.Height, 
          0, 0,
          Dimension.Width, Dimension.Height);

        ReleaseDC(Window, DeviceContext);
      };
    };
  }
  else
  {

  };

  return(0);
}
