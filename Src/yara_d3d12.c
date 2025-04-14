#include "yara.h"
#include "yara_d3d12.h"

#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "d3d12.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment (lib, "dxguid")

#include <memory.h>
void* alloc(size_t size)
{
    void* allocation = malloc(size);
    memset(allocation, 0, size);
    return allocation;
}

int device_create(struct Device** out_device)
{
    *out_device = alloc(sizeof(struct Device));

    ID3D12Debug* debug;
    D3D12GetDebugInterface(&IID_ID3D12Debug, &debug);
    ID3D12Debug_EnableDebugLayer(debug);
    
    D3D12CreateDevice(0, D3D_FEATURE_LEVEL_12_0, &IID_ID3D12Device, &(*out_device)->device);
    CreateDXGIFactory2(0, &IID_IDXGIFactory2, &(*out_device)->factory);

    (*out_device)->command_list_allocator = command_list_allocator_create(*out_device);

    return 0;
}

void device_destroy(struct Device* device)
{
    ID3D12Device_Release(device->device);
    IDXGIFactory2_Release(device->factory);
    free(device);
}
int device_create_command_queue(struct Device* device, struct Command_Queue** out_command_queue)
{
    *out_command_queue = alloc(sizeof(struct Command_Queue));

    D3D12_COMMAND_QUEUE_DESC command_queue_desc = {
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0
    };
    ID3D12Device_CreateCommandQueue(device->device, &command_queue_desc, &IID_ID3D12CommandQueue, &(*out_command_queue)->command_queue);
    (*out_command_queue)->device = device;
    return 0;
}
int device_create_swapchain(struct Device* device, struct Command_Queue* command_queue, struct Swapchain_Descriptor swapchain_descriptor, struct Swapchain** out_swapchain)
{
    *out_swapchain = alloc(sizeof(struct Swapchain));

    DXGI_SWAP_CHAIN_DESC1 d3d12_swapchain_description = {
        .Width = 0, // uses window width
        .Height = 0, // uses window height
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .Stereo = FALSE,
        .SampleDesc.Count = 1, // Single sample per pixel
        .SampleDesc.Quality = 0,
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = (UINT)swapchain_descriptor.backbuffer_count,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_IGNORE,
        .Flags = 0
    };
    IDXGISwapChain1 *swapchain1 = 0;
    IDXGIFactory2_CreateSwapChainForHwnd(device->factory, (IUnknown*)command_queue->command_queue, (HWND)swapchain_descriptor.window, &d3d12_swapchain_description, 0, 0, &swapchain1);
    IDXGISwapChain1_QueryInterface(swapchain1, &IID_IDXGISwapChain3, &(*out_swapchain)->swapchain);
    (*out_swapchain)->swapchain_descriptor = swapchain_descriptor;
    (*out_swapchain)->fences = alloc(sizeof(struct Fence*) * swapchain_descriptor.backbuffer_count);
    for (size_t i = 0; i < swapchain_descriptor.backbuffer_count; i++)
    {
        device_create_fence(device, &(*out_swapchain)->fences[i]);
        fence_signal((*out_swapchain)->fences[i], command_queue);
    }
    (*out_swapchain)->command_queue = command_queue;

    return 0;
}
struct Command_List_Allocation* device_create_command_list_allocation(struct Device* device)
{
    struct Command_List_Allocation* command_list_allocation = alloc(sizeof(struct Command_List_Allocation));
    ID3D12Device_CreateCommandAllocator(device->device, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator, &(command_list_allocation)->command_allocator);
    ID3D12Device_CreateCommandList(device->device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, (command_list_allocation)->command_allocator, 0, &IID_ID3D12CommandList, &(command_list_allocation)->command_list);
    ID3D12GraphicsCommandList_Close((command_list_allocation)->command_list);
    return command_list_allocation;
}
int device_create_command_list(struct Device* device, struct Command_List** out_command_list)
{
    *out_command_list = alloc(sizeof(struct Command_List));

    (*out_command_list)->buffer_states_size = 16;
    (*out_command_list)->buffer_states = alloc(sizeof(struct Buffer_State) * (*out_command_list)->buffer_states_size);
    (*out_command_list)->required_buffer_states_size = 16;
    (*out_command_list)->required_buffer_states = alloc(sizeof(struct Buffer_State) * (*out_command_list)->required_buffer_states_size);
    
    (*out_command_list)->device = device;

    return 0;
}
int device_create_descriptor_set(struct Device* device, enum DESCRIPTOR_TYPE descriptor_type, size_t descriptor_count, struct Descriptor_Set** out_descriptor_set)
{
    *out_descriptor_set = alloc(sizeof(struct Descriptor_Set));

    D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_description = {
        .NumDescriptors = (UINT)descriptor_count,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0,
        .Type = to_d3d12_descriptor_type[descriptor_type]
    };
    ID3D12Device_CreateDescriptorHeap(device->device, &descriptor_heap_description, &IID_ID3D12DescriptorHeap, &(*out_descriptor_set)->descriptor_heap);
    (*out_descriptor_set)->descriptor_size = ID3D12Device_GetDescriptorHandleIncrementSize(device->device, descriptor_heap_description.Type);
    (*out_descriptor_set)->descriptor_heap_type = descriptor_heap_description.Type;
    
    return 0;
}
int device_create_buffer(struct Device* device, struct Buffer_Descriptor buffer_description, struct Buffer** out_buffer)
{
    *out_buffer = alloc(sizeof(struct Buffer));

    D3D12_HEAP_PROPERTIES defaultHeapProperties = {
        .Type = D3D12_HEAP_TYPE_DEFAULT
    };
    D3D12_RESOURCE_DESC resource_desc = {
        .Alignment = 0,
        .Width = (UINT64)buffer_description.width,
        .Height = (UINT)buffer_description.height,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc.Count = 1,
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE
    };
    switch (buffer_description.buffer_type)
    {
    case BUFFER_TYPE_BUFFER:
	resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        break;
    case BUFFER_TYPE_TEXTRUE2D:
	resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        break;
    }

    int uav = 0;
    int dsv = 0;
    int rtv = 0;
    for (size_t i = 0; i < buffer_description.bind_types_count; i++)
    {
        switch (buffer_description.bind_types[i])
        {
        case BIND_TYPE_UAV:
            uav = 1;
            break;
        case BIND_TYPE_DSV:
            dsv = 1;
            break;
        case BIND_TYPE_RTV:
            rtv = 1;
            break;
        }
    }
    if (rtv)
        resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    if (uav)
        resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    if (dsv)
        resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    
    ID3D12Device_CreateCommittedResource(device->device, &defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &resource_desc, to_d3d12_resource_state[RESOURCE_STATE_COPY_DEST], 0, &IID_ID3D12Resource, &(*out_buffer)->resource);
    (*out_buffer)->last_known_state = RESOURCE_STATE_COPY_DEST;
    
    for (size_t i = 0; i < buffer_description.descriptor_sets_count; i++)
    {
        struct Descriptor_Set* descriptor_set = buffer_description.descriptor_sets[i];
        (*out_buffer)->handles[to_yara_descriptor_type[descriptor_set->descriptor_heap_type]] = descriptor_set_alloc(descriptor_set);
    }
    
    return 0;
}
int device_create_upload_buffer(struct Device* device, void* data, size_t data_size, struct Upload_Buffer** out_upload_buffer)
{
    *out_upload_buffer = alloc(sizeof(struct Upload_Buffer));

    D3D12_HEAP_PROPERTIES UploadHeapProperties = {
        .Type = D3D12_HEAP_TYPE_UPLOAD,
        .CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
        .MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN
    };
    D3D12_RESOURCE_DESC ResourceDesc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = 0, 
        .Width = data_size,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc.Count = 1,
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE
    };
    ID3D12Device_CreateCommittedResource(device->device, &UploadHeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, 0, &IID_ID3D12Resource, &(*out_upload_buffer)->resource);
    D3D12_RANGE ReadRange = {0};
    void *MappedAddress = 0;
    ID3D12Resource_Map((*out_upload_buffer)->resource, 0, &ReadRange, &MappedAddress);
    memcpy(MappedAddress, data, data_size);
    ID3D12Resource_Unmap((*out_upload_buffer)->resource, 0, 0);

    return 0;
}
int device_create_shader(struct Device* device, struct Shader** out_shader)
{
    *out_shader = alloc(sizeof(struct Shader));

    D3D12_ROOT_SIGNATURE_DESC RootSignatureDesc = {
        .NumParameters = 0,
        .pParameters = 0,
        .NumStaticSamplers = 0,
        .pStaticSamplers = 0,
        .Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
    };
    D3D12SerializeRootSignature(&RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &(*out_shader)->signature_blob, 0);
    ID3D12Device_CreateRootSignature(device->device, 0, ID3DBlob_GetBufferPointer((*out_shader)->signature_blob),  ID3DBlob_GetBufferSize((*out_shader)->signature_blob), &IID_ID3D12RootSignature, &(*out_shader)->root_signature);
    
    D3DCompileFromFile(L"./shader.hlsl", 0, 0, "VSMain", "vs_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &(*out_shader)->vs_code_blob, 0);
    D3DCompileFromFile(L"./shader.hlsl", 0, 0, "PSMain", "ps_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &(*out_shader)->ps_code_blob, 0);

    return 0;
}
int device_create_pipeline_state_object(struct Device* device, struct Swapchain* swapchain, struct Shader* shader, struct Pipeline_State_Object** out_pipeline_state_object)
{
    *out_pipeline_state_object = alloc(sizeof(struct Pipeline_State_Object));

    D3D12_INPUT_ELEMENT_DESC InputElements[] = {
        {"POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    DXGI_SWAP_CHAIN_DESC1 d3d12_swapchain_description = {0};
    IDXGISwapChain3_GetDesc1(swapchain->swapchain, &d3d12_swapchain_description);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d12_pipeline_state_object_description = {
        .pRootSignature = shader->root_signature,
        .VS = { ID3DBlob_GetBufferPointer(shader->vs_code_blob), ID3DBlob_GetBufferSize(shader->vs_code_blob)},
        .PS = { ID3DBlob_GetBufferPointer(shader->ps_code_blob), ID3DBlob_GetBufferSize(shader->ps_code_blob)},
        .BlendState.AlphaToCoverageEnable = FALSE,
        .BlendState.IndependentBlendEnable = FALSE,
        .SampleMask = UINT_MAX,
        .RasterizerState.FillMode = D3D12_FILL_MODE_SOLID,
        .RasterizerState.CullMode = D3D12_CULL_MODE_NONE,
        .RasterizerState.FrontCounterClockwise = TRUE,
        .DepthStencilState.DepthEnable = FALSE,
        .DepthStencilState.StencilEnable = FALSE,
        .InputLayout.pInputElementDescs = InputElements,
        .InputLayout.NumElements = ARRAY_COUNT(InputElements),
        .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
        .NumRenderTargets = 1,
        .RTVFormats[0] = d3d12_swapchain_description.Format,
        .DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT,
        .SampleDesc.Count = 1, // one sample per pixel
        .Flags = D3D12_PIPELINE_STATE_FLAG_NONE,
    };
    for (int i = 0; i < ARRAY_COUNT(d3d12_pipeline_state_object_description.BlendState.RenderTarget); ++i)
    {
        d3d12_pipeline_state_object_description.BlendState.RenderTarget[i] = (D3D12_RENDER_TARGET_BLEND_DESC){
            .BlendEnable = FALSE,
            .LogicOpEnable = FALSE,
            .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
        };
    }
    ID3D12Device_CreateGraphicsPipelineState(device->device, &d3d12_pipeline_state_object_description, &IID_ID3D12PipelineState, &(*out_pipeline_state_object)->pipeline_state_object);

    return 0;
}
int device_create_fence(struct Device* device, struct Fence** out_fence)
{
    *out_fence = alloc(sizeof(struct Fence));

    ID3D12Device_CreateFence(device->device, (*out_fence)->value, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, &(*out_fence)->fence);
    (*out_fence)->event = CreateEventA(0, 0, FALSE, 0);
    
    return 0;
}

void command_queue_destroy(struct Command_Queue* command_queue);
void command_queue_execute(struct Command_Queue* command_queue, struct Command_List* command_lists[], size_t command_lists_count)
{
    ID3D12CommandList** d3d12_command_lists = _alloca(sizeof(ID3D12CommandList*) * (command_lists_count * 2));

    struct Command_List_Allocation** resource_barrier_command_lists = _alloca(sizeof(struct Command_List_Allocation*) * command_lists_count);
    size_t resource_barrier_command_lists_count = 0;

    size_t command_lists_to_execute = 0;
    for (size_t i = 0; i < command_lists_count; i++)
    {
        struct Command_List* command_list = command_lists[i];

        if(command_list->required_buffer_states_count > 0)
        { // Process requried resource barriers
            struct Command_List_Allocation* resource_barrier_command_list = command_list_allocator_alloc(&command_queue->device->command_list_allocator);
            ID3D12CommandAllocator_Reset(resource_barrier_command_list->command_allocator);
            ID3D12GraphicsCommandList_Reset(resource_barrier_command_list->command_list, resource_barrier_command_list->command_allocator, 0);

            for (size_t j = 0; j < command_list->required_buffer_states_count; j++)
            {
                struct Buffer* buffer = command_list->required_buffer_states[j].buffer;
                enum RESOURCE_STATE to_state = command_list->required_buffer_states[j].state;

                if (buffer->last_known_state == to_state)
                    continue;

                D3D12_RESOURCE_BARRIER resource_barrier = {
                    .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                    .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
                    .Transition.pResource = buffer->resource,
                    .Transition.StateBefore = to_d3d12_resource_state[buffer->last_known_state],
                    .Transition.StateAfter = to_d3d12_resource_state[to_state]
                };
                ID3D12GraphicsCommandList_ResourceBarrier(resource_barrier_command_list->command_list, 1, &resource_barrier);
                buffer->last_known_state = to_state;
            }

            ID3D12GraphicsCommandList_Close(resource_barrier_command_list->command_list);
            d3d12_command_lists[command_lists_to_execute++] = (ID3D12CommandList*)resource_barrier_command_list->command_list;
            resource_barrier_command_lists[resource_barrier_command_lists_count++] = resource_barrier_command_list;
        }

        d3d12_command_lists[command_lists_to_execute++] = (ID3D12CommandList*)command_list->command_list_allocation->command_list;
    }
    
    ID3D12CommandQueue_ExecuteCommandLists(command_queue->command_queue, (UINT)command_lists_to_execute, d3d12_command_lists);

    unsigned long long fence_value = fence_signal(command_queue->device->command_list_allocator.fence, command_queue);
    for (size_t i = 0; i < command_lists_count; i++)
    {
        command_lists[i]->command_list_allocation->fence_value = fence_value;
        command_lists[i]->command_list_allocation->is_allocated = 0;
        command_lists[i]->command_list_allocation = 0;
    }
    for (size_t i = 0; i < resource_barrier_command_lists_count; i++)
    {
        resource_barrier_command_lists[i]->fence_value = fence_value;
        resource_barrier_command_lists[i]->is_allocated = 0;
    }
}

void swapchain_destroy(struct Swapchain* swapchain);
int swapchain_create_backbuffers(struct Swapchain* swapchain, struct Device* device, struct Descriptor_Set* rtv_descriptor_set, struct Buffer** out_buffers)
{
    memset(out_buffers, 0, sizeof(struct Buffer*) * swapchain->swapchain_descriptor.backbuffer_count);

    for (unsigned int i = 0; i < swapchain->swapchain_descriptor.backbuffer_count; i++)
    {
        out_buffers[i] = alloc(sizeof(struct Buffer));
        struct Buffer* buffer = out_buffers[i];

        buffer->handles[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = descriptor_set_alloc(rtv_descriptor_set);
        buffer->last_known_state = RESOURCE_STATE_PRESENT;

        IDXGISwapChain3_GetBuffer(swapchain->swapchain, i, &IID_ID3D12Resource, &buffer->resource);
        ID3D12Device_CreateRenderTargetView(device->device, buffer->resource, 0, buffer->handles[D3D12_DESCRIPTOR_HEAP_TYPE_RTV].cpu_descriptor_handle);
    }

    return 0;
}
int swapchain_get_current_backbuffer_index(struct Swapchain* swapchain)
{
    return IDXGISwapChain3_GetCurrentBackBufferIndex(swapchain->swapchain);
}
int swapchain_present(struct Swapchain* swapchain)
{
    IDXGISwapChain3_Present(swapchain->swapchain, 0, 0); // DXGI_PRESENT_ALLOW_TEARING

    int next_frame_index = (swapchain->frame + 1) % swapchain->swapchain_descriptor.backbuffer_count;
    fence_signal(swapchain->fences[swapchain->frame], swapchain->command_queue);
    fence_wait_for_completion(swapchain->fences[next_frame_index]);
    swapchain->frame = next_frame_index;

    return 0;
}

void command_list_destroy(struct Command_List* command_list);
void command_list_reset(struct Command_List* command_list)
{
    command_list->command_list_allocation = command_list_allocator_alloc(&command_list->device->command_list_allocator);
    ID3D12CommandAllocator_Reset(command_list->command_list_allocation->command_allocator);
    ID3D12GraphicsCommandList_Reset(command_list->command_list_allocation->command_list, command_list->command_list_allocation->command_allocator, 0);
}
void command_list_set_pipeline_state_object(struct Command_List* command_list, struct Pipeline_State_Object* pipeline_state_object)
{
    ID3D12GraphicsCommandList_SetPipelineState(command_list->command_list_allocation->command_list, pipeline_state_object->pipeline_state_object);
}
void command_list_set_shader(struct Command_List* command_list, struct Shader* shader)
{
    ID3D12GraphicsCommandList_SetGraphicsRootSignature(command_list->command_list_allocation->command_list, shader->root_signature);
}
void command_list_set_viewport(struct Command_List* command_list, struct Viewport viewport)
{
    D3D12_VIEWPORT d3d12_viewport = {
        .TopLeftX = viewport.x,
        .TopLeftY = viewport.y,
        .Width = viewport.width,
        .Height = viewport.height,
        .MinDepth = viewport.min_depth,
        .MaxDepth = viewport.max_depth
    };
    ID3D12GraphicsCommandList_RSSetViewports(command_list->command_list_allocation->command_list, 1, &d3d12_viewport);
}
void command_list_set_scissor_rect(struct Command_List* command_list, struct Rect scissor_rect)
{
    D3D12_RECT d3d12_scissor_rect = {
        .left = scissor_rect.left,
        .top = scissor_rect.top,
        .right = scissor_rect.right,
        .bottom = scissor_rect.bottom
    };
    ID3D12GraphicsCommandList_RSSetScissorRects(command_list->command_list_allocation->command_list, 1, &d3d12_scissor_rect);
}
void command_list_set_render_targets(struct Command_List* command_list, struct Buffer* render_targets[], int render_targets_count, struct Buffer* opt_depth_buffer)
{
    D3D12_CPU_DESCRIPTOR_HANDLE* handles = _alloca(sizeof(D3D12_CPU_DESCRIPTOR_HANDLE) * render_targets_count);
    for (int i = 0; i < render_targets_count; i++)
    {
        command_list_set_buffer_state(command_list, render_targets[i], RESOURCE_STATE_RENDER_TARGET);
        handles[i] = render_targets[i]->handles[D3D12_DESCRIPTOR_HEAP_TYPE_RTV].cpu_descriptor_handle;
    }
    
    ID3D12GraphicsCommandList_OMSetRenderTargets(command_list->command_list_allocation->command_list, render_targets_count, (const D3D12_CPU_DESCRIPTOR_HANDLE*)handles, 0, (const D3D12_CPU_DESCRIPTOR_HANDLE*)(opt_depth_buffer ? &opt_depth_buffer->handles[D3D12_DESCRIPTOR_HEAP_TYPE_DSV].cpu_descriptor_handle : 0));
}
void command_list_clear_render_target(struct Command_List* command_list, struct Buffer* render_target, float clear_color[4])
{
    command_list_set_buffer_state(command_list, render_target, RESOURCE_STATE_RENDER_TARGET);
    ID3D12GraphicsCommandList_ClearRenderTargetView(command_list->command_list_allocation->command_list, render_target->handles[D3D12_DESCRIPTOR_HEAP_TYPE_RTV].cpu_descriptor_handle, clear_color, 0, 0);
}
void command_list_set_vertex_buffer(struct Command_List* command_list, struct Buffer* vertex_buffer, size_t size, size_t stride)
{
    command_list_set_buffer_state(command_list, vertex_buffer, RESOURCE_STATE_VERTEX_BUFFER);
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {
        .BufferLocation = ID3D12Resource_GetGPUVirtualAddress(vertex_buffer->resource),
        .SizeInBytes = (UINT)size,
        .StrideInBytes = (UINT)stride
    };
    ID3D12GraphicsCommandList_IASetVertexBuffers(command_list->command_list_allocation->command_list, 0, 1, &vertex_buffer_view);
}
void command_list_set_primitive_topology(struct Command_List* command_list, enum PRIMITIVE_TOPOLOGY primitive_topology)
{
    ID3D12GraphicsCommandList_IASetPrimitiveTopology(command_list->command_list_allocation->command_list, to_d3d12_primitive_topology[primitive_topology]);
}
void command_list_draw_instanced(struct Command_List* command_list, size_t vertex_count_per_instance, size_t instance_count, size_t start_vertex_location, size_t start_instance_location)
{
    ID3D12GraphicsCommandList_DrawInstanced(command_list->command_list_allocation->command_list, (UINT)vertex_count_per_instance, (UINT)instance_count, (UINT)start_vertex_location, (UINT)start_instance_location);
}
void command_list_copy_upload_buffer_to_buffer(struct Command_List* command_list, struct Upload_Buffer* src, struct Buffer* dst)
{
    command_list_set_buffer_state(command_list, dst, RESOURCE_STATE_COPY_DEST);
    ID3D12GraphicsCommandList_CopyResource(command_list->command_list_allocation->command_list, dst->resource, src->resource);
}
void command_list_set_buffer_state(struct Command_List* command_list, struct Buffer* buffer, enum RESOURCE_STATE to_state)
{
    int buffer_state_index = -1;
    for (int i = 0; i < command_list->buffer_states_count; i++)
    {
        if (command_list->buffer_states[i].buffer == buffer)
        {
            buffer_state_index = i;
            break;
        }
    }
    if (buffer_state_index == -1)
    { // Handle buffer not being registered.
        command_list_append_required_buffer_state(command_list, (struct Buffer_State){ buffer, to_state });
        command_list_append_buffer_state(command_list, (struct Buffer_State){ buffer, to_state });
        return;
    }
    
    if (command_list->buffer_states[buffer_state_index].state == to_state)
        return;
    
    D3D12_RESOURCE_BARRIER resource_barriers[] = {
        {
            .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            .Transition.pResource = buffer->resource,
            .Transition.StateBefore = to_d3d12_resource_state[command_list->buffer_states[buffer_state_index].state],
            .Transition.StateAfter = to_d3d12_resource_state[to_state]
        }
    };
    ID3D12GraphicsCommandList_ResourceBarrier(command_list->command_list_allocation->command_list, ARRAY_COUNT(resource_barriers), resource_barriers);
    command_list->buffer_states[buffer_state_index].state = to_state;
}
int command_list_close(struct Command_List* command_list)
{
    ID3D12GraphicsCommandList_Close(command_list->command_list_allocation->command_list);
    return 0;
}
void command_list_append_buffer_state(struct Command_List* command_list, struct Buffer_State buffer_state)
{
    if(command_list->buffer_states_count == command_list->buffer_states_size)
    {
        struct Buffer_State* new_buffer = alloc(sizeof(struct Buffer_State) * command_list->buffer_states_size * 2);
        memcpy(new_buffer, command_list->buffer_states, sizeof(struct Buffer_State) * command_list->buffer_states_size);
        command_list->buffer_states = new_buffer;
        command_list->buffer_states_size *= 2;
    }

    command_list->buffer_states[command_list->buffer_states_count++] = buffer_state;
}
void command_list_append_required_buffer_state(struct Command_List* command_list, struct Buffer_State buffer_state)
{
    if(command_list->required_buffer_states_count == command_list->required_buffer_states_size)
    {
        struct Buffer_State* new_buffer = alloc(sizeof(struct Buffer_State) * command_list->required_buffer_states_size * 2);
        memcpy(new_buffer, command_list->required_buffer_states, sizeof(struct Buffer_State) * command_list->required_buffer_states_size);
        command_list->required_buffer_states = new_buffer;
        command_list->required_buffer_states_size *= 2;
    }

    command_list->required_buffer_states[command_list->required_buffer_states_count++] = buffer_state;
}

void descriptor_set_destroy(struct Descriptor_Set* descriptor_set);
struct Descriptor_Handle descriptor_set_alloc(struct Descriptor_Set* descriptor_set)
{
    struct Descriptor_Handle handle = {0};
    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(descriptor_set->descriptor_heap, &handle.cpu_descriptor_handle);
    handle.cpu_descriptor_handle.ptr += descriptor_set->descriptor_count * descriptor_set->descriptor_size;
    
    ID3D12DescriptorHeap_GetGPUDescriptorHandleForHeapStart(descriptor_set->descriptor_heap, &handle.gpu_descriptor_handle);
    handle.gpu_descriptor_handle.ptr += descriptor_set->descriptor_count * descriptor_set->descriptor_size;  
    descriptor_set->descriptor_count++;
    return handle;
}

void buffer_destroy(struct Buffer* buffer);
struct Buffer_Descriptor buffer_get_descriptor(struct Buffer* buffer)
{
    D3D12_RESOURCE_DESC resource_description = {0};
    ID3D12Resource_GetDesc(buffer->resource, &resource_description);

    return (struct Buffer_Descriptor){
        .width = (long long)resource_description.Width,
        .height = (long long)resource_description.Height
    };
}

void upload_buffer_destroy(struct Upload_Buffer* upload_buffer);

void pipeline_state_object_destroy(struct Pipeline_State_Object* pipeline_state_object);

void fence_destroy(struct Fence* fence);
unsigned long long fence_signal(struct Fence* fence, struct Command_Queue* command_queue)
{
    fence->value += 1;
    ID3D12CommandQueue_Signal(command_queue->command_queue, fence->fence, fence->value);
    return fence->value;
}
void fence_wait_for_completion(struct Fence* fence)
{
    if (fence_is_completed(fence) == 0)
    {
        ID3D12Fence_SetEventOnCompletion(fence->fence, fence->value, fence->event);
        WaitForSingleObject(fence->event, INFINITE);
    }
}
int fence_is_completed(struct Fence* fence)
{
    if (ID3D12Fence_GetCompletedValue(fence->fence) >= fence->value)
    {
        return 1;
    }

    return 0;
}
unsigned long long fence_get_completed_value(struct Fence* fence)
{
    return ID3D12Fence_GetCompletedValue(fence->fence);
}

struct Command_List_Allocator command_list_allocator_create(struct Device* device)
{
    struct Command_List_Allocator command_list_allocator = {
        .command_lists = alloc(sizeof(struct Command_List*) * 4),
        .command_lists_size = 4,
        .device = device
    };
    device_create_fence(device, &command_list_allocator.fence);
    return command_list_allocator;
}
void command_list_allocator_increment_index(struct Command_List_Allocator* command_list_allocator)
{
    if (command_list_allocator->command_lists_count > 0)
        command_list_allocator->command_lists_index = (command_list_allocator->command_lists_index + 1) % command_list_allocator->command_lists_count;
}
struct Command_List_Allocation* command_list_allocator_alloc(struct Command_List_Allocator* command_list_allocator)
{
    if (!command_list_allocator->command_lists[command_list_allocator->command_lists_index])
    { // Handle current CL not existing.
        command_list_allocator->command_lists[command_list_allocator->command_lists_index] = device_create_command_list_allocation(command_list_allocator->device);
        struct Command_List_Allocation* command_list = command_list_allocator->command_lists[command_list_allocator->command_lists_index];
        command_list_allocator_increment_index(command_list_allocator);
        command_list_allocator->command_lists_count++;
        command_list->is_allocated = 1;
        return command_list;
    }
    else
    { // Handle current CL existing.
        struct Command_List_Allocation* command_list = command_list_allocator->command_lists[command_list_allocator->command_lists_index];
        if (fence_get_completed_value(command_list_allocator->fence) >= command_list->fence_value && command_list->is_allocated == 0)
        { // Handle CL being available.
            command_list->is_allocated = 1;
            command_list_allocator_increment_index(command_list_allocator);
            return command_list;
        }
        else
        { // Handle CL not being available.
            command_list_allocator_increment_index(command_list_allocator);

            if ((command_list_allocator->command_lists_count) == command_list_allocator->command_lists_size)
            { // Handle growing the command_lists buffer.
                struct Command_List_Allocation** new_buffer = alloc(sizeof(struct Command_List_Allocation*) * command_list_allocator->command_lists_size * 2);
                memcpy(new_buffer, command_list_allocator->command_lists, sizeof(struct Command_List_Allocation*) * command_list_allocator->command_lists_size);
                command_list_allocator->command_lists_size *= 2;
                free(command_list_allocator->command_lists);
                command_list_allocator->command_lists = new_buffer;
            }
            
            if (command_list_allocator->command_lists[command_list_allocator->command_lists_index])
            {
                memmove(command_list_allocator->command_lists + command_list_allocator->command_lists_index + 1, command_list_allocator->command_lists + command_list_allocator->command_lists_index, sizeof(struct Command_List_Allocation*) * (command_list_allocator->command_lists_count - command_list_allocator->command_lists_index));
                command_list_allocator->command_lists[command_list_allocator->command_lists_index] = 0;
            }

            return command_list_allocator_alloc(command_list_allocator);
        }
    }
}