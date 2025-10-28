//=============================================================================
// Desc: ������Ч��
//=============================================================================
#include <d3dx9.h>


//-----------------------------------------------------------------------------
// ȫ�ֱ���
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D           = NULL; //Direct3D����
LPDIRECT3DDEVICE9       g_pd3dDevice     = NULL; //Direct3D�豸����

LPD3DXMESH              g_pMesh          = NULL; //����ģ�Ͷ���
D3DMATERIAL9*           g_pMeshMaterials = NULL; //����ģ�Ͳ���
LPDIRECT3DTEXTURE9*     g_pMeshTextures  = NULL; //����ģ������
DWORD                   g_dwNumMaterials = 0L;   //����ģ���ӿ�����


//----------------------------------------------------------------------------
// Desc: 
//----------------------------------------------------------------------------
inline FLOAT HeightField( FLOAT x, FLOAT z )
{
    FLOAT y = 0.0f;
    y += 10.0f * cosf( 0.051f*x + 0.0f ) * sinf( 0.055f*x + 0.0f );
    y += 10.0f * cosf( 0.053f*z + 0.0f ) * sinf( 0.057f*z + 0.0f );
    y += 2.0f * cosf( 0.101f*x + 0.0f ) * sinf( 0.105f*x + 0.0f );
    y += 2.0f * cosf( 0.103f*z + 0.0f ) * sinf( 0.107f*z + 0.0f );
    y += 2.0f * cosf( 0.251f*x + 0.0f ) * sinf( 0.255f*x + 0.0f );
    y += 2.0f * cosf( 0.253f*z + 0.0f ) * sinf( 0.257f*z + 0.0f );
    return y;
}


//-----------------------------------------------------------------------------
// Desc: ��ʼ��Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	//����Direct3D����, �ö�����������Direct3D�豸����
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

	//����D3DPRESENT_PARAMETERS�ṹ, ׼������Direct3D�豸����
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    //����Direct3D�豸����
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    //���û�����
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

	//���������ɫ
	g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, 0xff0000ff);

	//�����������״̬
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

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
HRESULT InitGeometry()
{
    LPD3DXBUFFER pD3DXMtrlBuffer; //�洢ģ�Ͳ��ϵĻ���������

    //�Ӵ����ļ���������ģ��
    if( FAILED( D3DXLoadMeshFromX( L"seafloor.x", D3DXMESH_SYSTEMMEM, 
                                   g_pd3dDevice, NULL, 
                                   &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, 
                                   &g_pMesh ) ) )
    {
        MessageBox(NULL, L"Could not find seafloor.x", L"Fog", MB_OK);
        return E_FAIL;
    }

    //������ģ������ȡ�������Ժ������ļ��� 
    D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
    g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials]; //���Ͻṹ����

    if( g_pMeshMaterials == NULL )
        return E_OUTOFMEMORY;
    g_pMeshTextures  = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];
    if( g_pMeshTextures == NULL )
        return E_OUTOFMEMORY;

	//�����ȡ����ģ�Ͳ������Ժ������ļ���
    for( DWORD i=0; i<g_dwNumMaterials; i++ )
    {
        //���Ʋ�������
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
                MessageBox(NULL, L"Could not find texture map", L"Fog", MB_OK);
            }
        }
    }

	//�ͷ��ڼ���ģ���ļ�ʱ�����ı���ģ�Ͳ��ʺ�������Ϣ�Ļ���������
    pD3DXMtrlBuffer->Release(); 

	//�޸�����ģ��
	LPDIRECT3DVERTEXBUFFER9 pVB;
    if( SUCCEEDED( g_pMesh->GetVertexBuffer( &pVB ) ) )
    {
		struct VERTEX { FLOAT x,y,z,tu,tv;};
        VERTEX* pVertices;
        DWORD   dwNumVertices = g_pMesh->GetNumVertices();

        pVB->Lock( 0, 0, (void**)&pVertices, 0 );

        for( DWORD i=0; i<dwNumVertices; i++ )
            pVertices[i].y = HeightField( pVertices[i].x, pVertices[i].z );

        pVB->Unlock();
        pVB->Release();
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: �ͷŴ�������
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
    
    //�ͷ�Direct3D�豸����
    if( g_pd3dDevice != NULL) 
        g_pd3dDevice->Release();

	//�ͷ�Direct3D����
    if( g_pD3D != NULL)
        g_pD3D->Release();
}



//-------------------------------------------------
// Desc: ���ñ任����
//--------------------------------------------------
VOID SetupMatrices()
{
    //�����������������
    D3DXMATRIXA16 matWorld;
    D3DXMatrixRotationY( &matWorld, timeGetTime()/1000.0f );
	//D3DXMatrixIdentity(&matWorld);
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    //���������ù۲����
    D3DXVECTOR3 vEyePt( 0.0f, 30.0f,-100.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    //����������ͶӰ����
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 200.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}




//-----------------------------------------------------------------------------
// Desc: ��Ⱦͼ��
//-----------------------------------------------------------------------------
VOID Render()
{
	//��պ�̨������
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
    
     //��ʼ�ں�̨����������ͼ��
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        SetupMatrices();  //�����������

        //�����Ⱦ����ģ��
        for( DWORD i=0; i<g_dwNumMaterials; i++ )
        {
            //���ò��Ϻ�����
            g_pd3dDevice->SetMaterial( &g_pMeshMaterials[i] );
            g_pd3dDevice->SetTexture( 0, g_pMeshTextures[i] );

			//��Ⱦģ��
            g_pMesh->DrawSubset( i );
        }

        //�����ں�̨��������Ⱦͼ��
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
	static float fogStart   = 50;      //��������ʼλ��
	static float fogEnd     = 200;     //����������λ��
	static float fogDensity = 0.01f;   //���Ũ��

	switch( msg )
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage( 0 );
		return 0;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case 48: //"0"��  ������Ч��
			g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, false);       //������
			break;

		case 49:  //"1"��,   ������
			g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, true);        //������

			//������������Ӽ��㹫ʽ
			g_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);

			//������������ʼλ�úͽ���λ��
			g_pd3dDevice->SetRenderState(D3DRS_FOGSTART,  *(DWORD*)&fogStart);
			g_pd3dDevice->SetRenderState(D3DRS_FOGEND,    *(DWORD*)&fogEnd);
			break;

		case 50:  //"2"��,  ָ����
			g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, true);        //������

			//������������Ӽ��㹫ʽ
			g_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);

			//�������Ũ��
			g_pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)&fogDensity);

			break;

		case 51:  //"3"��,  ָ��ƽ����
			g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, true);        //������
			
			//������������Ӽ��㹫ʽ
			g_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP2);

			//�������Ũ��
			g_pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)&fogDensity);
			break;
		}
		return 0;
	}

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Desc: �������
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    //ע�ᴰ����
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"ClassName", NULL };
    RegisterClassEx( &wc );

    //��������
    HWND hWnd = CreateWindow( L"ClassName", L"������", 
                              WS_OVERLAPPEDWINDOW, 200, 100, 600, 500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//��ʼ��Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    { 
		//��������ͼ��
        if( SUCCEEDED( InitGeometry() ) )
        {
            //��ʾ����
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            //������Ϣѭ��
            MSG msg; 
            ZeroMemory( &msg, sizeof(msg) );
            while( msg.message!=WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
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

    UnregisterClass( L"ClassName", wc.hInstance );
    return 0;
}



