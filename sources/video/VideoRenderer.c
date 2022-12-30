#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libY.h"

#include "VideoRenderer.h"

extern VideoRendererActions VideoRendererActionsDirect3D9;
VideoRendererActions *VideoRendererList[] = 
{
    &VideoRendererActionsDirect3D9,
    NULL
};

VideoRenderer *VideoRendererCreate(VideoRendererOptions *options)
{
    size_t length = sizeof(VideoRendererList) / sizeof(VideoRendererActions*);
	for (size_t i = 0; i < length; i++)
	{
		VideoRendererActions *renderActions = VideoRendererList[i];
        if(renderActions->Type != options->Type)
        {
            continue;
        }

        YLOGI("create AVStreamActions success, %d", renderActions->Type);

        VideoRenderer *renderer = (VideoRenderer*)calloc(1, sizeof(VideoRenderer));
        renderer->Options = options;
        renderer->Actions = renderActions;
        return renderer;
	}
    return NULL;
}

int VideoRendererInitialize(VideoRenderer *renderer)
{
    return renderer->Actions->Initialize(renderer);
}

int VideoRendererRelease(VideoRenderer *renderer)
{
    return renderer->Actions->Release(renderer);
}

int VideoRendererRender(VideoRenderer *renderer, char *videoData)
{
    return renderer->Actions->Render(renderer, videoData);
}


