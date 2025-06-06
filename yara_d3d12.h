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
#include <guiddef.h>
#include <d3dcompiler.h>

#include <d3d12shader.h>
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

struct Accessed_Object
{
    void* object;
};
#define ACCESSED_OBJECT(object) (struct Accessed_Object){ (void*)object }
unsigned long long accessed_object_get_releasable_object_count(struct Accessed_Object accessed_object);
unsigned long long accessed_object_get_last_fence_value(struct Accessed_Object accessed_object);
void accessed_object_set_last_fence_value(struct Accessed_Object accessed_object, unsigned long long last_fence_value);
IUnknown** accessed_object_get_releasable_object(struct Accessed_Object accessed_object, int index);

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
};
struct Device
{
    ID3D12Device* device;
    IDXGIFactory2* factory;
    struct Command_List_Allocator command_list_allocator;
    Mutex mutex;
    struct Fence* main_fence;
    struct Accessed_Object* destroyed_objects;
    size_t destroyed_objects_size;
    size_t destroyed_objects_count;
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

    struct Accessed_Object* accessed_objects;
    size_t accessed_objects_size;
    size_t accessed_objects_count;
    
    struct Device* device;
};
struct Descriptor_Set
{
    unsigned long long releasable_objects;
    ID3D12DescriptorHeap* descriptor_heap;
    unsigned long long last_used_fence_value;
    unsigned long long ref_count;

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
    unsigned long long releasable_objects;
    ID3D12Resource* resource;
    unsigned long long last_used_fence_value;
    unsigned long long ref_count;

    enum RESOURCE_STATES last_known_state;
    struct Device* device;
    unsigned long long size;
    enum BUFFER_TYPE buffer_type;
};
struct Upload_Buffer
{
    unsigned long long releasable_objects;
    ID3D12Resource* resource;
    unsigned long long last_used_fence_value;
    unsigned long long ref_count;

    struct Device* device;
};
struct Shader_Resource_View
{
    unsigned long long releasable_objects;
    unsigned long long last_used_fence_value;
    unsigned long long ref_count;

    struct Descriptor_Handle handle;
    struct Buffer* buffer;
    struct Device* device;
};
struct Constant_Buffer_View
{
    unsigned long long releasable_objects;
    unsigned long long last_used_fence_value;
    unsigned long long ref_count;

    struct Descriptor_Handle handle;
    struct Buffer* buffer;
    struct Device* device;
};
struct Depth_Stencil_View
{
    unsigned long long releasable_objects;
    unsigned long long last_used_fence_value;
    unsigned long long ref_count;

    struct Descriptor_Handle handle;
    struct Buffer* buffer;
    struct Device* device;
};
struct Render_Target_View
{
    unsigned long long releasable_objects;
    unsigned long long last_used_fence_value;
    unsigned long long ref_count;

