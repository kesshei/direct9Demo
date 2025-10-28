//=============================================================================
// Desc: ��ά�ı���ʾ
//=============================================================================

#include <d3dx9.h>

//-----------------------------------------------------------------------------
// Desc: ȫ�ֱ���
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL;    //Direct3D����
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;    //Direct3D�豸����
LPD3DXMESH              pTextMesh    = NULL;    //��ά��������ģ��


//-----------------------------------------------------------------------------
// Desc: �����������
//-----------------------------------------------------------------------------
VOID SetWorldMatrix()
{
	//�����������������
    D3DXMATRIX matWorld;
	D3DXMATRIX matTrans;
	D3DXMatrixTranslation(&matWorld,-1.75f,0.0f,0.0f);  //ƽ��
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}


//-----------------------------------------------------------------------------
// Desc: ���ù۲�����ͶӰ����
//-----------------------------------------------------------------------------
VOID SetupViewandProjMatrices()
{
    //���������ù۲����
    D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-8.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    //����������ͶӰ����
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


//-----------------------------------------------------------------------------
// Desc: ���ò��ʺ͵ƹ�
//-----------------------------------------------------------------------------
VOID SetLight()
{
    //���ò���
    D3DMATERIAL9 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    g_pd3dDevice->SetMaterial( &mtrl );

	//���õƹ�
    D3DXVECTOR3 vecDir;
    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
    vecDir = D3DXVECTOR3(1, 1, 1);
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
    light.Range       = 1000.0f;
    g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );

    //���û�����
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xff505050);
}


//-----------------------------------------------------------------------------
// Desc: ��ʼ��Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	//����Direct3D����, �ö������ڴ���Direct3D�豸����
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

	//����D3DPRESENT_PARAMETERS�ṹ, ׼������Direct3D�豸����
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//����Direct3D�豸����
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

	//�����豸������
	HDC hdc = CreateCompatibleDC( NULL );
    if( hdc == NULL )
        return E_OUTOFMEMORY;
    
	//��������
    HFONT hFont = CreateFont(0, 0, 0, 0, FW_BOLD , false, false, false, 
		               DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
					   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");   
    
	//������ѡ���豸������
	SelectObject(hdc, hFont); 

	//������ά�ı�����ģ��
	if( FAILED( D3DXCreateText( g_pd3dDevice, hdc, L"��ά����", 0.001f, 0.4f, &pTextMesh, NULL, NULL)))
	{
        return E_FAIL;
    }

	//�ͷ�������豸������
	DeleteObject( hFont );
    DeleteDC( hdc );

	//�����޳�ģʽ,Ϊ���޳��κ���(����ͷ���)
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	//�رչ��մ���, Ĭ�������ʹ�ù��մ���
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );

	//���ù۲��ͶӰ����
	SetupViewandProjMatrices();

	//���ò��ʺ͵ƹ�
	SetLight();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: �ͷŴ�������
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	//�ͷ���ά��������ģ��
	if( pTextMesh != NULL)
        pTextMesh->Release();

	//�ͷ�Direct3D�豸����
    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

	//�ͷ�Direct3D����
    if( g_pD3D != NULL )
        g_pD3D->Release();
}


//-----------------------------------------------------------------------------
// Desc: ��Ⱦͼ�� 
//-----------------------------------------------------------------------------
VOID Render()
{
	//��պ�̨������
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

	//��ʼ�ں�̨����������ͼ��
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
		//�����������
        SetWorldMatrix();

		//�ں�̨����������ͼ��
		pTextMesh->DrawSubset(0);

		//�����ں�̨����������ͼ��
        g_pd3dDevice->EndScene();
    }

	//���ں�̨���������Ƶ�ͼ���ύ��ǰ̨��������ʾ
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//-----------------------------------------------------------------------------
// Desc: ��Ϣ����
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
			Cleanup();
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Desc: ��ں���
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	//ע�ᴰ����
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC , MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"ClassName", NULL };
    RegisterClassEx( &wc );

	//��������
    HWND hWnd = CreateWindow( L"ClassName", L"��ά����",
                              WS_OVERLAPPEDWINDOW, 200, 100, 600,500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//��ʼ��Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{   
		//��ʾ����
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

		//������Ϣѭ��
		MSG msg;
		ZeroMemory( &msg, sizeof(msg) );
		while( msg.message!=WM_QUIT )
		{
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				Render();   //��Ⱦͼ��
			}
		}     
	}

    UnregisterClass( L"ClassName", wc.hInstance );
    return 0;
}



