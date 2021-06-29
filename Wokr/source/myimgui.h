#pragma once
#include <SDL2/SDL.h>
#include "engine.hxx"
#include "imgui/imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"



void imgui_init(SDL_Window* window, SDL_GLContext gl_context){

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& im=ImGui::GetIO();
    (void)im;

    //set dart theme
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window,gl_context);
    ImGui_ImplOpenGL3_Init("#version 300 es");

}
void imgui_newframe(SDL_Window* window){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();
}
void imgui_window(glm::mat4 & bodyMove, glm::mat4 & buffMat, bool & isVig,float * color,
                  bool * istim,float * range_of_vag){



    if(ImGui::Button("rotate")){
        buffMat=glm::mat4(1.0f);
        buffMat=glm::rotate(buffMat,glm::radians(45.0f),glm::vec3(0.0f,0.0f,1.0f));



       bodyMove*=buffMat;
    }
    if(ImGui::Button("moveLeft")){
        /*buffMat=glm::mat4(1.0f);

        buffMat=glm::translate(buffMat, glm::vec3(-0.2f,0.0f,0.0f));
        bodyMove*=buffMat;*/

        bodyMove[3][0]-=0.1;
    }
    if(ImGui::Button("moveRight")){
        /*buffMat=glm::mat4(1.0f);

        buffMat=glm::translate(buffMat, glm::vec3(0.2f,0.0f,0.0f));
        bodyMove*=buffMat;*/

        bodyMove[3][0]+=0.1;

    }
    if(ImGui::Button("Scale+")){
        buffMat=glm::mat4(1.0f);
        buffMat=glm::scale(buffMat,glm::vec3(2.0f,2.0f,0.0f));

        bodyMove*=buffMat;
    }
    if(ImGui::Button("Scale-")){
        buffMat=glm::mat4(1.0f);
        buffMat=glm::scale(buffMat,glm::vec3(0.5f,0.5f,0.0f));

        bodyMove*=buffMat;
    }
    if(ImGui::Button("Vignette")){
        if(isVig==true){
            isVig=false;
        }else{
            isVig=true;
        }
    }


    ImGui::Checkbox("timer",istim);

    ImGui::SliderFloat("color-mod",color,0.0f,1.0f);
    ImGui::SliderFloat("range_of_Vegnet",range_of_vag,0.25f,1.0f);
}
void imgui_render(){
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void imgui_shutdown(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}
