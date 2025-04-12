#include "yara.h"
#include "yara_d3d12.h"

#include <memory.h>
void* alloc(size_t size)
{
    void* allocation = malloc(size);
    memset(allocation, 0, size);
    return allocation;
}

int create_device(struct Device** out_device)
{
    *out_device = alloc(sizeof(struct Device));

    ID3D12Debug* debug;
    D3D12GetDebugInterface(&IID_ID3D12Debug, &debug);
    ID3D12Debug_EnableDebugLayer(debug);
    
    D3D12CreateDevice(0, D3D_FEATURE_LEVEL_12_0, &IID_ID3D12Device, &(*out_device)->device);
    CreateDXGIFactory2(0, &IID_IDXGIFactory2, &(*out_device)->factory);

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
        .BufferCount = BACKBUFFER_COUNT,
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_IGNORE,
        .Flags = 0
    };
    IDXGISwapChain1 *swapchain1 = 0;
    HRESULT result = IDXGIFactory2_CreateSwapChainForHwnd(device->factory, (IUnknown*)command_queue->command_queue, (HWND)swapchain_descriptor.window, &d3d12_swapchain_description, 0, 0, &swapchain1);
    IDXGISwapChain1_QueryInterface(swapchain1, &IID_IDXGISwapChain3, &(*out_swapchain)->swapchain);
    result;

    return 0;
}
int device_create_command_list(struct Device* device, struct Command_List** out_command_list)
{
    *out_command_list = alloc(sizeof(struct Command_List));

    ID3D12Device_CreateCommandAllocator(device->device, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator, &(*out_command_list)->command_allocator);
    ID3D12Device_CreateCommandList(device->device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, (*out_command_list)->command_allocator, 0, &IID_ID3D12CommandList, &(*out_command_list)->command_list);
    ID3D12GraphicsCommandList_Close((*out_command_list)->command_list);

    return 0;
}
int device_create_descriptor_set(struct Device* device, struct Descriptor_Set** out_descriptor_set)
{
    *out_descriptor_set = alloc(sizeof(struct Descriptor_Set));

    D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_description = {
        .NumDescriptors = BACKBUFFER_COUNT,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0
    };
    {
        descriptor_heap_description.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        ID3D12Device_CreateDescriptorHeap(device->device, &descriptor_heap_description, &IID_ID3D12DescriptorHeap, &(*out_descriptor_set)->descriptor_heap_cbv_srv_uav);
        (*out_descriptor_set)->descriptor_size = ID3D12Device_GetDescriptorHandleIncrementSize(device->device, descriptor_heap_description.Type);
    }
    {
        descriptor_heap_description.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        ID3D12Device_CreateDescriptorHeap(device->device, &descriptor_heap_description, &IID_ID3D12DescriptorHeap, &(*out_descriptor_set)->descriptor_heap_sampler);
        (*out_descriptor_set)->descriptor_size = ID3D12Device_GetDescriptorHandleIncrementSize(device->device, descriptor_heap_description.Type);
    }
    {
        descriptor_heap_description.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        ID3D12Device_CreateDescriptorHeap(device->device, &descriptor_heap_description, &IID_ID3D12DescriptorHeap, &(*out_descriptor_set)->descriptor_heap_rtv);
        (*out_descriptor_set)->descriptor_size = ID3D12Device_GetDescriptorHandleIncrementSize(device->device, descriptor_heap_description.Type);
    }
    {
        descriptor_heap_description.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        ID3D12Device_CreateDescriptorHeap(device->device, &descriptor_heap_description, &IID_ID3D12DescriptorHeap, &(*out_descriptor_set)->descriptor_heap_dsv);
        (*out_descriptor_set)->descriptor_size = ID3D12Device_GetDescriptorHandleIncrementSize(device->device, descriptor_heap_description.Type);
    }
        
    return 0;
}
int device_create_buffer(struct Device* device, struct Descriptor_Set* descriptor_set, size_t size, enum BUFFER_TYPE buffer_type, enum BIND_TYPE bind_types[], size_t bind_types_count, struct Buffer** out_buffer)
{
    D3D12_HEAP_PROPERTIES defaultHeapProperties = {
        .Type = D3D12_HEAP_TYPE_DEFAULT
    };
    D3D12_RESOURCE_DESC resource_desc = {
        .Alignment = 0,
        .Width = size,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc.Count = 1,
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE
    };
    switch (buffer_type)
    {
    case BUFFER_TYPE_BUFFER:
	resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        break;
    case BUFFER_TYPE_TEXTRUE2D:
	resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        break;
    }

    int uav = 0; // Only used for the flags check bellow. Not related to the 3 counters bellow
    
    // This is fucking awful why did I do this to my self
    int cbv_srv_uav = 0;
    int dsv = 0;
    int rtv = 0;
    for (size_t i = 0; i < bind_types_count; i++)
    {
        switch (bind_types[i])
        {
        case BIND_TYPE_UAV:
            uav = 1;
        case BIND_TYPE_CBV:
        case BIND_TYPE_SRV:
            cbv_srv_uav = 1;
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
    
    ID3D12Device_CreateCommittedResource(device->device, &defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_COMMON, 0, &IID_ID3D12Resource, &(*out_buffer)->resource);
    
    size_t total_heap_types = 0;
    if (cbv_srv_uav)
        total_heap_types++;
    if (dsv)
        total_heap_types++;
    if (rtv)
        total_heap_types++;
    
    D3D12_DESCRIPTOR_HEAP_TYPE* handle_types = _alloca(sizeof(D3D12_DESCRIPTOR_HEAP_TYPE) * total_heap_types);
    size_t count_heap_types = 0;
    if (cbv_srv_uav)
        handle_types[count_heap_types++] = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    if (dsv)
        handle_types[count_heap_types++] = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    if (rtv)
        handle_types[count_heap_types++] = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    struct Descriptor_Handle* handles = _alloca(sizeof(struct Descriptor_Handle) * total_heap_types);
    descriptor_set_alloc(descriptor_set, handles, handle_types, total_heap_types);
    for (size_t i = 0; i < bind_types_count; i++)
    {
        (*out_buffer)->handles[handle_types[i]] = handles[i];
    }

    // @continue
    // Is this enough?
    // No fucking clue, test this with the vertex buffer
    
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
    ID3D12CommandList** d3d12_command_lists = _alloca(sizeof(ID3D12CommandList*) * command_lists_count);
    for (size_t i = 0; i < command_lists_count; i++)
    {
        d3d12_command_lists[i] = (ID3D12CommandList*)command_lists[i]->command_list;
    }
    
    ID3D12CommandQueue_ExecuteCommandLists(command_queue->command_queue, (UINT)command_lists_count, d3d12_command_lists);
}

void swapchain_destroy(struct Swapchain* swapchain);
int swapchain_create_backbuffers(struct Swapchain* swapchain, struct Device* device, struct Descriptor_Set* descriptor_set, struct Buffer** out_buffers)
{
    memset(out_buffers, 0, sizeof(struct Buffer) * BACKBUFFER_COUNT);

    for (unsigned int i = 0; i < BACKBUFFER_COUNT; i++)
    {
        out_buffers[i] = alloc(sizeof(struct Buffer));
        struct Buffer* buffer = out_buffers[i];

        struct Descriptor_Handle handle = {0};
        D3D12_DESCRIPTOR_HEAP_TYPE heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        descriptor_set_alloc(descriptor_set, &handle, &heap_type, 1);
        buffer->handles[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = handle;

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
    IDXGISwapChain3_Present(swapchain->swapchain, 1, 0);
    return 0;
}

void command_list_destroy(struct Command_List* command_list);
void command_list_reset(struct Command_List* command_list)
{
    ID3D12CommandAllocator_Reset(command_list->command_allocator);
    ID3D12GraphicsCommandList_Reset(command_list->command_list, command_list->command_allocator, 0);
}
void command_list_set_pipeline_state_object(struct Command_List* command_list, struct Pipeline_State_Object* pipeline_state_object)
{
    ID3D12GraphicsCommandList_SetPipelineState(command_list->command_list, pipeline_state_object->pipeline_state_object);
}
void command_list_set_shader(struct Command_List* command_list, struct Shader* shader)
{
    ID3D12GraphicsCommandList_SetGraphicsRootSignature(command_list->command_list, shader->root_signature);
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
    ID3D12GraphicsCommandList_RSSetViewports(command_list->command_list, 1, &d3d12_viewport);
}
void command_list_set_scissor_rect(struct Command_List* command_list, struct Rect scissor_rect)
{
    D3D12_RECT d3d12_scissor_rect = {
        .left = scissor_rect.left,
        .top = scissor_rect.top,
        .right = scissor_rect.right,
        .bottom = scissor_rect.bottom
    };
    ID3D12GraphicsCommandList_RSSetScissorRects(command_list->command_list, 1, &d3d12_scissor_rect);
}
void command_list_set_render_targets(struct Command_List* command_list, struct Buffer* render_targets[], int render_targets_count, struct Buffer* opt_depth_buffer)
{
    D3D12_CPU_DESCRIPTOR_HANDLE* handles = _alloca(sizeof(D3D12_CPU_DESCRIPTOR_HANDLE) * render_targets_count);
    for (int i = 0; i < render_targets_count; i++)
    {
        handles[i] = render_targets[i]->handles[D3D12_DESCRIPTOR_HEAP_TYPE_RTV].cpu_descriptor_handle;
    }
    
    ID3D12GraphicsCommandList_OMSetRenderTargets(command_list->command_list, render_targets_count, (const D3D12_CPU_DESCRIPTOR_HANDLE*)handles, 0, (const D3D12_CPU_DESCRIPTOR_HANDLE*)(opt_depth_buffer ? &opt_depth_buffer->handles[D3D12_DESCRIPTOR_HEAP_TYPE_DSV].cpu_descriptor_handle : 0));
}
void command_list_clear_render_target(struct Command_List* command_list, struct Buffer* render_target, float clear_color[4])
{
    ID3D12GraphicsCommandList_ClearRenderTargetView(command_list->command_list, render_target->handles[D3D12_DESCRIPTOR_HEAP_TYPE_RTV].cpu_descriptor_handle, clear_color, 0, 0);
    // ID3D12GraphicsCommandList_ClearDepthStencilView // Implement this
}
void command_list_set_vertex_buffer(struct Command_List* command_list, struct Buffer* vertex_buffer, size_t size, size_t stride)
{
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {
        .BufferLocation = ID3D12Resource_GetGPUVirtualAddress(vertex_buffer->resource),
        .SizeInBytes = (UINT)size,
        .StrideInBytes = (UINT)stride
    };
    ID3D12GraphicsCommandList_IASetVertexBuffers(command_list->command_list, 0, 1, &vertex_buffer_view);
}
void command_list_set_primitive_topology(struct Command_List* command_list, enum PRIMITIVE_TOPOLOGY primitive_topology)
{
    ID3D12GraphicsCommandList_IASetPrimitiveTopology(command_list->command_list, to_d3d12_primitive_topology[primitive_topology]);
}
void command_list_draw_instanced(struct Command_List* command_list, size_t vertex_count_per_instance, size_t instance_count, size_t start_vertex_location, size_t start_instance_location)
{
    ID3D12GraphicsCommandList_DrawInstanced(command_list->command_list, (UINT)vertex_count_per_instance, (UINT)instance_count, (UINT)start_vertex_location, (UINT)start_instance_location);
}
int command_list_close(struct Command_List* command_list)
{
    ID3D12GraphicsCommandList_Close(command_list->command_list);
    return 0;
}

void descriptor_set_destroy(struct Descriptor_Set* descriptor_set);
void descriptor_set_alloc(struct Descriptor_Set* descriptor_set, struct Descriptor_Handle handles[], D3D12_DESCRIPTOR_HEAP_TYPE types[], size_t handles_count)
{
    for (size_t i = 0; i < handles_count; i++)
    {
        D3D12_DESCRIPTOR_HEAP_TYPE type = types[i];
        ID3D12DescriptorHeap* descriptor_heap = 0;
        switch (type)
        {
            case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
            descriptor_heap = descriptor_set->descriptor_heap_cbv_srv_uav;
            break;
            case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
            descriptor_heap = descriptor_set->descriptor_heap_sampler;
            break;
            case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
            descriptor_heap = descriptor_set->descriptor_heap_rtv;
            break;
            case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
            descriptor_heap = descriptor_set->descriptor_heap_dsv;
            break;
        }
        ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(descriptor_set->descriptor_heap_rtv, &handles[i].cpu_descriptor_handle);
        handles[i].cpu_descriptor_handle.ptr += descriptor_set->descriptor_count * descriptor_set->descriptor_size;
        
        ID3D12DescriptorHeap_GetGPUDescriptorHandleForHeapStart(descriptor_set->descriptor_heap_rtv, &handles[i].gpu_descriptor_handle);
        handles[i].gpu_descriptor_handle.ptr += descriptor_set->descriptor_count * descriptor_set->descriptor_size;    
    }
    descriptor_set->descriptor_count++;
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
void fence_signal(struct Fence* fence, struct Command_Queue* command_queue)
{
    fence->value += 1;
    ID3D12CommandQueue_Signal(command_queue->command_queue, fence->fence, fence->value);
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