//=============================================================================
// Desc: 三维文本显示
//=============================================================================

#include <d3dx9.h>

//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL;    //Direct3D对象
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;    //Direct3D设备对象
LPD3DXMESH              pTextMesh    = NULL;    //三维字体网格模型


//-----------------------------------------------------------------------------
// Desc: 设置世界矩阵
//-----------------------------------------------------------------------------
VOID SetWorldMatrix()
{
	//创建并设置世界矩阵
    D3DXMATRIX matWorld;
	D3DXMATRIX matTrans;
	D3DXMatrixTranslation(&matWorld,-1.75f,0.0f,0.0f);  //平移
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}


//-----------------------------------------------------------------------------
// Desc: 设置观察矩阵和投影矩阵
//-----------------------------------------------------------------------------
VOID SetupViewandProjMatrices()
{
    //建立并设置观察矩阵
    D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-8.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    //建立并设置投影矩阵
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


//-----------------------------------------------------------------------------
// Desc: 设置材质和灯光
//-----------------------------------------------------------------------------
VOID SetLight()
{
    //设置材质
    D3DMATERIAL9 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    g_pd3dDevice->SetMaterial( &mtrl );

	//设置灯光
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

    //设置环境光
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xff505050);
}


//-----------------------------------------------------------------------------
// Desc: 初始化Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	//创建Direct3D对象, 该对象用于创建Direct3D设备对象
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

	//设置D3DPRESENT_PARAMETERS结构, 准备创建Direct3D设备对象
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//创建Direct3D设备对象
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

	//创建设备描述表
	HDC hdc = CreateCompatibleDC( NULL );
    if( hdc == NULL )
        return E_OUTOFMEMORY;
    
	//创建字体
    HFONT hFont = CreateFont(0, 0, 0, 0, FW_BOLD , false, false, false, 
		               DEFAULT_CHARSET,OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
					   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");   
    
	//将字体选进设备描述表
	SelectObject(hdc, hFont); 

	//创建三维文本网格模型
	if( FAILED( D3DXCreateText( g_pd3dDevice, hdc, L"三维字体", 0.001f, 0.4f, &pTextMesh, NULL, NULL)))
	{
        return E_FAIL;
    }

	//释放字体和设备描述表
	DeleteObject( hFont );
    DeleteDC( hdc );

	//设置剔出模式,为不剔出任何面(正面和方面)
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	//关闭光照处理, 默认情况下使用光照处理
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true );

	//设置观察和投影矩阵
	SetupViewandProjMatrices();

	//设置材质和灯光
	SetLight();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 释放创建对象
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	//释放三维字体网格模型
	if( pTextMesh != NULL)
        pTextMesh->Release();

	//释放Direct3D设备对象
    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

	//释放Direct3D对象
    if( g_pD3D != NULL )
        g_pD3D->Release();
}


//-----------------------------------------------------------------------------
// Desc: 渲染图形 
//-----------------------------------------------------------------------------
VOID Render()
{
	//清空后台缓冲区
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

	//开始在后台缓冲区绘制图形
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
		//设置世界矩阵
        SetWorldMatrix();

		//在后台缓冲区绘制图形
		pTextMesh->DrawSubset(0);

		//结束在后台缓冲区绘制图形
        g_pd3dDevice->EndScene();
    }

	//将在后台缓冲区绘制的图形提交到前台缓冲区显示
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//-----------------------------------------------------------------------------
// Desc: 消息处理
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
// Desc: 入口函数
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	//注册窗口类
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC , MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"ClassName", NULL };
    RegisterClassEx( &wc );

	//创建窗口
    HWND hWnd = CreateWindow( L"ClassName", L"三维字体",
                              WS_OVERLAPPEDWINDOW, 200, 100, 600,500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//初始化Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{   
		//显示窗口
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

		//进入消息循环
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
				Render();   //渲染图形
			}
		}     
	}

    UnregisterClass( L"ClassName", wc.hInstance );
    return 0;
}



