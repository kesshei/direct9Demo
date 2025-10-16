#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

// ���������ͱ���
const wchar_t CLASS_NAME[] = L"DirectX9 Window Class";
const wchar_t WINDOW_TITLE[] = L"DirectX9 ��ʾ��";

// ȫ�ֱ���
HWND hWnd;                  // ���ھ��
LPDIRECT3D9 d3d;            // Direct3D ����
LPDIRECT3DDEVICE9 d3dDevice;// Direct3D �豸

// �����ζ���ṹ��λ�� + ��ɫ��
struct Vertex {
    float x, y, z;          // λ��
    DWORD color;            // ��ɫ��ARGB��
};
#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE) // �����ʽ

// ���ڹ��̣���Ϣ����
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
                 return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// ��ʼ������
bool InitWindow(HINSTANCE hInstance, int nCmdShow) {
    // ע�ᴰ����
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClass(&wc)) return false;

    // ��������
    hWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    if (hWnd == NULL) return false;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return true;
}

// ��ʼ�� Direct3D
bool InitD3D() {
    // ���� Direct3D ����
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return false;

    // ��ȡ���ڿͻ�����С
    RECT rc;
    GetClientRect(hWnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    // �豸��������
    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = TRUE;                  // ����ģʽ
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // ������ģʽ
    d3dpp.hDeviceWindow = hWnd;             // ��Ⱦ����
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8; // �󻺳�����ʽ
    d3dpp.BackBufferCount = 1;              // �󻺳�������
    d3dpp.BackBufferWidth = width;          // ���
    d3dpp.BackBufferHeight = height;        // �߶�
    d3dpp.EnableAutoDepthStencil = TRUE;    // ������Ȳ���
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16; // ��Ȼ�������ʽ

    // ���� Direct3D �豸
    HRESULT hr = d3d->CreateDevice(
        D3DADAPTER_DEFAULT,                // Ĭ���Կ�
        D3DDEVTYPE_HAL,                    // Ӳ������
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, // Ӳ�����㴦��
        &d3dpp,
        &d3dDevice
    );
    if (FAILED(hr)) return false;

    // ������Ⱦ״̬���رչ��գ�ʹ�ö�����ɫ��
    d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    return true;
}

// ��Ⱦ����
void Render() {
    if (!d3dDevice) return;

    // ��������ɫ������
    d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    // ��ʼ��Ⱦ
    if (SUCCEEDED(d3dDevice->BeginScene())) {
        // �����ζ������ݣ�λ�ú���ɫ��
        Vertex vertices[] = {
            { 0.0f,  0.5f, 0.0f, D3DCOLOR_XRGB(255, 0, 0) },  // �϶��㣨��ɫ��
            { 0.5f, -0.5f, 0.0f, D3DCOLOR_XRGB(0, 255, 0) },  // ���£���ɫ��
            { -0.5f, -0.5f, 0.0f, D3DCOLOR_XRGB(0, 0, 255) }  // ���£���ɫ��
        };

        // ���ö����ʽ������
        d3dDevice->SetFVF(D3DFVF_VERTEX);
        d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vertices, sizeof(Vertex));

        // ������Ⱦ
        d3dDevice->EndScene();
    }

    // ��������������ʾ��Ⱦ�����
    d3dDevice->Present(NULL, NULL, NULL, NULL);
}

// �ͷ���Դ
void Cleanup() {
    if (d3dDevice) d3dDevice->Release();
    if (d3d) d3d->Release();
}

// �������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // ��ʼ������
    if (!InitWindow(hInstance, nCmdShow)) return 0;

    // ��ʼ�� Direct3D
    if (!InitD3D()) {
        MessageBox(hWnd, L"Direct3D ��ʼ��ʧ��", L"����", MB_OK);
        return 0;
    }

    // ��Ϣѭ��
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // ��Ⱦ
            Render();
        }
    }

    // �ͷ���Դ
    Cleanup();
    return 0;
}