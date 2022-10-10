/***********************************************************************************
 * @ file    : vod_d3d9.h
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

#include "vod.h"

#pragma comment(lib, "d3d9.lib")

typedef struct d3d9_rnederer_s
{
    IDirect3D9 *d3d9;
    IDirect3DDevice9 *d3ddev9;
    IDirect3DSurface9 *d3dsurface9;
}d3d9_renderer;


static D3DFORMAT VIDEO_FORMAT2D3DFORMAT(VIDEO_FORMAT format)
{
    switch (format)
    {
    case VIDEO_FORMAT_YV12: return (D3DFORMAT)MAKEFOURCC('Y','V','1','2');
    default: return -1;
    }
}

int d3d_renderer_init(video_renderer *renderer, video_renderer_options *options, void **renderer_ctx)
{
    IDirect3D9 *d3d9 = NULL;
    IDirect3DDevice9 *d3ddev9 = NULL;
    IDirect3DSurface9 *d3dsurface9 = NULL;

    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    if(d3d9 == NULL)
    {
        YLOGE(YTEXT("Durect3DCreate9 failed, %d"), GetLastError());
        return YERR_FAILED;
    }

    D3DPRESENT_PARAMETERS d3dpp;
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
	d3dpp.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
	d3dpp.hDeviceWindow = (HWND)options->drawable;
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	HRESULT r = d3d9->lpVtbl->CreateDevice(d3d9, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)options->drawable, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &d3ddev9);
	if(FAILED(r))
    {
        YLOGE(YTEXT("IDirect3D9->CreateDevice failed, %d"), r);
        return YERR_FAILED;
    }

    D3DFORMAT d3dfmt = VIDEO_FORMAT2D3DFORMAT(options->format);
	r = d3ddev9->lpVtbl->CreateOffscreenPlainSurface(d3ddev9, (UINT)options->video_width, (UINT)options->video_height, d3dfmt, D3DPOOL_DEFAULT, &d3dsurface9, NULL);
	if(FAILED(r))
    {
        YLOGE(YTEXT("IDirect3DDevice9->CreateOffscreenPlainSurface failed, %d"), r);
        return YERR_FAILED;
    }

    d3d9_renderer *ctx = (d3d9_renderer*)Ycalloc(1, sizeof(d3d9_renderer));
    ctx->d3d9 = d3d9;
    ctx->d3ddev9 = d3ddev9;
    ctx->d3dsurface9 = d3dsurface9;
    *renderer_ctx = ctx;

    return YERR_SUCCESS;
}

void d3d_renderer_release(video_renderer *renderer)
{
    d3d9_renderer *ctx = (d3d9_renderer*)renderer->renderer_ctx;
    ctx->d3dsurface9->lpVtbl->Release(ctx->d3dsurface9);
    ctx->d3ddev9->lpVtbl->Release(ctx->d3ddev9);
    ctx->d3d9->lpVtbl->Release(ctx->d3d9);
    Yfree(ctx);
}

int d3d_renderer_render(video_renderer *renderer, char *video_data)
{
    d3d9_renderer *ctx = (d3d9_renderer*)renderer->renderer_ctx;

    D3DLOCKED_RECT lockedrect;
    HRESULT hr = ctx->d3dsurface9->lpVtbl->LockRect(ctx->d3dsurface9, &lockedrect, NULL, D3DLOCK_DISCARD);
    if(FAILED(hr))
    {
        YLOGE(YTEXT("d3dsurface9->LockRect failed, %d"), hr);
        return YERR_FAILED;
    }

    int video_height = renderer->options->video_height;
    int video_width = renderer->options->video_width;

    switch (renderer->options->format)
    {
        case VIDEO_FORMAT_YV12:
        {			
            int stride = lockedrect.Pitch;
			byte* PSrc = (byte*)video_data;
			byte* PSrcY = (LPBYTE)PSrc;
			byte* PSrcU = (LPBYTE)PSrc + renderer->options->video_width * renderer->options->video_height * 5 / 4;
			byte* PSrcV = (LPBYTE)PSrc + renderer->options->video_width * renderer->options->video_height;
			byte* pDest = (BYTE*)lockedrect.pBits;
			byte* PDesY = (LPBYTE)pDest;
			byte* PDesU = (LPBYTE)pDest + stride * renderer->options->video_height * 5 /  4;
			byte* PDesV = (LPBYTE)pDest + stride * renderer->options->video_height;
			for(unsigned int i = 0; i < video_height; i++)
			{
				memcpy(pDest+i*stride, PSrc+i * video_width, video_width);
			}
			for(unsigned int i = 0; i < video_height / 2; i++)
			{
				memcpy(PDesU+i*stride/2, PSrcU + i * video_width / 2, video_width / 2);
			}
			for(unsigned int i = 0; i < video_height / 2; i++)
			{
				memcpy(PDesV+i*stride/2, PSrcV + i * video_width / 2, video_width / 2);
			}
            break;
        }
    
        default: return YERR_VIDEO_FORMAT_NOT_SUPPORTED;
    }

    if((FAILED(hr = ctx->d3dsurface9->lpVtbl->UnlockRect(ctx->d3dsurface9))))
    {
        YLOGE(YTEXT("d3dsurface9->UnlockRect failed, %d"), hr);
        return YERR_FAILED;
    }

    IDirect3DSurface9 *pBackBuffer = NULL;
    ctx->d3ddev9->lpVtbl->Clear(ctx->d3ddev9, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XYUV(0, 0, 0), 1.0f, 0);
    ctx->d3ddev9->lpVtbl->BeginScene(ctx->d3ddev9);
    ctx->d3ddev9->lpVtbl->GetBackBuffer(ctx->d3ddev9, 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    ctx->d3ddev9->lpVtbl->StretchRect(ctx->d3ddev9, ctx->d3dsurface9, NULL, pBackBuffer, NULL, D3DTEXF_LINEAR);
    ctx->d3ddev9->lpVtbl->EndScene(ctx->d3ddev9);
    ctx->d3ddev9->lpVtbl->Present(ctx->d3ddev9, NULL, NULL, NULL, NULL);
    pBackBuffer->lpVtbl->Release(ctx->d3dsurface9);

    return YERR_SUCCESS;
}


struct video_renderer_actions_s d3d_video_renderer = 
{
    .init = d3d_renderer_init,
    .release = d3d_renderer_release,
    .render = d3d_renderer_render
};

