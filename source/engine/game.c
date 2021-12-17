//
// Created by pavon on 6/24/19.
//

#include "game.h"
#include "input.h"

#include "gui.h"
#include "../ThirdParty/cglm/cglm.h"

#include "camera.h"

#include "engine.h"

#include <dlfcn.h>

static vec4 backcolor= {0.1,0.2,0.4,1};

void pe_game_render_init(){
//referenced from render_thread_definition 
    
  LOG("render game init\n");
  game->init();
}

void pe_game_render_config(){
	render_thread_definition.init = &pe_game_render_init;
	render_thread_definition.draw = &window_manager_draw_windows; 
	render_thread_definition.end = &glfwTerminate;
}
void pe_game_draw(){

  pe_frame_draw();

}
void pe_game_init(){

    window_create(game_window, NULL, game->name); 
    
    glfwSetKeyCallback(game_window->window, pe_input_key_callback);
	glfwSetCursorPosCallback(game_window->window, pe_input_mouse_movement_callback);
	glfwSetMouseButtonCallback(game_window->window, pe_input_mouse_button_callback);
    glfwSetCharCallback(game_window->window, pe_input_key_callback);
    glfwSetWindowFocusCallback(game_window->window,window_focus_callback);
    glfwSetFramebufferSizeCallback(game_window->window, window_resize_callback);

}
void pe_game_input(){
  game->input();

}

void pe_game_create(PGame * created_game){
    game = created_game; 

    pe_renderer_type = PEWMOPENGLES2;
    pe_init();

   
    EngineWindow win;
    ZERO(win);

    win.init = &pe_game_init;//window specific data
    win.draw = &pe_game_draw;//Main loop draw in window
    win.input = &pe_game_input; 

    array_add(&engine_windows, &win);
    game_window = array_pop(&engine_windows);


    //Send window initialization to the render thread

    PEThreadCommand thread_commad;
    thread_commad.command = &window_manager_init_window;
    thread_commad.data = game_window;
    thread_commad.done = false;
    thread_commad.type = POINTER;
    array_add(&render_thread_commads, &thread_commad);



    pe_game_render_config();
    pe_render_thread_start_and_draw();

    LOG("game created\n");
    pe_program_main_loop(game->loop, game_window);

}




int load_gamplay_code(){
    
    char *error;

    dynamic_lib_handle = dlopen("../Game/src/test.so", RTLD_GLOBAL | RTLD_NOW);
    if(!dynamic_lib_handle){
        LOG("ERROR: Gameplay library not loaded\nError: %s \n",dlerror() );
        return -1;
    }    

    loop_fuction_dynamic_loaded = dlsym(dynamic_lib_handle,"test");
    if ((error = dlerror()) != NULL) 
    {
        fprintf(stderr, "%s\n", error);
        return -1;
    }
  
    return 0;
}

void close_dynamic_game_play(){
    if(dynamic_lib_handle)
        dlclose(dynamic_lib_handle);
}


void move_player_forward(){
    //LOG("Action pointer work\n");
    update_translation(VEC3(0,-0.3,0));

}

void move_player_backward(){
    update_translation(VEC3(0,0.3,0));
}

void rotate_player_left(){
    versor new_rot_quat;
    glm_quatv(new_rot_quat, glm_rad(5), VEC3(0,0,1) );
    rotate_element(selected_element,new_rot_quat);
}

void rotate_player_right(){
    versor new_rot_quat;
    glm_quatv(new_rot_quat, glm_rad(-5), VEC3(0,0,1) );
    rotate_element(selected_element,new_rot_quat);
}

int init_game(){
    add_action_function(&move_player_forward);
    add_action_function(&move_player_backward);
    add_action_function(&rotate_player_left);
    add_action_function(&rotate_player_right);

    game_initialized = true;
    return 0;
}

void update_game(){
    selected_element = array_get(actual_elements_array,0);
    CameraComponent* camera = get_component_from_element(selected_element,CAMERA_COMPONENT);
    update_main_camera_with_camera_component_values(camera);
}