    struct Descriptor_Handle handle;
    struct Buffer* buffer;
    struct Device* device;
};
struct Shader
{
    unsigned long long releasable_objects;
    ID3D12RootSignature* root_signature;
    ID3DBlob* signature_blob;
    ID3DBlob* vs_code_blob;
    ID3DBlob* ps_code_blob;
    unsigned long long last_used_fence_value;
    unsigned long long ref_count;
};
struct Pipeline_State_Object
{
    unsigned long long releasable_objects;
    ID3D12PipelineState* pipeline_state_object;
    unsigned long long last_used_fence_value;
    unsigned long long ref_count;
};
struct Fence
{
    unsigned long long releasable_objects;
    ID3D12Fence* fence;
    unsigned long long last_used_fence_value;
    unsigned long long ref_count;

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
static DXGI_FORMAT to_d3d12_format[_FORMAT_COUNT] = {
    DXGI_FORMAT_UNKNOWN,
    DXGI_FORMAT_R32G32B32A32_TYPELESS,
    DXGI_FORMAT_R32G32B32A32_FLOAT,
    DXGI_FORMAT_R32G32B32A32_UINT,
    DXGI_FORMAT_R32G32B32A32_SINT,
    DXGI_FORMAT_R32G32B32_TYPELESS,
    DXGI_FORMAT_R32G32B32_FLOAT,
    DXGI_FORMAT_R32G32B32_UINT,
    DXGI_FORMAT_R32G32B32_SINT,
    DXGI_FORMAT_R32G32_TYPELESS,
    DXGI_FORMAT_R32G32_FLOAT,
    DXGI_FORMAT_R32G32_UINT,
    DXGI_FORMAT_R32G32_SINT,
    DXGI_FORMAT_R32_TYPELESS,
    DXGI_FORMAT_D32_FLOAT,
    DXGI_FORMAT_R32_FLOAT,
    DXGI_FORMAT_R32_UINT,
    DXGI_FORMAT_R32_SINT,

    DXGI_FORMAT_R16G16B16A16_TYPELESS,
    DXGI_FORMAT_R16G16B16A16_FLOAT,
    DXGI_FORMAT_R16G16B16A16_UNORM,
    DXGI_FORMAT_R16G16B16A16_UINT,
    DXGI_FORMAT_R16G16B16A16_SNORM,
    DXGI_FORMAT_R16G16B16A16_SINT,
    DXGI_FORMAT_R16G16_TYPELESS,
    DXGI_FORMAT_R16G16_FLOAT,
    DXGI_FORMAT_R16G16_UNORM,
    DXGI_FORMAT_R16G16_UINT,
    DXGI_FORMAT_R16G16_SNORM,
    DXGI_FORMAT_R16G16_SINT,
    DXGI_FORMAT_R16_TYPELESS,
    DXGI_FORMAT_R16_FLOAT,
    DXGI_FORMAT_D16_UNORM,
    DXGI_FORMAT_R16_UNORM,
    DXGI_FORMAT_R16_UINT,
    DXGI_FORMAT_R16_SNORM,
    DXGI_FORMAT_R16_SINT,

    DXGI_FORMAT_R8G8B8A8_TYPELESS,
    DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
    DXGI_FORMAT_R8G8B8A8_UINT,
    DXGI_FORMAT_R8G8B8A8_SNORM,
    DXGI_FORMAT_R8G8B8A8_SINT,
    DXGI_FORMAT_R8G8_TYPELESS,
    DXGI_FORMAT_R8G8_UNORM,
    DXGI_FORMAT_R8G8_UINT,
    DXGI_FORMAT_R8G8_SNORM,
    DXGI_FORMAT_R8G8_SINT,
    DXGI_FORMAT_R8_TYPELESS,
    DXGI_FORMAT_R8_UNORM,
    DXGI_FORMAT_R8_UINT,
    DXGI_FORMAT_R8_SNORM,
    DXGI_FORMAT_R8_SINT,

    DXGI_FORMAT_R10G10B10A2_TYPELESS,
    DXGI_FORMAT_R10G10B10A2_UNORM,
    DXGI_FORMAT_R10G10B10A2_UINT,
    DXGI_FORMAT_R11G11B10_FLOAT,
    DXGI_FORMAT_R24G8_TYPELESS,
    DXGI_FORMAT_D24_UNORM_S8_UINT,
    DXGI_FORMAT_A8_UNORM,
    DXGI_FORMAT_R1_UNORM,
    DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
    DXGI_FORMAT_R8G8_B8G8_UNORM,
    DXGI_FORMAT_G8R8_G8B8_UNORM,
    DXGI_FORMAT_B5G6R5_UNORM,
    DXGI_FORMAT_B5G5R5A1_UNORM,
    DXGI_FORMAT_B8G8R8A8_UNORM,
    DXGI_FORMAT_B8G8R8X8_UNORM,
    DXGI_FORMAT_B8G8R8A8_TYPELESS,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
    DXGI_FORMAT_B8G8R8X8_TYPELESS,
    DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
    DXGI_FORMAT_B4G4R4A4_UNORM,

    DXGI_FORMAT_BC1_TYPELESS,
    DXGI_FORMAT_BC1_UNORM,
    DXGI_FORMAT_BC1_UNORM_SRGB,
    DXGI_FORMAT_BC2_TYPELESS,
    DXGI_FORMAT_BC2_UNORM,
    DXGI_FORMAT_BC2_UNORM_SRGB,
    DXGI_FORMAT_BC3_TYPELESS,
    DXGI_FORMAT_BC3_UNORM,
    DXGI_FORMAT_BC3_UNORM_SRGB,
    DXGI_FORMAT_BC4_TYPELESS,
    DXGI_FORMAT_BC4_UNORM,
    DXGI_FORMAT_BC4_SNORM,
    DXGI_FORMAT_BC5_TYPELESS,
    DXGI_FORMAT_BC5_UNORM,
    DXGI_FORMAT_BC5_SNORM,
    DXGI_FORMAT_BC6H_TYPELESS,
    DXGI_FORMAT_BC6H_UF16,
    DXGI_FORMAT_BC6H_SF16,
    DXGI_FORMAT_BC7_TYPELESS,
    DXGI_FORMAT_BC7_UNORM,
    DXGI_FORMAT_BC7_UNORM_SRGB,
};
static enum FORMAT to_yara_format[DXGI_FORMAT_B4G4R4A4_UNORM+1] = {
    FORMAT_UNKNOWN,
    FORMAT_R32G32B32A32_TYPELESS,
    FORMAT_R32G32B32A32_FLOAT,
    FORMAT_R32G32B32A32_UINT,
    FORMAT_R32G32B32A32_SINT,
    FORMAT_R32G32B32_TYPELESS,
    FORMAT_R32G32B32_FLOAT,
    FORMAT_R32G32B32_UINT,
    FORMAT_R32G32B32_SINT,
    FORMAT_R16G16B16A16_TYPELESS,
    FORMAT_R16G16B16A16_FLOAT,
    FORMAT_R16G16B16A16_UNORM,
    FORMAT_R16G16B16A16_UINT,
    FORMAT_R16G16B16A16_SNORM,
    FORMAT_R16G16B16A16_SINT,
    FORMAT_R32G32_TYPELESS,
    FORMAT_R32G32_FLOAT,
    FORMAT_R32G32_UINT,
    FORMAT_R32G32_SINT,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_R10G10B10A2_TYPELESS,
    FORMAT_R10G10B10A2_UNORM,
    FORMAT_R10G10B10A2_UINT,
    FORMAT_R11G11B10_FLOAT,
    FORMAT_R8G8B8A8_TYPELESS,
    FORMAT_R8G8B8A8_UNORM,
    FORMAT_R8G8B8A8_UNORM_SRGB,
    FORMAT_R8G8B8A8_UINT,
    FORMAT_R8G8B8A8_SNORM,
    FORMAT_R8G8B8A8_SINT,
    FORMAT_R16G16_TYPELESS,
    FORMAT_R16G16_FLOAT,
    FORMAT_R16G16_UNORM,
    FORMAT_R16G16_UINT,
    FORMAT_R16G16_SNORM,
    FORMAT_R16G16_SINT,
    FORMAT_R32_TYPELESS,
    FORMAT_D32_FLOAT,
    FORMAT_R32_FLOAT,
    FORMAT_R32_UINT,
    FORMAT_R32_SINT,
    FORMAT_R24G8_TYPELESS,
    FORMAT_D24_UNORM_S8_UINT,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_R8G8_TYPELESS,
    FORMAT_R8G8_UNORM,
    FORMAT_R8G8_UINT,
    FORMAT_R8G8_SNORM,
    FORMAT_R8G8_SINT,
    FORMAT_R16_TYPELESS,
    FORMAT_R16_FLOAT,
    FORMAT_D16_UNORM,
    FORMAT_R16_UNORM,
    FORMAT_R16_UINT,
    FORMAT_R16_SNORM,
    FORMAT_R16_SINT,
    FORMAT_R8_TYPELESS,
    FORMAT_R8_UNORM,
    FORMAT_R8_UINT,
    FORMAT_R8_SNORM,
    FORMAT_R8_SINT,
    FORMAT_A8_UNORM,
    FORMAT_R1_UNORM,
    FORMAT_R9G9B9E5_SHAREDEXP,
    FORMAT_R8G8_B8G8_UNORM,
    FORMAT_G8R8_G8B8_UNORM,
    FORMAT_BC1_TYPELESS,
    FORMAT_BC1_UNORM,
    FORMAT_BC1_UNORM_SRGB,
    FORMAT_BC2_TYPELESS,
    FORMAT_BC2_UNORM,
    FORMAT_BC2_UNORM_SRGB,
    FORMAT_BC3_TYPELESS,
    FORMAT_BC3_UNORM,
    FORMAT_BC3_UNORM_SRGB,
    FORMAT_BC4_TYPELESS,
    FORMAT_BC4_UNORM,
    FORMAT_BC4_SNORM,
    FORMAT_BC5_TYPELESS,
    FORMAT_BC5_UNORM,
    FORMAT_BC5_SNORM,
    FORMAT_B5G6R5_UNORM,
    FORMAT_B5G5R5A1_UNORM,
    FORMAT_B8G8R8A8_UNORM,
    FORMAT_B8G8R8X8_UNORM,
    FORMAT_UNKNOWN,
    FORMAT_B8G8R8A8_TYPELESS,
    FORMAT_B8G8R8A8_UNORM_SRGB,
    FORMAT_B8G8R8X8_TYPELESS,
    FORMAT_B8G8R8X8_UNORM_SRGB,
    FORMAT_BC6H_TYPELESS,
    FORMAT_BC6H_UF16,
    FORMAT_BC6H_SF16,
    FORMAT_BC7_TYPELESS,
    FORMAT_BC7_UNORM,
    FORMAT_BC7_UNORM_SRGB,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_UNKNOWN,
    FORMAT_B4G4R4A4_UNORM
};
static D3D12_INPUT_CLASSIFICATION to_d3d12_input_classification[_INPUT_ELEMENT_CLASSIFICATION_COUNT] = {
    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
    D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA
};
static D3D12_BLEND to_d3d12_blend[_BLEND_TYPE_COUNT] = {
    D3D12_BLEND_ZERO,
    D3D12_BLEND_ONE,
    D3D12_BLEND_SRC_COLOR,
    D3D12_BLEND_INV_SRC_COLOR,
    D3D12_BLEND_SRC_ALPHA,
    D3D12_BLEND_INV_SRC_ALPHA,
    D3D12_BLEND_DEST_ALPHA,
    D3D12_BLEND_INV_DEST_ALPHA,
    D3D12_BLEND_DEST_COLOR,
    D3D12_BLEND_INV_DEST_COLOR,
    D3D12_BLEND_SRC_ALPHA_SAT,
    D3D12_BLEND_BLEND_FACTOR,
    D3D12_BLEND_INV_BLEND_FACTOR,
    D3D12_BLEND_SRC1_COLOR,
    D3D12_BLEND_INV_SRC1_COLOR,
    D3D12_BLEND_SRC1_ALPHA,
    D3D12_BLEND_INV_SRC1_ALPHA,
    D3D12_BLEND_ALPHA_FACTOR,
    D3D12_BLEND_INV_ALPHA_FACTOR
};
static D3D12_BLEND_OP to_d3d12_blend_op[_BLEND_OP_COUNT] = {
    D3D12_BLEND_OP_ADD,
    D3D12_BLEND_OP_SUBTRACT,
    D3D12_BLEND_OP_REV_SUBTRACT,
    D3D12_BLEND_OP_MIN,
    D3D12_BLEND_OP_MAX
};
static D3D12_LOGIC_OP to_d3d12_logic_op[_LOGIC_OP_COUNT] = {
    D3D12_LOGIC_OP_CLEAR,
    D3D12_LOGIC_OP_SET,
    D3D12_LOGIC_OP_COPY,
    D3D12_LOGIC_OP_COPY_INVERTED,
    D3D12_LOGIC_OP_NOOP,
    D3D12_LOGIC_OP_INVERT,
    D3D12_LOGIC_OP_AND,
    D3D12_LOGIC_OP_NAND,
    D3D12_LOGIC_OP_OR,
    D3D12_LOGIC_OP_NOR,
    D3D12_LOGIC_OP_XOR,
    D3D12_LOGIC_OP_EQUIV,
    D3D12_LOGIC_OP_AND_REVERSE,
    D3D12_LOGIC_OP_AND_INVERTED,
    D3D12_LOGIC_OP_OR_REVERSE,
    D3D12_LOGIC_OP_OR_INVERTED
};
static D3D12_FILL_MODE to_d3d12_fill_mode[_FILL_MODE_COUNT] = {
    D3D12_FILL_MODE_WIREFRAME,
    D3D12_FILL_MODE_SOLID
};
static D3D12_CULL_MODE to_d3d12_cull_mode[_CULL_MODE_COUNT] = {
    D3D12_CULL_MODE_NONE,
    D3D12_CULL_MODE_FRONT,
    D3D12_CULL_MODE_BACK
};
static D3D12_CONSERVATIVE_RASTERIZATION_MODE to_d3d12_conservative_rasterization_mode[_CONSERVATIVE_RASTERIZATION_MODE_COUNT] = {
    D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF,
    D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON
};
static D3D12_STENCIL_OP to_d3d12_stencil_op[_STENCIL_OP_COUNT] = {
    D3D12_STENCIL_OP_KEEP,
    D3D12_STENCIL_OP_ZERO,
    D3D12_STENCIL_OP_REPLACE,
    D3D12_STENCIL_OP_INCR_SAT,
    D3D12_STENCIL_OP_DECR_SAT,
    D3D12_STENCIL_OP_INVERT,
    D3D12_STENCIL_OP_INCR,
    D3D12_STENCIL_OP_DECR
};
static D3D12_COMPARISON_FUNC to_d3d12_comparison_func[_COMPARISON_FUNC_COUNT] = {
    D3D12_COMPARISON_FUNC_NEVER,
    D3D12_COMPARISON_FUNC_LESS,
    D3D12_COMPARISON_FUNC_EQUAL,
    D3D12_COMPARISON_FUNC_LESS_EQUAL,
    D3D12_COMPARISON_FUNC_GREATER,
    D3D12_COMPARISON_FUNC_NOT_EQUAL,
    D3D12_COMPARISON_FUNC_GREATER_EQUAL,
    D3D12_COMPARISON_FUNC_ALWAYS
};
static D3D12_DEPTH_WRITE_MASK to_d3d12_depth_write_mask[_DEPTH_WRITE_MASK_COUNT] = {
    D3D12_DEPTH_WRITE_MASK_ZERO,
    D3D12_DEPTH_WRITE_MASK_ALL
};
static D3D12_PRIMITIVE_TOPOLOGY_TYPE to_d3d12_primitive_topology_type[_PRIMITIVE_TOPOLOGY_TYPE_COUNT] = {
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH
};
static D3D12_RESOURCE_DIMENSION to_d3d12_resource_dimension[_BUFFER_TYPE_COUNT] = {
    D3D12_RESOURCE_DIMENSION_BUFFER,
    D3D12_RESOURCE_DIMENSION_TEXTURE2D
};

struct Command_List_Allocation* device_create_command_list_allocation(struct Device* device);
void device_release_destroyed_objects(struct Device* device);
void device_append_destroyed_objects(struct Device* device, struct Accessed_Object accessed_object);
void device_pop_destroyed_objects(struct Device* device, size_t pop_count);

struct Descriptor_Handle descriptor_set_alloc(struct Descriptor_Set* descriptor_set);

void command_list_append_buffer_state(struct Command_List* command_list, struct Buffer_State buffer_state);
void command_list_append_required_buffer_state(struct Command_List* command_list, struct Buffer_State buffer_state);
void command_list_append_accessed_object(struct Command_List* command_list, struct Accessed_Object buffer);

struct Command_List_Allocator command_list_allocator_create(struct Device* device);
void command_list_allocator_increment_index(struct Command_List_Allocator* command_list_allocator);
struct Command_List_Allocation* command_list_allocator_alloc(struct Command_List_Allocator* command_list_allocator);

#endif
