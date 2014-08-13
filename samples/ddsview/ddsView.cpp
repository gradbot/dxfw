//--------------------------------------------------------------------------------------
// File: SimpleSample11.cpp
//
// Starting point for new Direct3D 11 samples.  For a more basic starting point, 
// use the EmptyProject11 sample instead.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTmisc.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "DDSTextureLoader.h"
#include "DXUTgui.h"
#include <DirectXMath.h>
#include "../../directxtex/DirectXTex/DirectXTex.h"

#include <atlbase.h>

#include <string>
#include <vector>
#include <sstream>

using namespace DirectX;

#include <filesystem>
namespace fs = std::tr2::sys;

const float kDefaultIndex = 0.0f;
const float kDefaultBias = 0.0f;
const float kDefaultScale = 1.0f;

//--------------------------------------------------------------------------------------

struct SimpleVertex
{
    XMFLOAT4 Pos;
    XMFLOAT4 Tex;
};

enum RenderChannel
{
    CHANNEL_RGBA,
    CHANNEL_R,
    CHANNEL_G,
    CHANNEL_B,
    CHANNEL_A,
    CHANNEL_COUNT,
};

#pragma pack(push,4)
// must be multiplier of 16
struct LogicControl
{
    float   index;
    float   scale;
    float   bias;
    bool    depthMode;
    bool    b0[3];

    RenderChannel  renderChannel;
    bool    normalized;
    bool    b2[3];
    float   width;
    float	height;
}g_LogicControl =
{
    kDefaultIndex, kDefaultScale, kDefaultBias
};
#pragma pack(pop)

namespace GUI
{
    int iY = 0;
    const int kDeltaY = 24;
    const int kBtnW = 24;
    const int kBtnH = 24;
}

// loop the folder
std::vector<fs::wpath> g_ddsFilesInFolder;
int g_ddsFileIndex;

bool IsSingleChannelFormat(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------------
#include "shaders/vs.h"
#include "shaders/ps1D.h"
#include "shaders/ps1Darray.h"
#include "shaders/ps2D.h"
#include "shaders/ps2D_R8uint.h"
#include "shaders/ps2Darray.h"
#include "shaders/ps3D.h"
#include "shaders/psCube.h"
#include "shaders/psStencil.h"

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs
CDXUTDialog                 g_TopPanel; 
CDXUTTextHelper*            g_pTxtHelper = nullptr;

// CComPtr<> is not useful here because of OnDestroyDevice()
ID3D11VertexShader*         g_pVertexShader = nullptr;
ID3D11PixelShader*          g_pCurrentPS = nullptr;
ID3D11PixelShader*          g_pMainPixelShader = nullptr;
ID3D11PixelShader*          g_pStencilPixelShader = nullptr;
ID3D11InputLayout*          g_pVertexLayout = nullptr;
ID3D11Buffer*               g_pVertexBuffer = nullptr;
ID3D11Buffer*               g_pIndexBuffer = nullptr;
ID3D11Buffer*               g_pCBArrayControl = nullptr;
ID3D11ShaderResourceView*   g_pCurrentSRV = nullptr;
ID3D11ShaderResourceView*   g_pMainSRV = nullptr;
ID3D11ShaderResourceView*   g_pStencilSRV = nullptr;
ID3D11BlendState*           g_AlphaBlendState = nullptr;
ID3D11SamplerState*         g_pSamplerLinear = nullptr;

int                         g_iMaxSliceCount = 1;
UINT                        g_iIndexBufferCount = 0;

TexMetadata                 g_ImageInfo;
std::wstring                g_ImageFileName;
bool                        g_IsDepthMode = false;
bool                        g_IsStencilMode = false;

//--------------------------------------------------------------------------------------
// Constant buffers
//--------------------------------------------------------------------------------------
const wchar_t               kTitleName[] = L"ddsView";

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );

HRESULT CALLBACK OnCreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                void* pUserContext );
HRESULT CALLBACK OnResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                    const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnReleasingSwapChain( void* pUserContext );
void CALLBACK OnDestroyDevice( void* pUserContext );
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
void CALLBACK OnFrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                            float fElapsedTime, void* pUserContext );

LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                         void* pUserContext );
void RenderText();

