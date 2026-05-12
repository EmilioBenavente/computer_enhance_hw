#include "win32_app.h"

file_scope file_result
Win32ReadEntireFile(char* Filename)
{
  file_result Result = {};

  HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ,
                                  FILE_SHARE_READ, 0, OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL, 0);
  if(FileHandle != INVALID_HANDLE_VALUE)
  {
    LARGE_INTEGER FileSize;
    if(GetFileSizeEx(FileHandle, &FileSize))
    {
      //@NOTE(Emilio): The 8086 Chip can only store 1 MB of data.
      ECB_ASSERT(FileSize.LowPart < MegaBytes(1));

      Result.Contents = VirtualAlloc(0, FileSize.LowPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

      DWORD BytesRead;
      if(ReadFile(FileHandle, Result.Contents, FileSize.LowPart, &BytesRead, 0))
      {
        //@NOTE(Emilio): At this point we should have read EXACTLY FileSize.
        ECB_ASSERT((DWORD)FileSize.LowPart == BytesRead);

        Result.ContentSize = BytesRead;
      }
    }

    CloseHandle(FileHandle);
  }

  return Result;
}

file_scope b32
Win32WriteEntireFile(char* Filename, file_result* Contents)
{
  b32 Result = 0;

  HANDLE FileHandle = CreateFileA(Filename, GENERIC_WRITE,
                                  FILE_SHARE_WRITE, 0, CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL, 0);

  if(FileHandle != INVALID_HANDLE_VALUE)
  {
    DWORD BytesWritten;
    if(WriteFile(&FileHandle, &Contents->Contents, Contents->ContentSize, &BytesWritten, 0))
    {
      //@NOTE(Emilio): At this point we should have written EXACTLY FileSize.
      ECB_ASSERT((DWORD)Contents->ContentSize == BytesWritten);

      Result = 1;
    }

    CloseHandle(&FileHandle);
  }

  return Result;
}

file_scope void
Win32FreeFile(file_result* Contents)
{
  if(Contents->Contents)
  {
    VirtualFree(Contents->Contents, 0, MEM_RELEASE);
  }
}

//@NOTE(Emilio): For now we will include this here
//  but when we add support for hot-reloading
//  we will need to load this manually
#include "simulator.c"
#include "decoder.c"


file_scope void
Win32RenderTestImage(win32_display_buffer* Buffer)
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
Win32RenderBlankImage(win32_display_buffer* Buffer)
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
    ///////////////////////////////
    //@NOTE(Emilio): Simulator Code
    ///////////////////////////////
    sim_cpu SimCPU = {};

    file_result FileResult = Win32ReadEntireFile(HW_FILE);
    SimulatorFlashProgram(SimMemory, FileResult.Contents, SimCPU.PC, FileResult.ContentSize);

    decoder_context DecoderContext = {};
    DecoderDecodeInstruction(&DecoderContext, SimMemory, SimCPU.PC);

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
      Win32RenderBlankImage(&GlobalDisplayBuffer);
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
