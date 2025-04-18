#ifndef YARA
#define YARA

enum PRIMITIVE_TOPOLOGY
{
    PRIMITIVE_TOPOLOGY_POINTLIST,
    PRIMITIVE_TOPOLOGY_LINELIST,
    PRIMITIVE_TOPOLOGY_LINESTRIP,
    PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
    PRIMITIVE_TOPOLOGY_LINELIST_ADJ,
    PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ,
    PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ,
    PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ,
    _PRIMITIVE_TOPOLOGY_COUNT
};
enum DESCRIPTOR_TYPE
{
    DESCRIPTOR_TYPE_CBV_SRV_UAV,
    DESCRIPTOR_TYPE_SAMPLER,
    DESCRIPTOR_TYPE_RTV,
    DESCRIPTOR_TYPE_DSV,
    _DESCRIPTOR_TYPE_COUNT
};
enum BIND_TYPE
{
    BIND_TYPE_SRV,
    BIND_TYPE_UAV,
    BIND_TYPE_CBV,
    BIND_TYPE_DSV,
    BIND_TYPE_RTV,
    _BIND_TYPE_COUNT
};
enum BUFFER_TYPE
{
    BUFFER_TYPE_BUFFER,
    BUFFER_TYPE_TEXTRUE2D
};
enum RESOURCE_STATE
{
    RESOURCE_STATE_UNKNOWN,
    // READ
    RESOURCE_STATE_VERTEX_BUFFER,
    RESOURCE_STATE_INDEX_BUFFER,
    RESOURCE_STATE_CONSTANT_BUFFER,
    RESOURCE_STATE_STRUCTURED_BUFFER,
    // WRITE
    RESOURCE_STATE_RENDER_TARGET,
    RESOURCE_STATE_UNORDERED_ACCESS,
    RESOURCE_STATE_DEPTH,
    RESOURCE_STATE_COPY_DEST,
    // SPECIAL
    RESOURCE_STATE_PRESENT,
    _RESOURCE_STATE_COUNT
};

struct Device;
struct Command_Queue;
struct Swapchain;
struct Command_List;
struct Descriptor_Set;
struct Buffer;
struct Upload_Buffer;
struct Shader;
struct Pipeline_State_Object;
struct Fence;

struct Swapchain_Descriptor
{
    void* window;
    size_t backbuffer_count;
};
struct Buffer_Descriptor
{
    unsigned long long width;
    unsigned long long height;
    struct Descriptor_Set* descriptor_sets[_DESCRIPTOR_TYPE_COUNT];
    size_t descriptor_sets_count;
    enum BUFFER_TYPE buffer_type;
    enum BIND_TYPE bind_types[_BIND_TYPE_COUNT];
    size_t bind_types_count;
};
enum FORMAT
{
    FORMAT_UNKNOWN,
    FORMAT_R32G32B32A32_TYPELESS,
    FORMAT_R32G32B32A32_FLOAT,
    FORMAT_R32G32B32A32_UINT,
    FORMAT_R32G32B32A32_SINT,
    FORMAT_R32G32B32_TYPELESS,
    FORMAT_R32G32B32_FLOAT,
    FORMAT_R32G32B32_UINT,
    FORMAT_R32G32B32_SINT,
    FORMAT_R32G32_TYPELESS,
    FORMAT_R32G32_FLOAT,
    FORMAT_R32G32_UINT,
    FORMAT_R32G32_SINT,
    FORMAT_R32_TYPELESS,
    FORMAT_D32_FLOAT,
    FORMAT_R32_FLOAT,
    FORMAT_R32_UINT,
    FORMAT_R32_SINT,