HRESULT BeforeLoadingSrv(const std::wstring& filename);
HRESULT OnLoadingSrv(ID3D11Device* pd3dDevice);
void OnReleasingSrv();

bool    g_bShowInfo             = true;

enum
{
    IDC_TOGGLE_SHOW_INFO,
    IDC_SLIDER_CHANNEL,
    IDC_TOGGLE_NORMALIZED_RENDERING,
};

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
    case IDC_TOGGLE_NORMALIZED_RENDERING:
        {
            break;
        }
    default:
        {
            break;
        }
    }
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    std::wstring imageFileName;
    int nNumArgs;
    LPWSTR* pstrArgList = CommandLineToArgvW( GetCommandLine(), &nNumArgs );
    if (nNumArgs >= 2)
    {
        imageFileName = pstrArgList[1];
    }
    else
    {
        MessageBox( NULL, L"Usage: ddsView.exe <filename>", kTitleName, MB_OK | MB_ICONEXCLAMATION );
        return 0;
    }

    TexMetadata tempInfo;

    HRESULT hr = GetMetadataFromDDSFile( imageFileName.c_str(), DDS_FLAGS_NONE, tempInfo);
    if ( FAILED(hr) )
    {
        WCHAR buff[2048];
        swprintf_s( buff, L"Failed to open texture file\n\nFilename = %s\n %s", 
            imageFileName.c_str(), DXGetErrorString(hr) );
        MessageBox( NULL, buff, kTitleName, MB_OK | MB_ICONEXCLAMATION );
        return 0;
    }

    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackMsgProc( MsgProc );

    DXUTSetCallbackD3D11DeviceCreated( OnCreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnResizedSwapChain );
    DXUTSetCallbackD3D11SwapChainReleasing( OnReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackD3D11FrameRender( OnFrameRender );

    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( kTitleName );

    g_TopPanel.Init( &g_DialogResourceManager );
    g_TopPanel.SetCallback( OnGUIEvent );
    // add controls
    {
        g_TopPanel.AddCheckBox( IDC_TOGGLE_SHOW_INFO, L"Show (i)nfo", 0, GUI::iY += GUI::kDeltaY, GUI::kBtnW, GUI::kBtnH, 
            true, 'I' );
        g_TopPanel.AddSlider( IDC_SLIDER_CHANNEL, 10, GUI::iY += GUI::kDeltaY, GUI::kBtnW*4, GUI::kBtnH, 
            CHANNEL_RGBA, CHANNEL_A, CHANNEL_RGBA);
#ifdef VERSION_1_0_ENABLED
        g_TopPanel.AddCheckBox( IDC_TOGGLE_NORMALIZED_RENDERING, L"(N)ormalized", 0, GUI::iY += GUI::kDeltaY, GUI::kBtnW, GUI::kBtnH, false, 'N');
#endif
    }

    V_RETURN(BeforeLoadingSrv(imageFileName)); 

    DXUTMainLoop(); // Enter into the DXUT render loop

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos( 5, 5 );

    const XMFLOAT4 fontColors[CHANNEL_COUNT]=
    {
        XMFLOAT4(1, 1, 1, 1),
        XMFLOAT4(1, 0, 0, 1),
        XMFLOAT4(0, 1, 0, 1),
        XMFLOAT4(0, 0, 1, 1),
        XMFLOAT4(0.5, 0.5, 0.5, 1),
    };

    g_pTxtHelper->SetForegroundColor(fontColors[(int)g_LogicControl.renderChannel]);

    wchar_t info[MAX_PATH];

    swprintf_s(info, MAX_PATH, L"Scale(A/S)= %.3f, Bias(Q/W)= %.3f",
        g_LogicControl.scale, g_LogicControl.bias);
    g_pTxtHelper->DrawTextLine( info );

    if (g_iMaxSliceCount == 1)
    {
        swprintf_s(info, MAX_PATH, L"%s\n%d X %d", 
            DXUTDXGIFormatToString(g_ImageInfo.format, false), 
            g_ImageInfo.width, g_ImageInfo.height);
    }
    else
    {
        swprintf_s(info, MAX_PATH, L"%s\n%d X %d X %d\n[#%d of %d] Press LEFT|RIGHT", 
            DXUTDXGIFormatToString(g_ImageInfo.format, false), 
            g_ImageInfo.width, g_ImageInfo.height, g_iMaxSliceCount,  //Width * Height * Depth or ArraySize
            (UINT)g_LogicControl.index, g_iMaxSliceCount);
    }
    g_pTxtHelper->DrawTextLine( info );

    g_pTxtHelper->End();
}

