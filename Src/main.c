#include <stdio.h>
#include <stdbool.h>
#include "linker.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "yara.h"
#include "yara_d3d12.h"

static bool DoneRunning;

LRESULT CALLBACK WindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
        case WM_CLOSE:
        case WM_QUIT:
        {
            DoneRunning = true;
        } break;
        
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    return Result;
}

int CALLBACK WinMain(HINSTANCE CurrentInstance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{
    CurrentInstance; PrevInstance; CommandLine; ShowCode;
    printf("Hello World!\n");

    // Windows
    WNDCLASSA WindowClass = {
        .lpfnWndProc = WindowCallback,
        .hInstance = CurrentInstance,
        .hCursor = LoadCursor(0, IDC_ARROW),
        .lpszClassName = "MinimalD3DWindowClass"
    };
    RegisterClassA(&WindowClass);
    HWND Window = CreateWindowExA(0, WindowClass.lpszClassName, "Minimal D3D12", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, CurrentInstance, 0);

    // D3D12
    struct Device* device = 0;
    create_device(&device);

    struct Command_Queue* command_queue = 0;
    device_create_command_queue(device, &command_queue);

    struct Swapchain* swapchain = 0;
    device_create_swapchain(device, command_queue, (struct Swapchain_Descriptor){ Window }, &swapchain);

    struct Command_List* command_list = 0;
    device_create_command_list(device, &command_list);

    struct Descriptor_Set* descriptor_set = 0;
    device_create_descriptor_set(device, &descriptor_set);

    struct Buffer* backbuffers[BACKBUFFER_COUNT] = {0};
    swapchain_create_backbuffers(swapchain, device, descriptor_set, backbuffers);
    
    struct Shader* shader = 0;
    device_create_shader(device, &shader);

    struct Pipeline_State_Object* pipeline_state_object = 0;
    device_create_pipeline_state_object(device, swapchain, shader, &pipeline_state_object);

    float vertices[] = {
        // position    color
        0.0f, 0.7f,    1.0f, 0.0f, 0.0f,
        -0.4f, -0.5f,  0.0f, 1.0f, 0.0f,
        0.4f, -0.5f,   0.0f, 0.0f, 1.0f,
    };
    struct Upload_Buffer* upload_buffer = 0;
    device_create_upload_buffer(device, vertices, sizeof(vertices), &upload_buffer);

    struct Fence* fence = 0;
    device_create_fence(device, &fence);
    
    while (!DoneRunning)
    {
        MSG Message;
        if (PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        int backbuffer_index = swapchain_get_current_backbuffer_index(swapchain);
        
        command_list_reset(command_list);
        
        struct Buffer* backbuffer = backbuffers[backbuffer_index];
        struct Buffer_Descriptor backbuffer_description = buffer_get_descriptor(backbuffer);

        struct Viewport viewport = {
            .width = (float)backbuffer_description.width,
            .height = (float)backbuffer_description.height,
            .min_depth = 0.0f,
            .max_depth = 1.0
        };

        struct Rect scissor_rect = {
            .right = (long)backbuffer_description.width,
            .bottom = (long)backbuffer_description.height,
        };
        
        command_list_set_pipeline_state_object(command_list, pipeline_state_object);
        command_list_set_shader(command_list, shader);
        command_list_set_viewport(command_list, viewport);
        command_list_set_scissor_rect(command_list, scissor_rect);
        
        
        D3D12_RESOURCE_BARRIER RenderBarrier = {
            .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition.pResource = backbuffer->resource,
            .Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT,
            .Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET
        };
        ID3D12GraphicsCommandList_ResourceBarrier(command_list->command_list, 1, &RenderBarrier);
        
        command_list_set_render_targets(command_list, &backbuffer, 1, 0);
        float clear_color[4] = {0.1f, 0.1f, 0.1f, 1.0f};
        command_list_clear_render_target(command_list, backbuffer, clear_color);
        struct Buffer shit = {0};
        shit.resource = upload_buffer->resource;
        command_list_set_vertex_buffer(command_list, &shit, sizeof(vertices), 5 * sizeof(float));
        command_list_set_primitive_topology(command_list, PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        command_list_draw_instanced(command_list, 3, 1, 0, 0);
        
        D3D12_RESOURCE_BARRIER PresentBarrier = {
            .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition.pResource = backbuffer->resource,
            .Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET,
            .Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT
        };
        ID3D12GraphicsCommandList_ResourceBarrier(command_list->command_list, 1, &PresentBarrier);
        command_list_close(command_list);

        command_queue_execute(command_queue, &command_list, 1);
        
        swapchain_present(swapchain);
        //NOTE(chen): waiting until all the commands are executed,
        //            effectively doing single buffering. It's inefficient
        //            but simple and easy to understand.
        fence_signal(fence, command_queue);
        fence_wait_for_completion(fence);
        
        Sleep(2);
    }
    
    return 0;
}