    FORMAT_R16G16B16A16_TYPELESS,
    FORMAT_R16G16B16A16_FLOAT,
    FORMAT_R16G16B16A16_UNORM,
    FORMAT_R16G16B16A16_UINT,
    FORMAT_R16G16B16A16_SNORM,
    FORMAT_R16G16B16A16_SINT,
    FORMAT_R16G16_TYPELESS,
    FORMAT_R16G16_FLOAT,
    FORMAT_R16G16_UNORM,
    FORMAT_R16G16_UINT,
    FORMAT_R16G16_SNORM,
    FORMAT_R16G16_SINT,
    FORMAT_R16_TYPELESS,
    FORMAT_R16_FLOAT,
    FORMAT_D16_UNORM,
    FORMAT_R16_UNORM,
    FORMAT_R16_UINT,
    FORMAT_R16_SNORM,
    FORMAT_R16_SINT,

    FORMAT_R8G8B8A8_TYPELESS,
    FORMAT_R8G8B8A8_UNORM,
    FORMAT_R8G8B8A8_UNORM_SRGB,
    FORMAT_R8G8B8A8_UINT,
    FORMAT_R8G8B8A8_SNORM,
    FORMAT_R8G8B8A8_SINT,
    FORMAT_R8G8_TYPELESS,
    FORMAT_R8G8_UNORM,
    FORMAT_R8G8_UINT,
    FORMAT_R8G8_SNORM,
    FORMAT_R8G8_SINT,
    FORMAT_R8_TYPELESS,
    FORMAT_R8_UNORM,
    FORMAT_R8_UINT,
    FORMAT_R8_SNORM,
    FORMAT_R8_SINT,

    FORMAT_R10G10B10A2_TYPELESS,
    FORMAT_R10G10B10A2_UNORM,
    FORMAT_R10G10B10A2_UINT,
    FORMAT_R11G11B10_FLOAT,
    FORMAT_R24G8_TYPELESS,
    FORMAT_D24_UNORM_S8_UINT,
    FORMAT_A8_UNORM,
    FORMAT_R1_UNORM,
    FORMAT_R9G9B9E5_SHAREDEXP,
    FORMAT_R8G8_B8G8_UNORM,
    FORMAT_G8R8_G8B8_UNORM,
    FORMAT_B5G6R5_UNORM,
    FORMAT_B5G5R5A1_UNORM,
    FORMAT_B8G8R8A8_UNORM,
    FORMAT_B8G8R8X8_UNORM,
    FORMAT_B8G8R8A8_TYPELESS,
    FORMAT_B8G8R8A8_UNORM_SRGB,
    FORMAT_B8G8R8X8_TYPELESS,
    FORMAT_B8G8R8X8_UNORM_SRGB,
    FORMAT_B4G4R4A4_UNORM,

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
    FORMAT_BC6H_TYPELESS,
    FORMAT_BC6H_UF16,
    FORMAT_BC6H_SF16,
    FORMAT_BC7_TYPELESS,
    FORMAT_BC7_UNORM,
    FORMAT_BC7_UNORM_SRGB,

    _FORMAT_COUNT
};
enum INPUT_ELEMENT_CLASSIFICATION
{
    INPUT_ELEMENT_CLASSIFICATION_PER_VERTEX,
    INPUT_ELEMENT_CLASSIFICATION_PER_INSTANCE,

    _INPUT_ELEMENT_CLASSIFICATION_COUNT
};
struct Input_Element_Descriptor
{
    union
    {
        char* name; // D3D12
        unsigned int slot; // VULKAN
    } element_binding; 
    unsigned int element_index;
    enum FORMAT format;
    unsigned int buffer_index;
    unsigned int offset;
    enum INPUT_ELEMENT_CLASSIFICATION element_classification;
    unsigned int instance_rate;
};
struct Pipeline_State_Object_Descriptor
{
    struct Input_Element_Descriptor* input_element_descriptors;
    unsigned int input_element_descriptors_count;
};
struct Viewport
{
    float x;
    float y;
    float width;
    float height;
    float min_depth;
    float max_depth;
};
struct Rect
{
    long left;
    long top;
    long right;
    long bottom;
};