//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependent on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                void* pUserContext )
{
    HRESULT hr = S_OK;

    ID3D11DeviceContext* context = DXUTGetD3D11DeviceContext();
    V_RETURN( g_DialogResourceManager.OnD3D11CreateDevice( pd3dDevice, context ) );
    g_pTxtHelper = new CDXUTTextHelper( pd3dDevice, context, &g_DialogResourceManager, 20 );

    {
        static const WORD indices[] =
        {
            0, 1, 2,
            2, 1, 3
        };
        D3D11_SUBRESOURCE_DATA InitData;
        ZeroMemory( &InitData, sizeof(InitData) );
        g_iIndexBufferCount = _countof(indices);
        InitData.pSysMem = indices;
        CD3D11_BUFFER_DESC desc(g_iIndexBufferCount * sizeof(WORD), D3D11_BIND_INDEX_BUFFER);
        V_RETURN(pd3dDevice->CreateBuffer( &desc, &InitData, &g_pIndexBuffer ));
    }

    {
        CD3D11_BUFFER_DESC desc(sizeof(LogicControl), D3D11_BIND_CONSTANT_BUFFER);
        V_RETURN(pd3dDevice->CreateBuffer( &desc, NULL, &g_pCBArrayControl ));
    }

    {
        CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        V_RETURN(pd3dDevice->CreateSamplerState( &desc, &g_pSamplerLinear ));
    }

    {
        CD3D11_BLEND_DESC desc(D3D11_DEFAULT);
        V_RETURN(pd3dDevice->CreateBlendState(&desc, &g_AlphaBlendState ));
    }

    V_RETURN(pd3dDevice->CreateVertexShader( g_VS, sizeof(g_VS), NULL, &g_pVertexShader ));

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(XMFLOAT4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    V_RETURN(pd3dDevice->CreateInputLayout( layout, ARRAYSIZE( layout ), g_VS, sizeof(g_VS), &g_pVertexLayout ));

    V_RETURN(pd3dDevice->CreatePixelShader( g_PS_Stencil, sizeof(g_PS_Stencil), NULL, &g_pStencilPixelShader));

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                    const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    //::SetWindowPos( DXUTGetHWNDDeviceWindowed(), 0, 10, 10, DXUTGetWindowWidth(), DXUTGetWindowWi, SWP_NOZORDER | SWP_NOMOVE );
    HRESULT hr = S_OK;

    V_RETURN( g_DialogResourceManager.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );

    g_TopPanel.SetLocation( 5, 70 );
    g_TopPanel.SetSize( 170, 300 );

    OnLoadingSrv(pd3dDevice);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* context, double fTime,
                            float fElapsedTime, void* pUserContext )
{
    float ClearColor[4] = { 0.f, 0.f, 0.f, 0.0f }; // RGBA
    context->ClearRenderTargetView( DXUTGetD3D11RenderTargetView(), ClearColor );
    context->ClearDepthStencilView( DXUTGetD3D11DepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

    // update const buffer
    {
        context->UpdateSubresource( g_pCBArrayControl, 0, NULL, &g_LogicControl, 0, 0 );
    }

    // IA
    {
        context->IASetInputLayout( g_pVertexLayout );
        UINT stride = sizeof( SimpleVertex );
        UINT offset = 0;
        context->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );
        context->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
        context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    }

    // shader stage
    {
        context->VSSetShader( g_pVertexShader, NULL, 0 );
        context->PSSetShader( g_pCurrentPS, NULL, 0 );
        context->PSSetConstantBuffers( 0, 1, &g_pCBArrayControl );
        context->PSSetShaderResources( 0, 1, &g_pCurrentSRV );
        context->PSSetSamplers( 0, 1, &g_pSamplerLinear );
    }

    float bf [4] = {1.0f, 1.0f, 1.0f, 1.0f};
    context->OMSetBlendState( NULL, bf, 0xffffffff );
    context->DrawIndexed( g_iIndexBufferCount, 0, 0 );

    if (g_bShowInfo)
    {
        // Render objects here...
        DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
        RenderText();
        DXUT_EndPerfEvent();
    }

    g_TopPanel.OnRender( fElapsedTime );
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnReleasingSwapChain( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11ReleasingSwapChain();

    OnReleasingSrv();
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();
    SAFE_DELETE( g_pTxtHelper );

    SAFE_RELEASE( g_pSamplerLinear );
    SAFE_RELEASE( g_AlphaBlendState );
    SAFE_RELEASE( g_pIndexBuffer );
    SAFE_RELEASE( g_pCBArrayControl );
    SAFE_RELEASE( g_pVertexLayout );
    SAFE_RELEASE( g_pVertexShader );
    SAFE_RELEASE( g_pStencilPixelShader );

    OnReleasingSrv();
}

//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    const float kDeltaBias = 0.05f;
    const float kDeltaScale = 2.0f;

    if (bKeyDown)
    {
        switch (nChar)
        {
            // channel selection
        case '1':
        case '2':
            {
                int idx = g_TopPanel.GetSlider(IDC_SLIDER_CHANNEL)->GetValue();
                if (nChar == '1')
                    idx --;
                else /*if (nChar == '2')*/
                    idx ++;
                idx = std::min<int>(std::max<int>(idx, CHANNEL_RGBA), CHANNEL_A);
                g_TopPanel.GetSlider(IDC_SLIDER_CHANNEL)->SetValue(idx);
            }break;
            // directory traverse
        case VK_UP:
        case VK_DOWN:
        case VK_PRIOR:
        case VK_NEXT:
        case VK_HOME:
        case VK_END:
            {
                int delta = 0;
                int d0 = 1;
                int d1 = std::max<int>(g_ddsFilesInFolder.size()/40, 4);
                int d2 = std::max<int>(g_ddsFilesInFolder.size()/10, 20);
                switch (nChar)
                {
                case VK_UP:     delta = -d0;    break;
                case VK_DOWN:   delta = +d0;    break;
                case VK_PRIOR:  delta = -d1;    break;
                case VK_NEXT:   delta = +d1;    break;
                case VK_HOME:   delta = -d2;    break;
                case VK_END:    delta = +d2;    break;
                }

                g_ddsFileIndex += delta;
                g_ddsFileIndex = std::min<int>(std::max<int>(g_ddsFileIndex, 0), g_ddsFilesInFolder.size()-1);

                std::wstring nextFileName = g_ddsFilesInFolder[g_ddsFileIndex].string();
                BeforeLoadingSrv(nextFileName);
            }break;

            // param of tuning
        case VK_RIGHT:
            {
                if ( g_LogicControl.index < g_iMaxSliceCount-1 )
                    ++g_LogicControl.index;
            }break;
        case VK_LEFT:
            {
                if ( g_LogicControl.index > 0 )
                {
                    --g_LogicControl.index;
                }
            }break;
        case 'Q':
            {
                g_LogicControl.bias += kDeltaBias;
            }break;  
        case 'W':
            {
                g_LogicControl.bias -= kDeltaBias;
            }break;
        case 'A':
            {
                g_LogicControl.scale *= kDeltaScale;
            }break;  
        case 'S':
            {
                g_LogicControl.scale /= kDeltaScale;
            }break;
        case VK_SPACE:
            {
                g_LogicControl.index = kDefaultIndex;
                g_LogicControl.bias = kDefaultBias;
                g_LogicControl.scale = kDefaultScale;
                g_TopPanel.GetSlider(IDC_SLIDER_CHANNEL)->SetValue(g_IsDepthMode ? CHANNEL_R : CHANNEL_RGBA);
            }break;

        default:
            break;
        }
    }
}


//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                         void* pUserContext )
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    *pbNoFurtherProcessing = g_TopPanel.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    return 0;
}

