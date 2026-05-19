#include "win32_app.h"

file_scope void
Win32DebugPrintInstruction(decoder_context* Context, char* Result)
{
  if(Result)
  {
    sprintf(Result, "%s ;%s\n", Context->CurrentInstruction, Context->Comments);
    OutputDebugStringA(Result);
  }
  else
  {
    char DebugText[256];
    sprintf(DebugText, "%s ;%s\n", Context->CurrentInstruction, Context->Comments);
    OutputDebugStringA(DebugText);
  }
}

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
  Buffer->Info.bmiHeader.biHeight       = -Buffer->Height;
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

  s8 Inst = 0b10001011;
  s8 PosAfterInst = 6;


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
    //@NOTE(Emilio): Simulator Code
    sim_cpu SimCPU = {};

    file_result FileResult = Win32ReadEntireFile(HW_FILE);
    SimulatorFlashProgram(SimMemory, FileResult.Contents, SimCPU.PC, FileResult.ContentSize);


    //@NOTE(Emilio): Initialize Globals.
    GlobalIsGameRunning = 1;
    GlobalSleepValue = 50;

    GlobalDisplayBuffer.BytesPerPixel = 4;
    Win32DrawDisplayRegion(&GlobalDisplayBuffer, WND_WIDTH, WND_HEIGHT);

    //@NOTE(Emilio): Initialize Render Code
    render_display_buffer RenderDisplayBuffer = {};
    RenderDisplayBuffer.Width = GlobalDisplayBuffer.Width;
    RenderDisplayBuffer.Height = GlobalDisplayBuffer.Height;
    RenderDisplayBuffer.BytesPerPixel = GlobalDisplayBuffer.BytesPerPixel;
    RenderDisplayBuffer.Stride = GlobalDisplayBuffer.Stride;
    RenderDisplayBuffer.Memory = GlobalDisplayBuffer.Memory;
    RendererRenderBlankImage(&RenderDisplayBuffer);

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
            if(Message.wParam == VK_SPACE)
            {
              GlobalIsResetAsserted = 1;
            }
            if(Message.wParam == VK_LEFT)
            {
              GlobalSleepValue -= 50;
            }
            if(Message.wParam == VK_RIGHT)
            {
              GlobalSleepValue += 50;
            }
            if(Message.wParam == VK_UP)
            {
              GlobalMemoryOffset -= 50;
            }
            if(Message.wParam == VK_DOWN)
            {
              GlobalMemoryOffset += 50;
            }
          }
          default:
          {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
          }
        }
      }

      if(GlobalSleepValue < 0)
      {
        GlobalSleepValue = 0;
      }
      if(GlobalSleepValue > 500)
      {
        GlobalSleepValue = 500;
      }
      if(GlobalMemoryOffset < 0)
      {
        GlobalMemoryOffset = 0;
      }
      if(GlobalMemoryOffset > (MegaBytes(1) - 64))
      {
        GlobalMemoryOffset = (MegaBytes(1) - 64);
      }

      //@INCOMPLETE(Emilio): Sleep is not grandular and we are not
      //  picking up a refresh rate yet!
      Sleep(GlobalSleepValue);

      if(GlobalIsResetAsserted)
      {
        SimCPU.PC = 0;
        GlobalIsResetAsserted = 0;
      }

      decoder_context DecoderContext = {};
      char DebugText[256];
      if(SimCPU.PC < FileResult.ContentSize)
      {
        DecoderDecodeInstruction(&DecoderContext, SimMemory, SimCPU.PC);
        Win32DebugPrintInstruction(&DecoderContext, DebugText);
        SimCPU.PC += DecoderContext.BytesRead;
      }


      //@NOTE(Emilio): Rendering Code.
      RendererRenderBox(&RenderDisplayBuffer, 0, 0, WND_WIDTH, WND_HEIGHT, 0.66f, 0.00f, 0.66f);

//@NOTE(Emilio): Memory Window.
      RendererRenderDisplayBox(&RenderDisplayBuffer, (WND_WIDTH/2),
                    (WND_HEIGHT/32), 900, 800, 5,
                    0.17f, 0.17f, 0.17f,
                    0.1f, 0.1f, 0.1f);
      RendererRenderMemoryWindow(&RenderDisplayBuffer, (WND_WIDTH/2) + 5,
                    (WND_HEIGHT/32) + 5, 890, 790, GlobalMemoryOffset,
                    SimCPU.PC - DecoderContext.BytesRead,
                    DecoderContext.BytesRead);


//@NOTE(Emilio): Simulated Decoder.
      RendererRenderDisplayBox(&RenderDisplayBuffer, (WND_WIDTH / 24),
                    (WND_HEIGHT / 32), 800, 400, 5,
                    0.17f, 0.17f, 0.17f,
                    0.1f, 0.1f, 0.1f);

//@NOTE(Emilio): Simulated CPU.
      RendererRenderDisplayBox(&RenderDisplayBuffer, (WND_WIDTH / 24),
                    WND_HEIGHT - (WND_HEIGHT / 3) - (WND_WIDTH / 16), 800, 400, 5,
                    0.17f, 0.17f, 0.17f,
                    0.1f, 0.1f, 0.1f);


//@NOTE(Emilio): Output Window.
      RendererRenderDisplayBox(&RenderDisplayBuffer, ((WND_WIDTH / 3) * 2) + (WND_WIDTH/24),
                    WND_HEIGHT - (WND_HEIGHT / 5), 500, 100, 5,
                    0.17f, 0.17f, 0.17f,
                    0.1f, 0.1f, 0.1f);

      RendererRenderString(&RenderDisplayBuffer, ((WND_WIDTH / 3) * 2 + 10) + (WND_WIDTH/24),
                    WND_HEIGHT - (WND_HEIGHT / 5) + 55, DebugText);

//@NOTE(Emilio): Speed Window.
      RendererRenderDisplayBox(&RenderDisplayBuffer, ((WND_WIDTH / 3) * 2) + (WND_WIDTH/24),
                    WND_HEIGHT - (WND_HEIGHT / 10), 300, 50, 5,
                    0.17f, 0.17f, 0.17f,
                    0.1f, 0.1f, 0.1f);

      char SleepText[256];
      sprintf(SleepText, "The Delay Value is %d.\n", GlobalSleepValue);
      RendererRenderString(&RenderDisplayBuffer, ((WND_WIDTH / 3) * 2) + (WND_WIDTH/24),
                    WND_HEIGHT - (WND_HEIGHT / 10) + 30, SleepText);



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
