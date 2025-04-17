#include <stdio.h>
#include <stdbool.h>
#include "linker.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "yara.h"
#include "yara_d3d12.h"

#include "util.h"
#include "HandmadeMath.h"

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
    SetCpuAndThreadPriority();
    CreateConsole();

    CurrentInstance; PrevInstance; CommandLine; ShowCode;
    printf("Hello World!\n");

    // Windows
    WNDCLASSA WindowClass = {
        .lpfnWndProc = WindowCallback,
        .hInstance = CurrentInstance,
        .hCursor = LoadCursor(0, IDC_ARROW),
        .lpszClassName = "YaraWindowClass"
    };
    RegisterClassA(&WindowClass);
    HWND Window = CreateWindowExA(0, WindowClass.lpszClassName, "Yara", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, CurrentInstance, 0);

    // D3D12
    struct Device* device = 0;
    device_create(&device);

    struct Command_Queue* command_queue = 0;
    device_create_command_queue(device, &command_queue);

    struct Swapchain* swapchain = 0;
    device_create_swapchain(device, command_queue, (struct Swapchain_Descriptor){ .window = Window, .backbuffer_count = 2 }, &swapchain);

    struct Command_List* command_list = 0;
    device_create_command_list(device, &command_list);

    struct Descriptor_Set* rtv_descriptor_set = 0;
    device_create_descriptor_set(device, DESCRIPTOR_TYPE_RTV, 2048, &rtv_descriptor_set);

    struct Descriptor_Set* cbv_srv_uav_descriptor_set = 0;
    device_create_descriptor_set(device, DESCRIPTOR_TYPE_CBV_SRV_UAV, 2048, &cbv_srv_uav_descriptor_set);

    struct Buffer** backbuffers = _alloca(sizeof(struct Buffer*) * swapchain->swapchain_descriptor.backbuffer_count);
    swapchain_create_backbuffers(swapchain, device, rtv_descriptor_set, backbuffers);
    
    struct Shader* shader = 0;
    device_create_shader(device, &shader);

    D3D12_INPUT_ELEMENT_DESC InputElements[] = {
        {"POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };
    InputElements;
    struct Vertex 
    {
        Vec2 pos;
        Vec3 color;
    };
    struct Input_Element_Descriptor input_element_descriptors[2] = {
        {
            .element_binding = 0,
            .format = FORMAT_R32G32_FLOAT,
            .element_classification = INPUT_ELEMENT_CLASSIFICATION_PER_VERTEX,
        },
        {
            .element_binding = 1,
            .format = FORMAT_R32G32B32_FLOAT,
            .element_classification = INPUT_ELEMENT_CLASSIFICATION_PER_VERTEX,
            .offset = offsetof(struct Vertex, color)
        },
    };
    struct Pipeline_State_Object_Descriptor pipeline_state_object_descriptor = {
        .input_element_descriptors = input_element_descriptors,
        .input_element_descriptors_count = 2
    };
    struct Pipeline_State_Object* pipeline_state_object = 0;
    device_create_pipeline_state_object(device, swapchain, shader, pipeline_state_object_descriptor, &pipeline_state_object);

    float vertices[] = {
        // position    color
        0.0f, 0.7f,    1.0f, 0.0f, 0.0f,
        -0.4f, -0.5f,  0.0f, 1.0f, 0.0f,
        0.4f, -0.5f,   0.0f, 0.0f, 1.0f,
    };
    struct Upload_Buffer* upload_buffer = 0;
    device_create_upload_buffer(device, vertices, sizeof(vertices), &upload_buffer);

    struct Buffer* vertex_buffer = 0;
    struct Buffer_Descriptor buffer_description = {
        .width = sizeof(vertices),
        .height = 1,
        .descriptor_sets = {
            cbv_srv_uav_descriptor_set
        },
        .descriptor_sets_count = 1,
        .buffer_type = BUFFER_TYPE_BUFFER,
        .bind_types = {
            BIND_TYPE_SRV
        },
        .bind_types_count = 1
    };
    device_create_buffer(device, buffer_description, &vertex_buffer);

    {
        struct Command_List* upload_command_list = 0;
        device_create_command_list(device, &upload_command_list);

        command_list_reset(upload_command_list);

        command_list_copy_upload_buffer_to_buffer(upload_command_list, upload_buffer, vertex_buffer);

        command_list_close(upload_command_list);

        command_queue_execute(command_queue, &upload_command_list, 1);
    }
    
    unsigned long long frame_counter = 0;
    while (!DoneRunning)
    {
        unsigned long long timestamp1 = GetRdtsc();

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
        
        command_list_set_render_targets(command_list, &backbuffer, 1, 0);
        float clear_color[4] = {0.1f, 0.1f, 0.1f, 1.0f};
        command_list_clear_render_target(command_list, backbuffer, clear_color);
        command_list_set_vertex_buffer(command_list, vertex_buffer, sizeof(vertices), 5 * sizeof(float));
        command_list_set_primitive_topology(command_list, PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        command_list_draw_instanced(command_list, 3, 1, 0, 0);
        
        command_list_set_buffer_state(command_list, backbuffer, RESOURCE_STATE_PRESENT);
        command_list_close(command_list);

        command_queue_execute(command_queue, &command_list, 1);
        
        swapchain_present(swapchain);
        
        // Sleep(2);
        frame_counter++;

        unsigned long long timestamp2 = GetRdtsc();
        printf("ms: %f \r", (timestamp2 - timestamp1) * 1000.0 / GetRdtscFreq());
    }
    
    return 0;
}