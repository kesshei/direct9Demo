//=============================================================================
//Desc: 深度测试
//=============================================================================

#include <d3dx9.h>


//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D           = NULL; //Direct3D对象
LPDIRECT3DDEVICE9       g_pd3dDevice     = NULL; //Direct3D设备对象
LPD3DXMESH              g_pMesh          = NULL; //网格模型对象
D3DMATERIAL9*           g_pMeshMaterials = NULL; //网格模型材质
LPDIRECT3DTEXTURE9*     g_pMeshTextures  = NULL; //网格模型纹理
DWORD                   g_dwNumMaterials = 0L;   //网格模型子块数量
LPDIRECT3DVERTEXBUFFER9 g_pShutter       = NULL; //遮挡板顶点缓冲区
D3DXMATRIXA16           g_matTank;
D3DXMATRIXA16           g_matShutter;


//-----------------------------------------------------------------------------
// Desc: 顶点结构
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{   FLOAT x, y, z;    //顶点位置
	DWORD color;      //顶点漫反射颜色值
};
#define D3DFVF_CUSTOMVERTEX   (D3DFVF_XYZ| D3DFVF_DIFFUSE)


//-----------------------------------------------------------------------------
// Desc: 设置观察矩阵和投影矩阵
//-----------------------------------------------------------------------------
VOID SetViewAndProjMatrices()
{
    //创建并设置世界矩阵
    D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixRotationY( &matWorld, D3DX_PI/2);
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	//创建并设置观察矩阵
	D3DXVECTOR3 vEyePt( 0.0f, 2.0f,-30.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 2.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView; 
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//创建并设置投影矩阵
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 5000.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
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
    d3dpp.EnableAutoDepthStencil = TRUE;            //使用深度测试
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;      //深度缓冲区格式

    //创建Direct3D设备对象
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    //启用深度测试,　设置深度测试
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, true );       //默认激活
	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESS );  //设置比较函数，默认为D3DCMP_LESSEQUAL
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);    //默认值为TRUE

    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );  //设置环境光

	//设置观察矩阵和投影矩阵
	SetViewAndProjMatrices();

	//初始化坦克世界矩阵
	D3DXMatrixIdentity(&g_matTank);
	D3DXMatrixRotationY( &g_matTank, D3DX_PI/2);

	//初始化遮挡物世界矩阵
	D3DXMatrixIdentity(&g_matShutter);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 从绝对路径中提取纹理文件名
//-----------------------------------------------------------------------------
void RemovePathFromFileName(LPSTR fullPath, LPWSTR fileName)
{
	//先将fullPath的类型变换为LPWSTR
	WCHAR wszBuf[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, fullPath, -1, wszBuf, MAX_PATH );
	wszBuf[MAX_PATH-1] = L'\0';

	WCHAR* wszFullPath = wszBuf;

	//从绝对路径中提取文件名
	LPWSTR pch=wcsrchr(wszFullPath,'\\');
	if (pch)
		lstrcpy(fileName, ++pch);
	else
		lstrcpy(fileName, wszFullPath);
}


//-----------------------------------------------------------------------------
// Desc: 创建场景图形
//-----------------------------------------------------------------------------
HRESULT InitGriphics()
{
    LPD3DXBUFFER pD3DXMtrlBuffer;

    //从文件中加载网格模型，创建网格对象
    if( FAILED( D3DXLoadMeshFromX( L"tank.x", D3DXMESH_SYSTEMMEM, 
                                   g_pd3dDevice, NULL, 
                                   &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, 
                                   &g_pMesh ) ) )
    {
		return E_FAIL;
	}

	//提取网格模型材料信息，创建纹理
    D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
    g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
    g_pMeshTextures  = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];

    for( DWORD i=0; i<g_dwNumMaterials; i++ )
    {
        g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
        g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

        g_pMeshTextures[i] = NULL;
        if( d3dxMaterials[i].pTextureFilename != NULL && 
            strlen(d3dxMaterials[i].pTextureFilename) > 0 )
        {
			//获取纹理文件名
			WCHAR filename[256];
			RemovePathFromFileName(d3dxMaterials[i].pTextureFilename, filename);

            //创建纹理
            if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, filename,
                                                &g_pMeshTextures[i] ) ) )
            {
				MessageBox(NULL, L"Could not find texture map", L"ZTest", MB_OK);
            }        
		}
    }

    //释放存放网格模型材料和纹理的缓冲区对象
    pD3DXMtrlBuffer->Release();

	//遮挡板顶点数据
    CUSTOMVERTEX g_Vertices[] =
    {
		{ -8,   -2,  -10.0f,   0xff808080},     
	    { -8,    6,  -10.0f,   0xff808080},	
	    {  8,   -2,  -10.0f,   0xff808080},	
	    {  8,    6,  -10.0f,   0xff808080}

    };
	
	//创建遮挡板顶点缓冲区
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
                                                 0, D3DFVF_CUSTOMVERTEX,
                                                 D3DPOOL_DEFAULT, &g_pShutter,NULL ) ) )
    {
        return E_FAIL;
    }

    //填充遮挡板顶点缓冲区
    VOID* pVertices;
    if( FAILED( g_pShutter->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
    g_pShutter->Unlock();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 释放创建的对象
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    //释放网格模型材质
    if( g_pMeshMaterials != NULL ) 
        delete[] g_pMeshMaterials;

	//释放网格模型纹理
    if( g_pMeshTextures )
    {
        for( DWORD i = 0; i < g_dwNumMaterials; i++ )
        {
            if( g_pMeshTextures[i] )
                g_pMeshTextures[i]->Release();
        }
        delete[] g_pMeshTextures;
    }

	//释放网格模型对象
    if( g_pMesh != NULL )
        g_pMesh->Release();

	//释放遮挡板缓冲区对象
    if( g_pShutter != NULL )
        g_pShutter->Release();
    
	//释放Direct3D设备对象
    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

	//释放Direct3D设备对象
    if( g_pD3D != NULL )
        g_pD3D->Release();
}


//------------------------------------------------------------------------------
// Desc: 渲染图形
//------------------------------------------------------------------------------
VOID Render()
{
    //清除后台颜色缓冲区和深度缓冲区
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
    
    //开始在后台缓冲区绘制图形
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
		//渲染遮挡板
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, false ); 
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matShutter );
		g_pd3dDevice->SetStreamSource( 0, g_pShutter, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);

		//渲染坦克
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true ); 
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matTank );
		for( DWORD i=0; i<g_dwNumMaterials; i++)
        {
			g_pd3dDevice->SetMaterial( &g_pMeshMaterials[i] );
            g_pd3dDevice->SetTexture( 0, g_pMeshTextures[i] );        
            g_pMesh->DrawSubset( i );
        }

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
	case WM_KEYDOWN:
		switch(wParam)
		{
		case 48:  //"0"键, 禁用深度测试
			 g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, false );
			break;

		case 49: //"1"键, 激活深度测试
			 g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, true );
			break;
		}
		return 0;

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
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"ClassName", NULL };
    RegisterClassEx( &wc );

    //创建窗口
    HWND hWnd = CreateWindow( L"ClassName", L"深度测试", 
                              WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//初始化Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    { 
		//创建场景图形
        if( SUCCEEDED( InitGriphics() ) )
        {
            //显示窗口
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            //进入效果循环
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
                    Render();  //渲染图形
				}
            }
        }
    }

    UnregisterClass(L"ClassName", wc.hInstance );
    return 0;
}

