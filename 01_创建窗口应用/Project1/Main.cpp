#include <windows.h>

//���崰�ڿ��
int WINDOW_WIDTH = 800;
//���嵱ǰ�ı����ƵĿ��λ��
int CurrentWidth = 0;
// ���ڹ��̺�������
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ������ڵ�
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// 1. ע�ᴰ����
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProc;    // ���ڹ��̺���
	wc.hInstance = hInstance;     // ʵ�����
	wc.lpszClassName = CLASS_NAME;    // ��������
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // ����ɫ

	RegisterClass(&wc);

	// 2. ��������
	HWND hwnd = CreateWindowEx(
		0,                              // ��չ��ʽ
		CLASS_NAME,                     // ��������
		L"�ҵĵ�һ������",               // ���ڱ���
		WS_OVERLAPPEDWINDOW,            // ������ʽ�������������߿����/��С����ť��

		// ����λ�úʹ�С
		CW_USEDEFAULT, CW_USEDEFAULT,   // ��ʼx��y���꣨ʹ��Ĭ��ֵ��
		WINDOW_WIDTH, 600,                       // ��Ⱥ͸߶�

		NULL,       // �����ھ��
		NULL,       // �˵����
		hInstance,  // ʵ�����
		NULL        // ��������
	);

	if (hwnd == NULL)
	{
		return 0; // ��������ʧ��
	}

	// 3. ��ʾ����
	ShowWindow(hwnd, nCmdShow);

	// 4. ��Ϣѭ��
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);  // ���������Ϣ
		DispatchMessage(&msg);   // �ַ���Ϣ�����ڹ���
	}

	return 0;
}

// ���ڹ��̣���������Ϣ��
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
			// ����1�붨ʱ����IDΪ1��
			SetTimer(hwnd, 1, 1000, NULL);
			break;
		case WM_TIMER:
			// ��ʱ��������wParam Ϊ��ʱ��ID�������ж��Ƿ����������õ�ID=1��
			if (wParam == 1) {
				CurrentWidth += 10;
				if (CurrentWidth >= (WINDOW_WIDTH -10))//�����ȳ������ڿ�ȣ�����Ϊ0
				{
					CurrentWidth = 0;
				}
				// ÿ1�봥��һ���ػ棨��������ͻ�����Ч��
				InvalidateRect(hwnd, NULL, TRUE);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);  // ���ڹر�ʱ�����˳���Ϣ
			return 0;

		case WM_PAINT:// ���ƴ�������
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			// ���Ʊ���
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

			// �����ı�
			const wchar_t text[] = L"Hello, Win32!";
			TextOut(hdc, CurrentWidth, 50, text, wcslen(text));

			EndPaint(hwnd, &ps);
			return 0;
		}
	}

	// ����δ�������Ϣ��ʹ��Ĭ�ϴ���
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}