#ifndef ENGINE_COMPONENTS_H
#define ENGINE_COMPONENTS_H

#include "../../engine.h"

typedef struct CameraComponent{
    mat4 projection;
    mat4 view;
    vec3 front;
    vec3 up;
}CameraComponent;

typedef enum ComponentType{
    TRASNFORM_COMPONENT,
    SPHERE_COMPONENT,
    CAMERA_COMPONENT,
    CUBE_COMPONENT
}ComponentType;


typedef struct ComponentDefinition{
    ComponentType type;
    unsigned int id;
    unsigned short int bytes_size;
    void* data;
}ComponentDefinition;

typedef struct SphereComponent{    
    int radius;
    vec3 center;
    Model* model;
}SphereComponent;

typedef struct TransformComponent{    
    vec3 position;
    vec4 rotation;
    vec3 scale;
    mat4 model_matrix;
}TransformComponent;

void add_component_to_selected_element(int component_byte_size, void* new_component, ComponentType type);
void init_sphere_component(SphereComponent* component);
void update_component(ComponentDefinition*);
void init_transfrom_component(TransformComponent* component);
#endif