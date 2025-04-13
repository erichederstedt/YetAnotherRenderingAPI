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
int device_create_pipeline_state_object(struct Device* device, struct Swapchain* swapchain, struct Shader* shader, struct Pipeline_State_Object** out_pipeline_state_object);
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