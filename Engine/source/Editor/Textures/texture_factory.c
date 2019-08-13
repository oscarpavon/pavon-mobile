
#include "texture_factory.h"
#include "../../engine.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../third_party/stb_image_write.h"
#include "../windows.h"
#include "../HLOD/HLOD_factory.h"
Model *uv_model;
GLuint renderedTexture;

GLint old_fbo;
Model *fireman;
Model *UV_true;

void * textures_pixels[2];
int render_texture_count = 0; 

int offsetU = 0;
int offsetV = 0;

Array model_in_UV_form;

void export_texture(){
    if(textures_pixels[0] == NULL){
        LOG("Texture not exported\n");
        return;
    }
    stbi_write_png("test.png", 512, 512, 3, textures_pixels[0], 512 * 3);
}




void draw_texture(int size,int atlas_texture_size)
{
    glBindBuffer(GL_ARRAY_BUFFER, uv_model->vertex_buffer_id);

    glUseProgram(uv_model->shader);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void *)offsetof(struct Vertex, uv));

    glBindTexture(GL_TEXTURE_2D, uv_model->texture.id);
    GLint mvp_uniform = glGetUniformLocation(uv_model->shader, "MVP");

    mat4 scale;
    glm_mat4_identity(scale);

    mat4 position_mat;
    glm_mat4_identity(position_mat);

    mat4 projection;
    glm_mat4_identity(projection);

    glm_ortho(0, atlas_texture_size, atlas_texture_size, 0, 0, 1, projection);
    //glm_mat4_inv(projection,projection);

    glm_scale(scale, (vec3){size, size, 0});

    glm_translate(position_mat, (vec3){offsetU, offsetV, 0});

    mat4 model;
    mat4 rotation;
    glm_mat4_identity(rotation);
    //glm_rotate(rotation,18,VEC3(0,0,1));

    glm_mul(position_mat, rotation, model);
    glm_mul(model,scale,model);
    

    mat4 model_projection;
    glm_mat4_identity(model_projection);

    glm_mul(projection, model, model_projection);

    glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, &model_projection[0][0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uv_model->index_buffer_id);

    if (uv_model->index_array.count == 0)
        LOG("Index is equal to 0, model not render\n");
    glDrawElements(GL_TRIANGLES, uv_model->index_array.count, GL_UNSIGNED_SHORT, (void *)0);

    offsetU += size;
}



void draw_textures(int per_texture_size, int atlas_size){
    for(int i = 0; i<model_in_UV_form.count; i++)
    {
        Model** ppModel = get_from_array(&model_in_UV_form,i);
        Model* model = ppModel[0];
        uv_model = model;
        draw_texture(per_texture_size,atlas_size);
        
    }
    
}

void render_to_texture(int size)
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old_fbo);

    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    glGenTextures(1, &renderedTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, size, size);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           renderedTexture, 0);

    // Render to our framebuffer

    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    glViewport(0, 0, size, size); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //draw_editor_viewport();
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        draw_textures(256,size);
    glEnable(GL_CULL_FACE);

    
    textures_pixels[render_texture_count] = malloc(size * size * 3);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, size, size, GL_RGB, GL_UNSIGNED_BYTE, textures_pixels[render_texture_count]);

    glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);
    glViewport(0, 0, actual_window_width, actual_window_height);
}


void init_UV_draw(Model *model)
{

    new_empty_model();
    init_array(&selected_model->vertex_array, sizeof(Vertex), model->vertex_array.count);
    for (int i = 0; i < model->vertex_array.count; i++)
    {
        Vertex *vertex = get_from_array(&model->vertex_array, i);
        vec3 position;
        glm_vec3_copy(VEC3(vertex->uv[0], vertex->uv[1], 0), position);
        add_to_array(&selected_model->vertex_array, position);
    }

    init_model_gl_buffers(selected_model);
    selected_model->index_buffer_id = model->index_buffer_id;
    selected_model->index_array.count = model->index_array.count;

    selected_model->shader = create_engine_shader(standart_vertex_shader, standart_fragment_shader);
    selected_model->texture.id = model->texture.id;
    uv_model = selected_model;

    if(model_in_UV_form.initialized)
    add_to_array(&model_in_UV_form,&selected_model);
    
}

void add_model_to_UV_proccessing(ComponentDefinition* component){
    if(component->type == STATIC_MESH_COMPONENT){
        StaticMeshComponent* mesh = component->data;
        unsigned int* id = get_from_array(&mesh->meshes, 1);
        Model* model = get_from_array(actual_model_array,*id);
        init_UV_draw(model);
    }
}
void init_model_to_draw_texture(){
    if(!model_in_UV_form.initialized)
        init_array(&model_in_UV_form,sizeof(Model*),20);
    
    for_each_element_components_in_array_of_pp(&array_elements_for_HLOD_generation,add_model_to_UV_proccessing);
}

bool initialized = false;

void scale_UV(float size, Model* model, vec2 UV_offset){
    for (int i = 0; i < model->vertex_array.count; i++)
    {
        Vertex *vertex = get_from_array(&model->vertex_array, i);
        vec3 position;
        glm_vec3_copy(VEC3(vertex->uv[0]+UV_offset[0], vertex->uv[1]+UV_offset[1], 0), position);
        glm_vec3_scale(position,size,position);
        vertex->uv[0] = position[0];
        vertex->uv[1] = position[1]*-1;
    }
}
void translate_UV(vec3 tranlation, Model* model, vec2 UV_offset){
    for (int i = 0; i < model->vertex_array.count; i++)
    {
        Vertex *vertex = get_from_array(&model->vertex_array, i);
        vec3 position;
        glm_vec3_copy(VEC3(vertex->uv[0]+UV_offset[0], vertex->uv[1]+UV_offset[1], 0), position);
        glm_vec3_add(position,tranlation,position);
        vertex->uv[0] = position[0];
        vertex->uv[1] = position[1];
    }
}

void draw_UV()
{
    if(!initialized){
    scale_UV(0.5,selected_model,(vec2){0,0});
    init_UV_draw(selected_model);
    initialized = true;
    }
    glBindBuffer(GL_ARRAY_BUFFER, uv_model->vertex_buffer_id);

    glUseProgram(uv_model->shader);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void *)offsetof(struct Vertex, uv));

    glBindTexture(GL_TEXTURE_2D, uv_model->texture.id);
    GLint mvp_uniform = glGetUniformLocation(uv_model->shader, "MVP");

    mat4 mvp;
    glm_mat4_identity(mvp);
    glm_rotate(mvp, 180, VEC3(0, 1, 0));
    glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, &mvp[0][0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uv_model->index_buffer_id);

    glm_mat4_copy(mvp,uv_model->model_mat);

    if (uv_model->index_array.count == 0)
        LOG("Index is equal to 0, model not render\n");
    glDrawElements(GL_TRIANGLES, uv_model->index_array.count, GL_UNSIGNED_SHORT, (void *)0);
    //glDrawArrays(GL_LINE_STRIP,0,uv_model->vertex_array.count);
    
}

void merge_textures(){
    init_model_to_draw_texture();
    render_to_texture(512);
    export_texture();
    free(textures_pixels[0]);
}
