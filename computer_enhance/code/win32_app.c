#include "win32_app.h"

LRESULT
Win32MainWindowCallback(HWND Window, UINT Message,
                        WPARAM WParam, LPARAM LParam)
{
  LRESULT Result = 0;

  switch(Message)
  {
    //@TODO(Emilio): Windows messaging into switch cases.
    case WM_PAINT:
    {
      PAINTSTRUCT Paint = {};
      BeginPaint(Window, &Paint);


      EndPaint(Window, &Paint);
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

    GlobalIsGameRunning = 1;
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

      OutputDebugString("We Are Here!\n");
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
