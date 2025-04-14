#ifndef YARA_D3D12
#define YARA_D3D12

#define COBJMACROS
#pragma warning( push )
#pragma warning( disable : 4201 )
#pragma warning( disable : 4115 )
#include <d3d12.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#pragma warning( pop )

void* alloc(size_t size);

typedef struct Mutex
{
    HANDLE mutex;
} Mutex;
Mutex mutex_create();
void mutex_lock(Mutex* mutex);
void mutex_unlock(Mutex* mutex);
void mutex_destroy(Mutex* mutex);

#define ARRAY_COUNT(Array) (sizeof(Array)/sizeof((Array)[0]))
// #define BACKBUFFER_COUNT 2
#define ID3DBlob_QueryInterface(self, riid, ppvObject) ((self)->lpVtbl->QueryInterface(self, riid, ppvObject))
#define ID3DBlob_AddRef(self) ((self)->lpVtbl->AddRef(self))
#define ID3DBlob_Release(self) ((self)->lpVtbl->Release(self))
#define ID3DBlob_GetBufferPointer(self) ((self)->lpVtbl->GetBufferPointer(self))
#define ID3DBlob_GetBufferSize(self) ((self)->lpVtbl->GetBufferSize(self))

struct Command_List_Allocation
{
    ID3D12CommandAllocator* command_allocator;
    ID3D12GraphicsCommandList* command_list;
    unsigned long long fence_value;
    int is_allocated;
};
struct Command_List_Allocator
{
    struct Command_List_Allocation** command_lists;
    size_t command_lists_count;
    size_t command_lists_size;
    size_t command_lists_index;

    struct Device* device;
    struct Fence* fence;
};
struct Device
{
    ID3D12Device* device;
    IDXGIFactory2* factory;
    struct Command_List_Allocator command_list_allocator;
    Mutex mutex;
};
struct Command_Queue
{
    ID3D12CommandQueue* command_queue;
    struct Device* device;
};
struct Swapchain
{
    IDXGISwapChain3* swapchain;
    struct Swapchain_Descriptor swapchain_descriptor;
    struct Fence** fences;
    int frame;
    struct Command_Queue* command_queue;
};
struct Buffer_State
{
    struct Buffer* buffer;
    enum RESOURCE_STATE state;
};
struct Command_List
{
    struct Command_List_Allocation* command_list_allocation;

    struct Buffer_State* buffer_states;
    size_t buffer_states_size;
    size_t buffer_states_count;
    struct Buffer_State* required_buffer_states;
    size_t required_buffer_states_size;
    size_t required_buffer_states_count;
    
    struct Device* device;
    unsigned long long fence_value;
};
struct Descriptor_Set
{
    ID3D12DescriptorHeap* descriptor_heap;
    unsigned int descriptor_count;
    unsigned int descriptor_size;
    D3D12_DESCRIPTOR_HEAP_TYPE descriptor_heap_type;
};
struct Descriptor_Handle
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor_handle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_descriptor_handle;
};
struct Buffer
{
    ID3D12Resource* resource;
    struct Descriptor_Handle handles[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    enum RESOURCE_STATES last_known_state;
};
struct Upload_Buffer
{
    ID3D12Resource* resource;
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
};
struct Shader
{
    ID3D12RootSignature* root_signature;
    ID3DBlob* signature_blob;
    ID3DBlob* vs_code_blob;
    ID3DBlob* ps_code_blob;
};
struct Pipeline_State_Object
{
    ID3D12PipelineState* pipeline_state_object;
};
struct Fence
{
    ID3D12Fence* fence;
    unsigned long long value;
    HANDLE event;
};

static D3D_PRIMITIVE_TOPOLOGY to_d3d12_primitive_topology[_PRIMITIVE_TOPOLOGY_COUNT] = {
    D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
    D3D_PRIMITIVE_TOPOLOGY_LINELIST,
    D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
    D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
    D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
    D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
    D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
    D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ
};
static D3D12_DESCRIPTOR_HEAP_TYPE to_d3d12_descriptor_type[_DESCRIPTOR_TYPE_COUNT] = {
    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
    D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
    D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
    D3D12_DESCRIPTOR_HEAP_TYPE_DSV
};
static enum DESCRIPTOR_TYPE to_yara_descriptor_type[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {
    DESCRIPTOR_TYPE_CBV_SRV_UAV,
    DESCRIPTOR_TYPE_SAMPLER,
    DESCRIPTOR_TYPE_RTV,
    DESCRIPTOR_TYPE_DSV
};
static enum RESOURCE_STATE to_d3d12_resource_state[_RESOURCE_STATE_COUNT] = {
    D3D12_RESOURCE_STATE_COMMON,
    
    D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
    D3D12_RESOURCE_STATE_INDEX_BUFFER,
    D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,

    D3D12_RESOURCE_STATE_RENDER_TARGET,
    D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
    D3D12_RESOURCE_STATE_DEPTH_WRITE,
    D3D12_RESOURCE_STATE_COPY_DEST,

    D3D12_RESOURCE_STATE_PRESENT,
};

struct Command_List_Allocation* device_create_command_list_allocation(struct Device* device);

struct Descriptor_Handle descriptor_set_alloc(struct Descriptor_Set* descriptor_set);

void command_list_append_buffer_state(struct Command_List* command_list, struct Buffer_State buffer_state);
void command_list_append_required_buffer_state(struct Command_List* command_list, struct Buffer_State buffer_state);

struct Command_List_Allocator command_list_allocator_create(struct Device* device);
void command_list_allocator_increment_index(struct Command_List_Allocator* command_list_allocator);
struct Command_List_Allocation* command_list_allocator_alloc(struct Command_List_Allocator* command_list_allocator);

#endif