HRESULT BeforeLoadingSrv( const std::wstring& filename )
{
    HRESULT hr = S_OK;

    g_ImageFileName = filename;
    V_RETURN(GetMetadataFromDDSFile(g_ImageFileName.c_str(), DDS_FLAGS_NONE, g_ImageInfo));

    const int spacingSize = 100;
    int screenX = GetSystemMetrics( SM_CXSCREEN ) - spacingSize;
    int screenY = GetSystemMetrics( SM_CYSCREEN ) - spacingSize;
    UINT suggestedWidth = g_ImageInfo.width;
    UINT suggestedHeight = g_ImageInfo.height;

    float aspectRatio = static_cast<float>(suggestedWidth)/suggestedHeight;
    if (aspectRatio > 1.0f)
    {
        // width major 
        suggestedWidth = std::max<UINT>(std::min<UINT>(suggestedWidth, screenX), 640);
        suggestedHeight = static_cast<UINT>(suggestedWidth / aspectRatio);
    }
    else
    {
        // height major 
        suggestedHeight = std::max<UINT>(std::min<UINT>(suggestedHeight, screenY), 480);
        suggestedWidth = static_cast<UINT>(suggestedHeight * aspectRatio);
    }

    int remoteMetric = ::GetSystemMetrics(SM_REMOTESESSION);
    if (remoteMetric)
    {
        suggestedWidth /= 2;
        suggestedHeight /= 2;
    }

    g_LogicControl.width = static_cast<float>(suggestedWidth);
    g_LogicControl.height = static_cast<float>(suggestedHeight);

#ifdef SRGB_DISABLED
    DXUTSetIsInGammaCorrectMode(false); // disable SRGB mode
#endif
    static bool firstTime = true;

    if (firstTime)
    {
        firstTime = false;

        fs::wpath currentDds(g_ImageFileName);
        fs::wpath ddsFolder = currentDds.parent_path();

        fs::wdirectory_iterator end_iter;
        for ( fs::wdirectory_iterator it(ddsFolder); it != end_iter; ++it)
        {
            if (fs::is_regular_file(it->path()))
            {
                std::wstring extension = fs::extension(it->path());
                //boost::to_lower(extension);
                if (extension != L".dds")
                    continue;

                if (it->path() == currentDds)
                    g_ddsFileIndex = g_ddsFilesInFolder.size();
                g_ddsFilesInFolder.push_back(*it);
            }
        }

        V_RETURN(DXUTCreateDevice( D3D_FEATURE_LEVEL_10_0, true, suggestedWidth, suggestedHeight ));
    }
    else
    {
        IDXGISwapChain* swapChain = DXUTGetDXGISwapChain();
        if (swapChain == NULL)
            return E_FAIL;
        DXGI_SWAP_CHAIN_DESC chainDesc;
        swapChain->GetDesc(&chainDesc);
        DXGI_MODE_DESC bufferDesc = chainDesc.BufferDesc;
        if (bufferDesc.Width == suggestedWidth && bufferDesc.Height == suggestedHeight)
        {
            OnReleasingSrv();
            OnLoadingSrv(DXUTGetD3D11Device());
        }
        else
        {
            bufferDesc.Width = suggestedWidth;
            bufferDesc.Height = suggestedHeight;
            V_RETURN(DXUTGetDXGISwapChain()->ResizeTarget(&bufferDesc));
        }
    }

    return S_OK;
}

