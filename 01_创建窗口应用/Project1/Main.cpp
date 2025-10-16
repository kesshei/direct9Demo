#include <windows.h>

//定义窗口宽度
int WINDOW_WIDTH = 800;
//定义当前文本绘制的宽度位置
int CurrentWidth = 0;
// 窗口过程函数声明
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 程序入口点
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// 1. 注册窗口类
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProc;    // 窗口过程函数
	wc.hInstance = hInstance;     // 实例句柄
	wc.lpszClassName = CLASS_NAME;    // 窗口类名
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // 背景色

	RegisterClass(&wc);

	// 2. 创建窗口
	HWND hwnd = CreateWindowEx(
		0,                              // 扩展样式
		CLASS_NAME,                     // 窗口类名
		L"我的第一个窗口",               // 窗口标题
		WS_OVERLAPPEDWINDOW,            // 窗口样式（带标题栏、边框、最大化/最小化按钮）

		// 窗口位置和大小
		CW_USEDEFAULT, CW_USEDEFAULT,   // 初始x、y坐标（使用默认值）
		WINDOW_WIDTH, 600,                       // 宽度和高度

		NULL,       // 父窗口句柄
		NULL,       // 菜单句柄
		hInstance,  // 实例句柄
		NULL        // 附加数据
	);

	if (hwnd == NULL)
	{
		return 0; // 创建窗口失败
	}

	// 3. 显示窗口
	ShowWindow(hwnd, nCmdShow);

	// 4. 消息循环
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);  // 翻译键盘消息
		DispatchMessage(&msg);   // 分发消息到窗口过程
	}

	return 0;
}

// 窗口过程（处理窗口消息）
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
			// 设置1秒定时器（ID为1）
			SetTimer(hwnd, 1, 1000, NULL);
			break;
		case WM_TIMER:
			// 定时器触发（wParam 为定时器ID，这里判断是否是我们设置的ID=1）
			if (wParam == 1) {
				CurrentWidth += 10;
				if (CurrentWidth >= (WINDOW_WIDTH -10))//如果宽度超过窗口宽度，重置为0
				{
					CurrentWidth = 0;
				}
				// 每1秒触发一次重绘（标记整个客户区无效）
				InvalidateRect(hwnd, NULL, TRUE);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);  // 窗口关闭时发送退出消息
			return 0;

		case WM_PAINT:// 绘制窗口内容
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			// 绘制背景
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

			// 绘制文本
			const wchar_t text[] = L"Hello, Win32!";
			TextOut(hdc, CurrentWidth, 50, text, wcslen(text));

			EndPaint(hwnd, &ps);
			return 0;
		}
	}

	// 处理未定义的消息（使用默认处理）
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}