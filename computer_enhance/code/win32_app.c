#include "win32_app.h"

//@NOTE(Emilio): For now we will include this here
//  but when we add support for hot-reloading
//  we will need to load this manually
#include "simulator.c"
#include "decoder.c"

file_scope void
RenderTestImage(win32_display_buffer* Buffer)
{

  s8* Row = (s8*)Buffer->Memory;
  for(s32 Y = 0; Y < Buffer->Height; Y++)
  {
    s32* Pixel = (s32*)Row;
    for(s32 X = 0; X < Buffer->Width; X++)
    {
      *Pixel++ = (X << 8) | (Y << 16);
    }
    Row += Buffer->Stride;
  }
}

file_scope void
RenderBlankImage(win32_display_buffer* Buffer)
{

  s8* Row = (s8*)Buffer->Memory;
  for(s32 Y = 0; Y < Buffer->Height; Y++)
  {
    s32* Pixel = (s32*)Row;
    for(s32 X = 0; X < Buffer->Width; X++)
    {
      *Pixel++ = 0;
    }
    Row += Buffer->Stride;
  }
}



file_scope void
Win32DrawDisplayRegion(win32_display_buffer* Buffer, s32 Width, s32 Height)
{
  Buffer->Width  = Width;
  Buffer->Height = Height;
  Buffer->Stride = Buffer->Width * Buffer->BytesPerPixel;

  //@IMPORTANT(Emilio): We actually don't update the memory to account for a
  //  new width and height, since for now this is expected to only
  //  be called once.
  if(Buffer->Memory == 0)
  {
    Buffer->Memory = VirtualAlloc(0, Buffer->Width * Buffer->Height * Buffer->BytesPerPixel,
                                   MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  }

  Buffer->Info.bmiHeader.biSize         = sizeof(Buffer->Info.bmiHeader);
  Buffer->Info.bmiHeader.biWidth        = Buffer->Width;
  Buffer->Info.bmiHeader.biHeight       = Buffer->Height;
  Buffer->Info.bmiHeader.biPlanes       = 2;
  Buffer->Info.bmiHeader.biBitCount     = Buffer->BytesPerPixel * 8;
  Buffer->Info.bmiHeader.biCompression  = BI_RGB;
}

file_scope void
Win32WindowResize(HDC DeviceContext, win32_display_buffer* Buffer)
{
  StretchDIBits(DeviceContext,
                0, 0, WND_WIDTH, WND_HEIGHT,
                0, 0, Buffer->Width, Buffer->Height,
                Buffer->Memory, &Buffer->Info,
                DIB_RGB_COLORS, SRCCOPY);
}

LRESULT
Win32MainWindowCallback(HWND Window, UINT Message,
                        WPARAM WParam, LPARAM LParam)
{
  LRESULT Result = 0;

  switch(Message)
  {
    case WM_PAINT:
    {
      PAINTSTRUCT Paint = {};
      BeginPaint(Window, &Paint);
      EndPaint(Window, &Paint);
    } break;

    case WM_SIZE:
    {
      HDC DeviceContext = GetDC(Window);
      Win32WindowResize(DeviceContext, &GlobalDisplayBuffer);
      ReleaseDC(Window, DeviceContext);
    } break;

    case WM_CLOSE:
    {
      GlobalIsGameRunning = 0;
      DestroyWindow(Window);
    } break;

    case WM_DESTROY:
    {
      GlobalIsGameRunning = 0;
      PostQuitMessage(0);
    } break;

    default:
    {
      Result =
        DefWindowProcA(Window, Message, WParam, LParam);
    }
  }

  return Result;
}

s32 WINAPI
wWinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
         PWSTR CmdLine, s32 CmdShow)
{


  WNDCLASSA WindowClass = {};
  WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  WindowClass.lpfnWndProc = &Win32MainWindowCallback;
  WindowClass.hInstance = Instance;
  WindowClass.lpszClassName = "Computer Enhance";

  RegisterClassA(&WindowClass);

  HWND MainWindow =
    CreateWindowA(WindowClass.lpszClassName, WindowClass.lpszClassName,
                  WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
                  CW_USEDEFAULT, WND_WIDTH, WND_HEIGHT,
                  0, 0, Instance, 0);

  if(MainWindow)
  {
    //@NOTE(Emilio): Initialize Globals.
    GlobalIsGameRunning = 1;

    GlobalDisplayBuffer.BytesPerPixel = 4;
    Win32DrawDisplayRegion(&GlobalDisplayBuffer, WND_WIDTH, WND_HEIGHT);

    //@NOTE(Emilio): Start of Game Loop
    while(GlobalIsGameRunning)
    {
      MSG Message = {};
      while(PeekMessageA(&Message, MainWindow, 0, 0, PM_REMOVE))
      {
        switch(Message.message)
        {
          case WM_KEYUP:
          case WM_KEYDOWN:
          case WM_SYSKEYUP:
          case WM_SYSKEYDOWN:
          {
            if(Message.wParam == VK_ESCAPE)
            {
              GlobalIsGameRunning = 0;
            }
          }
          default:
          {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
          }
        }
      }

      //@NOTE(Emilio): Rendering Code.
      RenderBlankImage(&GlobalDisplayBuffer);
      HDC DeviceContext = GetDC(MainWindow);
      Win32WindowResize(DeviceContext, &GlobalDisplayBuffer);
      ReleaseDC(MainWindow, DeviceContext);
    }

  }
  else
  {
    DWORD Error = GetLastError();
    //@NOTE(Emilio): So compiler won't optimize Error out.
    int Sum = Error + 2;
    //@TODO(Emilio): Logging -> Unable to CreateWindow || RegisterClassA
  }

}
