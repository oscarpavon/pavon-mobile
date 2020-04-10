#ifndef ENGINE_WINDOWS_MANAGER_H
#define ENGINE_WINDOWS_MANAGER_H

#define GLFW_INCLUDE_ES2
#define GLFW_INCLUDE_GLEXT

#include <GLFW/glfw3.h>

#include "../engine/camera.h"
#ifdef EDITOR
#include "../editor/windows/tabs.h"
#include "../editor/windows/static_mesh_editor.h"
#endif //EDITOR

typedef struct EngineWindow{
	Array tabs;
 	char name[20];
 	bool focus;
 	bool initialized; 
 	GLFWwindow* window;
	void(*draw)(void);
	void(*init)(void);
	void(*finish)(void);
	void(*input)(void);
#ifdef EDITOR
	u8 tab_current_id;
	EditorTab* tab_current;
#endif
	CameraComponent camera;
}EngineWindow;

void window_resize_callback(GLFWwindow* window, int width, int height);
void window_focus_callback(GLFWwindow*,int);
void window_create(EngineWindow *win,EngineWindow* share_window,const char* name);

void window_manager_init_window(EngineWindow* window);
void window_manager_draw_windows();
void window_initialize_windows();

void window_update_viewport();

void window_manager_create_editor_windows_data();

void window_set_focus(EngineWindow* window);

void windows_manager_init();

void window_update_windows_input();

float actual_window_width;
float actual_window_height;

bool editor_window_content_open;

EngineWindow* window_editor_main;

EngineWindow* current_window;

Array engine_windows;

inline static void window_update_envents(){
       glfwPollEvents();
}
#endif // !ENGINE_WINDOWS_MANAGER_H
