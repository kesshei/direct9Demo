//=================================================================
// Desc: Alpha混合测试
//=================================================================
#include <d3d9.h>
#include <d3dx9.h>

LPDIRECT3D9             g_pD3D       = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL;
LPDIRECT3DTEXTURE9      g_pTexture   = NULL;   //纹理对象指针

//顶点结构
struct CUSTOMVERTEX
{   FLOAT x, y, z; 
	DWORD color;       		//顶点漫反射颜色值
	FLOAT u,v ;				//顶点纹理坐标
};
#define D3DFVF_CUSTOMVERTEX   (D3DFVF_XYZ| D3DFVF_DIFFUSE| D3DFVF_TEX1)  //顶点格式


//----------------------------------------------
// Desc: 初始化
//----------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

	//获取当前显示其显示设置
    D3DDISPLAYMODE d3ddm;
    if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
        return E_FAIL;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;

	//创建图形渲染设备对象
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );  //禁用照明效果，默认情况下起用启用照明

	//计划Alpha混合, 设置ALPHA混合系数
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   true );
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	
	//设置纹理渲染状态
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

	//纹理Alpha操作设置
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
 

    return S_OK;
}


//-------------------------------------------------------------
// Desc: 创建图形
//-------------------------------------------------------------
HRESULT InitVB()
{
	//创建纹理
    if( FAILED( D3DXCreateTextureFromFile(g_pd3dDevice, L"tree.tga", &g_pTexture)) )
    {
		return E_FAIL;
    }

    CUSTOMVERTEX g_Vertices[] =
    {
		{ -3,   -3,  0.0f,   0xffffffff,  0.0f, 1.0f},
	    { -3,    3,  0.0f,   0xffffffff,  0.0f, 0.0f},	
	    {  3,   -3,  0.0f,   0xffffffff,  1.0f, 1.0f},	
	    {  3,    3,  0.0f,   0xffffffff,  1.0f, 0.0f}

    };
	
	//创建顶点缓冲区
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
                                                 0, D3DFVF_CUSTOMVERTEX,
                                                 D3DPOOL_DEFAULT, &g_pVB,NULL ) ) )
    {
        return E_FAIL;
    }

    //填充顶点缓冲区
    VOID* pVertices;
    if( FAILED( g_pVB->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
    g_pVB->Unlock();

    return S_OK;
}

//-------------------------------------------------
// Desc: 最后清除创建的对象
//-------------------------------------------------
VOID Cleanup()
{
	if( g_pTexture != NULL )        //释放创建的纹理对象
        g_pTexture->Release();

    if( g_pVB != NULL )        
        g_pVB->Release();

    if( g_pd3dDevice != NULL ) 
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )       
        g_pD3D->Release();
}

//-----------------------------------------------------
// Desc: 设置矩阵
//-----------------------------------------------------
VOID SetupMatrices()
{
    //世界矩阵
    D3DXMATRIXA16 matWorld;
    D3DXMatrixIdentity( &matWorld );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	//观察矩阵, 观察点根据输入逐渐拉近或变远
    D3DXVECTOR3 vEyePt( 0.0f, 0.0f, -10 );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//投影矩阵
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


//----------------------------------------------------------------
// Desc: 渲染场景
//-----------------------------------------------------------------
VOID Render()
{
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
	SetupMatrices();  //设置变换矩阵,

    //开始渲染场景
    g_pd3dDevice->BeginScene();
   
	g_pd3dDevice->SetTexture( 0, g_pTexture );  //设置纹理
	g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);

    //场景渲染结束
    g_pd3dDevice->EndScene();

    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//----------------------------------------------------
// Desc: 窗口过程, 处理窗口消息
//----------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
	case WM_KEYDOWN:
		switch(wParam)
		{
		case 48:  //"0"键, 禁用Alpha混合
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
			break;

		case 49: //"1"键, 激活Alpha混合
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			break;
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}



//-----------------------------------------------------
// Desc: 程序入口函数
//-----------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{

    //注册窗口类
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"ClassName", NULL };
    RegisterClassEx( &wc );

    //创建窗口
    HWND hWnd = CreateWindow( L"ClassName", L"纹理Alpha",
                              WS_OVERLAPPEDWINDOW, 200, 100, 500, 500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

    if( SUCCEEDED( InitD3D( hWnd ) ) )  //初始化Direct3D渲染设备
    {
        if( SUCCEEDED( InitVB() ) )     //创建图形
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
                    Render();  //渲染图形
            }
        }
    }

    //程序退出前的清除工作
    Cleanup();
    UnregisterClass( L"ClassName", wc.hInstance );
    return 0;
}
