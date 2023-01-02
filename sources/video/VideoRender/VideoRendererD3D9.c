/***********************************************************************************
 * @ file    : VideoRendererD3D9.h
 * @ author  : oheiheiheiheihei
 * @ version : 0.9
 * @ date    : 2022.10.02 11:21
 * @ brief   : 实现Direct3D9版本的视频渲染器
 * @ remark  ：
 ************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <d3d9.h>
#include <libY.h>

#include "VideoRenderer.h"

#pragma comment(lib, "d3d9.lib")

typedef struct Direct3D9Renderer
{
    IDirect3D9 *d3d9;
    IDirect3DDevice9 *d3ddev9;
    IDirect3DSurface9 *d3dsurface9;
}Direct3D9Renderer;


static D3DFORMAT VIDEO_FORMAT2D3DFORMAT(VideoFormats format)
{
    switch (format)
    {
    case VIDEO_FORMAT_YV12: return (D3DFORMAT)MAKEFOURCC('Y','V','1','2');
    default: return -1;
    }
}

int VideoRendererActionsDirect3D9Initialize(VideoRenderer *renderer)
{
    IDirect3D9 *d3d9 = NULL;
    IDirect3DDevice9 *d3ddev9 = NULL;
    IDirect3DSurface9 *d3dsurface9 = NULL;

    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    if(d3d9 == NULL)
    {
        YLOGE(("Durect3DCreate9 failed, %d"), GetLastError());
        return YERR_FAILED;
    }

    VideoRendererOptions *options = renderer->Options;

    D3DPRESENT_PARAMETERS d3dpp;
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
	d3dpp.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
    d3dpp.hDeviceWindow = (HWND)options->Drawable;
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	HRESULT r = d3d9->lpVtbl->CreateDevice(d3d9, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)options->Drawable, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &d3ddev9);
	if(FAILED(r))
    {
        YLOGE(("IDirect3D9->CreateDevice failed, %d"), r);
        return YERR_FAILED;
    }

    D3DFORMAT d3dfmt = VIDEO_FORMAT2D3DFORMAT(options->Format);
	r = d3ddev9->lpVtbl->CreateOffscreenPlainSurface(d3ddev9, (UINT)options->VideoWidth, (UINT)options->VideoHeight, d3dfmt, D3DPOOL_DEFAULT, &d3dsurface9, NULL);
	if(FAILED(r))
    {
        YLOGE(("IDirect3DDevice9->CreateOffscreenPlainSurface failed, %d"), r);
        return YERR_FAILED;
    }

    Direct3D9Renderer *d3d9Renderer = (Direct3D9Renderer*)calloc(1, sizeof(Direct3D9Renderer));
    d3d9Renderer->d3d9 = d3d9;
    d3d9Renderer->d3ddev9 = d3ddev9;
    d3d9Renderer->d3dsurface9 = d3dsurface9;

    renderer->ActionsData = d3d9Renderer;
    return YERR_SUCCESS;
}

void VideoRendererActionsDirect3D9Release(VideoRenderer *renderer)
{
    Direct3D9Renderer *d3d9Renderer = (Direct3D9Renderer*)renderer->ActionsData;
    d3d9Renderer->d3dsurface9->lpVtbl->Release(d3d9Renderer->d3dsurface9);
    d3d9Renderer->d3ddev9->lpVtbl->Release(d3d9Renderer->d3ddev9);
    d3d9Renderer->d3d9->lpVtbl->Release(d3d9Renderer->d3d9);
    free(d3d9Renderer);
}

int VideoRendererActionsDirect3D9Render(VideoRenderer *renderer, char *videoData)
{
    Direct3D9Renderer *d3d9Renderer = (Direct3D9Renderer*)renderer->ActionsData;

    D3DLOCKED_RECT lockedrect;
    HRESULT hr = d3d9Renderer->d3dsurface9->lpVtbl->LockRect(d3d9Renderer->d3dsurface9, &lockedrect, NULL, D3DLOCK_DISCARD);
    if(FAILED(hr))
    {
        YLOGE("d3dsurface9->LockRect failed, %d", hr);
        return YERR_FAILED;
    }

    int videoHeight = renderer->Options->VideoHeight;
    int videoWidth = renderer->Options->VideoWidth;

    switch (renderer->Options->Format)
    {
        case VIDEO_FORMAT_YV12:
        {			
            int stride = lockedrect.Pitch;
            byte *PSrc = (byte *)videoData;
            byte* PSrcY = (LPBYTE)PSrc;
			byte* PSrcU = (LPBYTE)PSrc + videoWidth * videoHeight * 5 / 4;
			byte* PSrcV = (LPBYTE)PSrc + videoWidth * videoHeight;
			byte* pDest = (BYTE*)lockedrect.pBits;
			byte* PDesY = (LPBYTE)pDest;
			byte* PDesU = (LPBYTE)pDest + stride * videoHeight * 5 /  4;
			byte* PDesV = (LPBYTE)pDest + stride * videoHeight;
			for(unsigned int i = 0; i < videoHeight; i++)
			{
				memcpy(pDest+i*stride, PSrc+i * videoWidth, videoWidth);
			}
			for(unsigned int i = 0; i < videoHeight / 2; i++)
			{
				memcpy(PDesU+i*stride/2, PSrcU + i * videoWidth / 2, videoWidth / 2);
			}
			for(unsigned int i = 0; i < videoHeight / 2; i++)
			{
				memcpy(PDesV+i*stride/2, PSrcV + i * videoWidth / 2, videoWidth / 2);
			}
            break;
        }
    
        default: return YERR_VIDEO_FORMAT_NOT_SUPPORTED;
    }

    if((FAILED(hr = d3d9Renderer->d3dsurface9->lpVtbl->UnlockRect(d3d9Renderer->d3dsurface9))))
    {
        YLOGE("d3dsurface9->UnlockRect failed, %d", hr);
        return YERR_FAILED;
    }

    IDirect3DSurface9 *pBackBuffer = NULL;
    d3d9Renderer->d3ddev9->lpVtbl->Clear(d3d9Renderer->d3ddev9, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XYUV(0, 0, 0), 1.0f, 0);
    d3d9Renderer->d3ddev9->lpVtbl->BeginScene(d3d9Renderer->d3ddev9);
    d3d9Renderer->d3ddev9->lpVtbl->GetBackBuffer(d3d9Renderer->d3ddev9, 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    d3d9Renderer->d3ddev9->lpVtbl->StretchRect(d3d9Renderer->d3ddev9, d3d9Renderer->d3dsurface9, NULL, pBackBuffer, NULL, D3DTEXF_LINEAR);
    d3d9Renderer->d3ddev9->lpVtbl->EndScene(d3d9Renderer->d3ddev9);
    d3d9Renderer->d3ddev9->lpVtbl->Present(d3d9Renderer->d3ddev9, NULL, NULL, NULL, NULL);
    pBackBuffer->lpVtbl->Release(d3d9Renderer->d3dsurface9);

    return YERR_SUCCESS;
}


struct VideoRendererActions VideoRendererActionsDirect3D9 = 
{
    .Type = VIDEO_RENDERER_D3D9,
    .Initialize = VideoRendererActionsDirect3D9Initialize,
    .Release = VideoRendererActionsDirect3D9Release,
    .Render = VideoRendererActionsDirect3D9Render
};