int device_create(struct Device** out_device);
void device_destroy(struct Device* device);
int device_create_command_queue(struct Device* device, struct Command_Queue** out_command_queue);
int device_create_swapchain(struct Device* device, struct Command_Queue* command_queue, struct Swapchain_Descriptor swapchain_descriptor, struct Swapchain** out_swapchain);
int device_create_command_list(struct Device* device, struct Command_List** out_command_list);
int device_create_descriptor_set(struct Device* device, enum DESCRIPTOR_TYPE descriptor_type, size_t descriptor_count, struct Descriptor_Set** out_descriptor_set);
int device_create_buffer(struct Device* device, struct Buffer_Descriptor buffer_description, struct Buffer** out_buffer);
int device_create_upload_buffer(struct Device* device, void* data, size_t data_size, struct Upload_Buffer** out_upload_buffer);
int device_create_shader(struct Device* device, struct Shader** out_shader);
int device_create_pipeline_state_object(struct Device* device, struct Swapchain* swapchain, struct Shader* shader, struct Pipeline_State_Object_Descriptor descriptor, struct Pipeline_State_Object** out_pipeline_state_object);
int device_create_fence(struct Device* device, struct Fence** out_fence);

void command_queue_destroy(struct Command_Queue* command_queue);
void command_queue_execute(struct Command_Queue* command_queue, struct Command_List* command_lists[], size_t command_lists_count);

void swapchain_destroy(struct Swapchain* swapchain);
int swapchain_create_backbuffers(struct Swapchain* swapchain, struct Device* device, struct Descriptor_Set* rtv_descriptor_set, struct Buffer** out_buffers);
int swapchain_get_current_backbuffer_index(struct Swapchain* swapchain);
int swapchain_present(struct Swapchain* swapchain);

void command_list_destroy(struct Command_List* command_list);
void command_list_reset(struct Command_List* command_list);
void command_list_set_pipeline_state_object(struct Command_List* command_list, struct Pipeline_State_Object* pipeline_state_object);
void command_list_set_shader(struct Command_List* command_list, struct Shader* shader);
void command_list_set_viewport(struct Command_List* command_list, struct Viewport viewport);
void command_list_set_scissor_rect(struct Command_List* command_list, struct Rect scissor_rect);
void command_list_set_render_targets(struct Command_List* command_list, struct Buffer* render_targets[], int render_targets_count, struct Buffer* opt_depth_buffer/* = 0*/);
void command_list_clear_render_target(struct Command_List* command_list, struct Buffer* render_target, float clear_color[4]);
void command_list_set_vertex_buffer(struct Command_List* command_list, struct Buffer* vertex_buffer, size_t size, size_t stride);
void command_list_set_primitive_topology(struct Command_List* command_list, enum PRIMITIVE_TOPOLOGY primitive_topology);
void command_list_draw_instanced(struct Command_List* command_list, size_t vertex_count_per_instance, size_t instance_count, size_t start_vertex_location, size_t start_instance_location);
void command_list_copy_upload_buffer_to_buffer(struct Command_List* command_list, struct Upload_Buffer* src, struct Buffer* dst);
void command_list_set_buffer_state(struct Command_List* command_list, struct Buffer* buffer, enum RESOURCE_STATE to_state);
int command_list_close(struct Command_List* command_list);

void descriptor_set_destroy(struct Descriptor_Set* descriptor_set);

void buffer_destroy(struct Buffer* buffer);
struct Buffer_Descriptor buffer_get_descriptor(struct Buffer* buffer);

void upload_buffer_destroy(struct Upload_Buffer* upload_buffer);

void pipeline_state_object_destroy(struct Pipeline_State_Object* pipeline_state_object);

void fence_destroy(struct Fence* fence);
unsigned long long fence_signal(struct Fence* fence, struct Command_Queue* command_queue);
void fence_wait_for_completion(struct Fence* fence);
int fence_is_completed(struct Fence* fence);
unsigned long long fence_get_completed_value(struct Fence* fence);

#endif