void OnReleasingSrv()
{
    SAFE_RELEASE( g_pMainSRV );
    SAFE_RELEASE( g_pStencilSRV );

    SAFE_RELEASE( g_pMainPixelShader );
    SAFE_RELEASE( g_pVertexBuffer );
}

HRESULT OnLoadingSrv(ID3D11Device* pd3dDevice)
{
    fs::wpath node = fs::wpath(g_ImageFileName).leaf();
    SetWindowText(DXUTGetHWND(), node.string().c_str());

    if (g_ImageInfo.dimension == TEX_DIMENSION_TEXTURE3D)
    {
        if ( g_ImageInfo.arraySize > 1 )
        {
            WCHAR buff[2048];
            swprintf_s( buff, L"Arrays of volume textures are not supported\n\nFilename = %s\nArray size %d", g_ImageFileName.c_str(), g_ImageInfo.arraySize );
            MessageBox( NULL, buff, kTitleName, MB_OK | MB_ICONEXCLAMATION );
            return E_FAIL;
        }

        g_iMaxSliceCount = static_cast<UINT>( g_ImageInfo.depth );
    }
    else
    {
        g_iMaxSliceCount = static_cast<UINT>( g_ImageInfo.arraySize );
    }

    UINT flags = 0;
    HRESULT hr = pd3dDevice->CheckFormatSupport ( g_ImageInfo.format, &flags );
    if ( FAILED(hr) || !(flags & (D3D11_FORMAT_SUPPORT_TEXTURE1D|D3D11_FORMAT_SUPPORT_TEXTURE2D|D3D11_FORMAT_SUPPORT_TEXTURE3D)) )
    {
        WCHAR buff[2048];
        swprintf_s( buff, L"Format not supported by DirectX hardware\n\nFilename = %s\nDXGI Format %d\nFeature Level %d\n %s", 
            g_ImageFileName.c_str(), g_ImageInfo.format, DXGetErrorString(hr) );
        MessageBox( NULL, buff, kTitleName, MB_OK | MB_ICONEXCLAMATION );
        return hr;
    }

    // Create SRV
    bool isTextureArray = false;
    if (g_ImageInfo.miscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
    {
        // always make it non-texture-cube
        g_ImageInfo.miscFlags &= ~D3D11_RESOURCE_MISC_TEXTURECUBE;
    }

    if (g_ImageInfo.arraySize > 1)
        isTextureArray = true;

    if (g_LogicControl.index > g_iMaxSliceCount - 1)
        g_LogicControl.index = 0;

    TexMetadata loadInfo = g_ImageInfo;
    // Patching

    bool isDepthMode = IsSingleChannelFormat(loadInfo.format);

    if (isDepthMode && !g_IsDepthMode)
    {
        // only when RGBA_tex ==> depth_tex, set the value to CHANNEL_R
        g_TopPanel.GetSlider( IDC_SLIDER_CHANNEL )->SetValue(CHANNEL_R);
    }

    if (g_IsDepthMode && !isDepthMode)
    {
        // only when depth_tex ==> RGBA_tex, set the value to CHANNEL_RGBA
        g_TopPanel.GetSlider( IDC_SLIDER_CHANNEL )->SetValue(CHANNEL_RGBA);
    }

    g_IsDepthMode = isDepthMode;

    hr = E_FAIL;
    ScratchImage image;
    hr = LoadFromDDSFile( g_ImageFileName.c_str(), DDS_FLAGS_NONE, &g_ImageInfo, image );
    if ( FAILED(hr) )
    {
        WCHAR buff[2048];
        swprintf_s( buff, L"Failed to load texture file\n\nFilename = %s\nHRESULT %08X", g_ImageFileName.c_str(), hr );
        MessageBox( NULL, buff, L"DDSView", MB_OK | MB_ICONEXCLAMATION );
        return 0;
    }

    // Special case to make sure Texture cubes remain arrays
    g_ImageInfo.miscFlags &= ~TEX_MISC_TEXTURECUBE;

    hr = CreateShaderResourceView( pd3dDevice, image.GetImages(), image.GetImageCount(), g_ImageInfo, &g_pMainSRV );
    if ( FAILED(hr) )
    {
        WCHAR buff[2048];
        swprintf_s( buff, L"Failed creating texture from file\n\nFilename = %s\n %s", 
            g_ImageFileName.c_str(), DXGetErrorString(hr) );
        MessageBox( NULL, buff, kTitleName, MB_OK | MB_ICONEXCLAMATION );
        return hr;
    }

    g_pCurrentSRV = g_pMainSRV;

    // Select the pixel shader
    bool is1D = false;
    const BYTE* pshader = nullptr;
    size_t pshader_size = 0;

    switch ( g_ImageInfo.dimension )
    {
    case TEX_DIMENSION_TEXTURE1D:
        if ( g_ImageInfo.arraySize > 1)
        {
            pshader = g_PS_1DArray;
            pshader_size = sizeof(g_PS_1DArray);
        }
        else
        {
            pshader = g_PS_1D;
            pshader_size = sizeof(g_PS_1D);
        }
        is1D = true;
        break;

    case TEX_DIMENSION_TEXTURE2D:
        if (loadInfo.format == DXGI_FORMAT_R8_UINT)
        {
            pshader = g_PS_2D_R8uint;
            pshader_size = sizeof(g_PS_2D_R8uint);
        }
        else if ( g_ImageInfo.miscFlags & TEX_MISC_TEXTURECUBE )
        {
            // should never arriver here
            pshader = g_PS_Cube;
            pshader_size = sizeof(g_PS_Cube);
        }
        else if ( g_ImageInfo.arraySize > 1 )
        {
            pshader = g_PS_2DArray;
            pshader_size = sizeof(g_PS_2DArray);
        }
        else
        {
            pshader = g_PS_2D;
            pshader_size = sizeof(g_PS_2D);
        }
        break;

    case TEX_DIMENSION_TEXTURE3D:
        pshader = g_PS_3D;
        pshader_size = sizeof(g_PS_3D);
        break;

    default:
        return E_FAIL;
    }

    assert( pshader && pshader_size > 0 );

    // Create the pixel shader
    V_RETURN(pd3dDevice->CreatePixelShader( pshader, pshader_size, NULL, &g_pMainPixelShader ));

    g_pCurrentPS = g_pMainPixelShader;

    // Create vertex buffer
    static const SimpleVertex vertices[] =
    {
        { XMFLOAT4( -1.f, 1.f, 0.f, 1.f ), XMFLOAT4( 0.f, 0.f, 0.f, 0.f ) },
        { XMFLOAT4( 1.f, 1.f, 0.f, 1.f ), XMFLOAT4( 1.f, 0.f, 0.f, 0.f ) },
        { XMFLOAT4( -1.f, -1.f, 0.f, 1.f ), XMFLOAT4( 0.f, 1.f, 0.f, 0.f ) },
        { XMFLOAT4( 1.f, -1.f, 0.f, 1.f ), XMFLOAT4( 1.f, 1.f, 0.f, 0.f ) },
    };

    float cell1DHeight = static_cast<float>(g_ImageInfo.width) * DXUTGetWindowHeight() / DXUTGetWindowWidth();
    SimpleVertex vertices1D[] =
    {
        { XMFLOAT4( -1.f, 1/cell1DHeight, 0.f, 1.f ), XMFLOAT4( 0.f, 0.f, 0.f, 0.f ) },
        { XMFLOAT4( 1.f, 1/cell1DHeight, 0.f, 1.f ), XMFLOAT4( 1.f, 0.f, 0.f, 0.f ) },
        { XMFLOAT4( -1.f, -1/cell1DHeight, 0.f, 1.f ), XMFLOAT4( 0.f, 0.f, 0.f, 0.f ) },
        { XMFLOAT4( 1.f, -1/cell1DHeight, 0.f, 1.f ), XMFLOAT4( 1.f, 0.f, 0.f, 0.f ) },
    };

    UINT nverts;
    D3D11_SUBRESOURCE_DATA InitData = {0};

    if ( is1D )
    {
        nverts = _countof(vertices1D);
        InitData.pSysMem = vertices1D;
    }
    else
    {
        nverts = _countof(vertices);
        InitData.pSysMem = vertices;
    }

    {
        CD3D11_BUFFER_DESC desc(sizeof( SimpleVertex ) * nverts, D3D11_BIND_VERTEX_BUFFER);
        V_RETURN(pd3dDevice->CreateBuffer( &desc, &InitData, &g_pVertexBuffer ));
    }

    return S_OK;
}

void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    g_bShowInfo = g_TopPanel.GetCheckBox( IDC_TOGGLE_SHOW_INFO )->GetChecked();
    g_LogicControl.renderChannel = (RenderChannel)g_TopPanel.GetSlider( IDC_SLIDER_CHANNEL )->GetValue();
#ifdef VERSION_1_0_ENABLED
    g_LogicControl.normalized = g_TopPanel.GetCheckBox( IDC_TOGGLE_NORMALIZED_RENDERING )->GetChecked();
#endif
    if (g_IsStencilMode && g_LogicControl.renderChannel == CHANNEL_G)
    {
        g_pCurrentPS = g_pStencilPixelShader;
        g_pCurrentSRV = g_pStencilSRV;
    }
    else
    {
        g_pCurrentPS = g_pMainPixelShader;
        g_pCurrentSRV = g_pMainSRV;
    }
}
