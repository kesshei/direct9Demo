//=============================================================================
//Desc: ��Ȳ���
//=============================================================================

#include <d3dx9.h>


//-----------------------------------------------------------------------------
// Desc: ȫ�ֱ���
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D           = NULL; //Direct3D����
LPDIRECT3DDEVICE9       g_pd3dDevice     = NULL; //Direct3D�豸����
LPD3DXMESH              g_pMesh          = NULL; //����ģ�Ͷ���
D3DMATERIAL9*           g_pMeshMaterials = NULL; //����ģ�Ͳ���
LPDIRECT3DTEXTURE9*     g_pMeshTextures  = NULL; //����ģ������
DWORD                   g_dwNumMaterials = 0L;   //����ģ���ӿ�����
LPDIRECT3DVERTEXBUFFER9 g_pShutter       = NULL; //�ڵ��嶥�㻺����
D3DXMATRIXA16           g_matTank;
D3DXMATRIXA16           g_matShutter;


//-----------------------------------------------------------------------------
// Desc: ����ṹ
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{   FLOAT x, y, z;    //����λ��
	DWORD color;      //������������ɫֵ
};
#define D3DFVF_CUSTOMVERTEX   (D3DFVF_XYZ| D3DFVF_DIFFUSE)


//-----------------------------------------------------------------------------
// Desc: ���ù۲�����ͶӰ����
//-----------------------------------------------------------------------------
VOID SetViewAndProjMatrices()
{
    //�����������������
    D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixRotationY( &matWorld, D3DX_PI/2);
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	//���������ù۲����
	D3DXVECTOR3 vEyePt( 0.0f, 2.0f,-30.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 2.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView; 
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//����������ͶӰ����
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 5000.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
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
    d3dpp.EnableAutoDepthStencil = TRUE;            //ʹ����Ȳ���
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;      //��Ȼ�������ʽ

    //����Direct3D�豸����
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    //������Ȳ���,��������Ȳ���
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, true );       //Ĭ�ϼ���
	g_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESS );  //���ñȽϺ�����Ĭ��ΪD3DCMP_LESSEQUAL
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);    //Ĭ��ֵΪTRUE

    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );  //���û�����

	//���ù۲�����ͶӰ����
	SetViewAndProjMatrices();

	//��ʼ��̹���������
	D3DXMatrixIdentity(&g_matTank);
	D3DXMatrixRotationY( &g_matTank, D3DX_PI/2);

	//��ʼ���ڵ����������
	D3DXMatrixIdentity(&g_matShutter);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: �Ӿ���·������ȡ�����ļ���
//-----------------------------------------------------------------------------
void RemovePathFromFileName(LPSTR fullPath, LPWSTR fileName)
{
	//�Ƚ�fullPath�����ͱ任ΪLPWSTR
	WCHAR wszBuf[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, fullPath, -1, wszBuf, MAX_PATH );
	wszBuf[MAX_PATH-1] = L'\0';

	WCHAR* wszFullPath = wszBuf;

	//�Ӿ���·������ȡ�ļ���
	LPWSTR pch=wcsrchr(wszFullPath,'\\');
	if (pch)
		lstrcpy(fileName, ++pch);
	else
		lstrcpy(fileName, wszFullPath);
}


//-----------------------------------------------------------------------------
// Desc: ��������ͼ��
//-----------------------------------------------------------------------------
HRESULT InitGriphics()
{
    LPD3DXBUFFER pD3DXMtrlBuffer;

    //���ļ��м�������ģ�ͣ������������
    if( FAILED( D3DXLoadMeshFromX( L"tank.x", D3DXMESH_SYSTEMMEM, 
                                   g_pd3dDevice, NULL, 
                                   &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, 
                                   &g_pMesh ) ) )
    {
		return E_FAIL;
	}

	//��ȡ����ģ�Ͳ�����Ϣ����������
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
			//��ȡ�����ļ���
			WCHAR filename[256];
			RemovePathFromFileName(d3dxMaterials[i].pTextureFilename, filename);

            //��������
            if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, filename,
                                                &g_pMeshTextures[i] ) ) )
            {
				MessageBox(NULL, L"Could not find texture map", L"ZTest", MB_OK);
            }        
		}
    }

    //�ͷŴ������ģ�Ͳ��Ϻ�����Ļ���������
    pD3DXMtrlBuffer->Release();

	//�ڵ��嶥������
    CUSTOMVERTEX g_Vertices[] =
    {
		{ -8,   -2,  -10.0f,   0xff808080},     
	    { -8,    6,  -10.0f,   0xff808080},	
	    {  8,   -2,  -10.0f,   0xff808080},	
	    {  8,    6,  -10.0f,   0xff808080}

    };
	
	//�����ڵ��嶥�㻺����
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
                                                 0, D3DFVF_CUSTOMVERTEX,
                                                 D3DPOOL_DEFAULT, &g_pShutter,NULL ) ) )
    {
        return E_FAIL;
    }

    //����ڵ��嶥�㻺����
    VOID* pVertices;
    if( FAILED( g_pShutter->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
    g_pShutter->Unlock();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: �ͷŴ����Ķ���
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    //�ͷ�����ģ�Ͳ���
    if( g_pMeshMaterials != NULL ) 
        delete[] g_pMeshMaterials;

	//�ͷ�����ģ������
    if( g_pMeshTextures )
    {
        for( DWORD i = 0; i < g_dwNumMaterials; i++ )
        {
            if( g_pMeshTextures[i] )
                g_pMeshTextures[i]->Release();
        }
        delete[] g_pMeshTextures;
    }

	//�ͷ�����ģ�Ͷ���
    if( g_pMesh != NULL )
        g_pMesh->Release();

	//�ͷ��ڵ��建��������
    if( g_pShutter != NULL )
        g_pShutter->Release();
    
	//�ͷ�Direct3D�豸����
    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

	//�ͷ�Direct3D�豸����
    if( g_pD3D != NULL )
        g_pD3D->Release();
}


//------------------------------------------------------------------------------
// Desc: ��Ⱦͼ��
//------------------------------------------------------------------------------
VOID Render()
{
    //�����̨��ɫ����������Ȼ�����
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
    
    //��ʼ�ں�̨����������ͼ��
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
		//��Ⱦ�ڵ���
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, false ); 
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matShutter );
		g_pd3dDevice->SetStreamSource( 0, g_pShutter, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);

		//��Ⱦ̹��
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, true ); 
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matTank );
		for( DWORD i=0; i<g_dwNumMaterials; i++)
        {
			g_pd3dDevice->SetMaterial( &g_pMeshMaterials[i] );
            g_pd3dDevice->SetTexture( 0, g_pMeshTextures[i] );        
            g_pMesh->DrawSubset( i );
        }

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
	case WM_KEYDOWN:
		switch(wParam)
		{
		case 48:  //"0"��, ������Ȳ���
			 g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, false );
			break;

		case 49: //"1"��, ������Ȳ���
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
// Desc: ��ں���
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    //ע�ᴰ����
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"ClassName", NULL };
    RegisterClassEx( &wc );

    //��������
    HWND hWnd = CreateWindow( L"ClassName", L"��Ȳ���", 
                              WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//��ʼ��Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    { 
		//��������ͼ��
        if( SUCCEEDED( InitGriphics() ) )
        {
            //��ʾ����
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            //����Ч��ѭ��
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
                    Render();  //��Ⱦͼ��
				}
            }
        }
    }

    UnregisterClass(L"ClassName", wc.hInstance );
    return 0;
}

