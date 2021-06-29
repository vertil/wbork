#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <SDL2/SDL.h>
#include <thread>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "structs.hxx"
#include "lodepng.h"

#include "myimgui.h"

#include "glad/glad.h"


static void APIENTRY callback_opengl_debug(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, [[maybe_unused]] const void* userParam);

static void audio_callback(void* userdata,uint8_t* stream, int len);

class engine{
    //openGL programs
    GLuint        program_id_simple = 0;
    GLuint        program_id_body = 0;
    GLuint        program_id_body_NoVig = 0;
    //textures
    GLuint tex_handl [2];

    glm::mat4 bodyMove=glm::mat4(1.0f);
    glm::mat4 buffMat=glm::mat4(1.0f);

    float colorDun=1.0;

    float range_of_veg=0.8f;

    bool isvignette=true;
    bool istimer=false;

    //--MAP
    triangle background1;
    triangle background2;

    triangle body1;
    triangle body2;


    //----SDL
    SDL_Window *window;
    SDL_Event event_log;
    SDL_Renderer *rend;


public:
    engine(){
        std::stringstream serr;
        using namespace std;


        const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
        if (init_result != 0)
        {
            const char* err_message = SDL_GetError();
             serr << "error: failed call SDL_Init: " << err_message << std::endl;
            //return serr.str();
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

        window=SDL_CreateWindow("Loop",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  1000,
                                  1000,
                                  SDL_WINDOW_OPENGL
                                  );

           if(window==NULL){
           std::cout<<"Couldn't create window"<<SDL_GetError()<<std::endl;
               SDL_DestroyWindow(window);
           }

           rend=SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

           int gl_major_ver       = 3;
           int gl_minor_ver       = 2;
           int gl_context_profile = SDL_GL_CONTEXT_PROFILE_ES;

           const char* platform_from_sdl = SDL_GetPlatform();
           std::string_view platform{platform_from_sdl};
           using namespace std::string_view_literals;
           using namespace std;
           auto list = { "Windows"sv, "Mac OS X"sv };
           auto it   = find(begin(list), end(list), platform);
           if (it != end(list))
           {
               gl_major_ver       = 4;
               gl_minor_ver       = (platform == "Mac OS X") ? 1 : 3;
               gl_context_profile = SDL_GL_CONTEXT_PROFILE_ES;
           }

           SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_context_profile);
           SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver);
           SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver);

           SDL_GLContext gl_context = SDL_GL_CreateContext(window);
           if (gl_context == nullptr)
           {
               gl_major_ver       = 3;
               gl_minor_ver       = 2;
               gl_context_profile = SDL_GL_CONTEXT_PROFILE_ES;

               SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_context_profile);
               SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_ver);
               SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_ver);
               gl_context = SDL_GL_CreateContext(window);
           }
           assert(gl_context != nullptr);

           int result =
               SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
           assert(result == 0);
           result =
               SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
           assert(result == 0);

           if (gl_major_ver < 3)
           {
               std::clog << "current context opengl version: " << gl_major_ver
                         << '.' << gl_minor_ver << '\n'
                         << "need openg ES version at least: 3.2\n"
                         << std::flush;
               throw std::runtime_error("opengl version too low");
           }

           std::clog << "OpenGl "<< gl_major_ver << '.'<< gl_minor_ver<<'\n';

           if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0)
           {
               std::clog << "error: failed to initialize glad" << std::endl;
           }

           if (platform != "Mac OS X") // not supported on Mac
           {
               glEnable(GL_DEBUG_OUTPUT);
               glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
               glDebugMessageCallback(callback_opengl_debug, nullptr);
               glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                                 nullptr, GL_TRUE);
           }

           std::cout<<"sas"<<std::endl;

           glGenTextures(20, tex_handl); //texture descriptor
           OM_GL_CHECK()
                   gl_context=SDL_GL_CreateContext(window);
                   imgui_init(window,gl_context);

    }
    void initData(){


        body1.v[0].x=-0.1;
        body1.v[0].y=-0.1;
        body1.v[0].tex_x=0.0;
        body1.v[0].tex_y=1.0;
                //2
                body1.v[1].x=-0.1;
                body1.v[1].y=0.1;
                body1.v[1].tex_x=0.0;
                body1.v[1].tex_y=0.0;
                //3
                body1.v[2].x=0.1;
                body1.v[2].y=-0.1;
                body1.v[2].tex_x=1.0;
                body1.v[2].tex_y=1.0;
                //
                body2.v[0].x=-0.1;
                    body2.v[0].y=0.1;
                    body2.v[0].tex_x=0.0;
                    body2.v[0].tex_y=0.0;
                    //2
               body2.v[1].x=0.1;
               body2.v[1].y=0.1;
               body2.v[1].tex_x=1.0;
               body2.v[1].tex_y=0.0;
                    //3
               body2.v[2].x=0.1;
               body2.v[2].y=-0.1;
               body2.v[2].tex_x=1.0;
               body2.v[2].tex_y=1.0;

        bodyMove=glm::mat4(1.0f);



    }
    ~engine(){


        imgui_shutdown();
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    bool load_texture(std::string_view path,uint number){
        const char* filename=&path[0];
        std::vector<unsigned char> image;
        unsigned int          w = 0;
        unsigned int          h = 0;
        int error = lodepng::decode(image,w,h,filename);
        /*GLuint tex_handl = 0;
        glGenTextures(1, &tex_handl); //texture descriptor
        OM_GL_CHECK()
        glBindTexture(GL_TEXTURE_2D, tex_handl);
        OM_GL_CHECK()*/
        glActiveTexture(GL_TEXTURE0+number);
        glBindTexture(GL_TEXTURE_2D,tex_handl[number]);
        GLint mipmap_level = 0;
        GLint border       = 0;
        glTexImage2D(GL_TEXTURE_2D, // type of texture
                     mipmap_level,  //
                     GL_RGBA,       //color format in
                     static_cast<GLsizei>(w),//texture weight
                     static_cast<GLsizei>(h),//texture height
                     border,
                     GL_RGBA,       // color format out(color what we want to get in opengl)
                     GL_UNSIGNED_BYTE, // Specifies the data type of the texel data
                     &image[0]);
        OM_GL_CHECK()
        //making correct color of zoomed texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        OM_GL_CHECK()
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        OM_GL_CHECK()
        return true;

    }
    bool event(){
        while(SDL_PollEvent(&event_log)){
            if(event_log.type==SDL_QUIT){
                return false;
            }else if(event_log.type==SDL_KEYDOWN){

                    switch(event_log.key.keysym.scancode){
                    case SDL_SCANCODE_A:
                        //move X
                        //std::cout<<std::to_string(bodyMove[3][0])<<" "<<std::to_string(bodyMove[3][1])<<" "<<std::to_string(bodyMove[3][2])<<" "<<std::to_string(bodyMove[3][3])<<std::endl;
                        //buffMat=glm::mat4(1.0f);

                        //buffMat=glm::translate(buffMat, glm::vec3(-0.2f,0.0f,0.0f));

                        //bodyMove*=buffMat;
                        bodyMove[3][0]-=0.1;

                        break;
                    case SDL_SCANCODE_D:
                        //move X
                        //buffMat=glm::mat4(1.0f);

                        //buffMat=glm::translate(buffMat, glm::vec3(0.2f,0.0f,0.0f));

                        //bodyMove*=buffMat;
                        bodyMove[3][0]+=0.1;
                        break;
                    case SDL_SCANCODE_S:
                        //rotate


                        buffMat=glm::mat4(1.0f);
                        buffMat=glm::rotate(buffMat,glm::radians(45.0f),glm::vec3(0.0f,0.0f,1.0f));



                       bodyMove*=buffMat;
                       break;
                     case SDL_SCANCODE_Q:

                        buffMat=glm::mat4(1.0f);
                        buffMat=glm::scale(buffMat,glm::vec3(2.0f,2.0f,0.0f));

                        bodyMove*=buffMat;

                        break;
                    case SDL_SCANCODE_E:

                       buffMat=glm::mat4(1.0f);
                       buffMat=glm::scale(buffMat,glm::vec3(0.5f,0.5f,0.0f));

                       bodyMove*=buffMat;

                       break;

                }


                return true;
            }else if(event_log.type==SDL_MOUSEBUTTONDOWN){
                checkColl();




            }
          return true;

        }
        return true;
    }
    void checkColl(){
        int x,y;
        SDL_GetMouseState(&x,&y);
        float xf,yf;
        if(x>=500){
            xf=0.0f+(1.0f/500.0f)*(x-500);
        }else{
            xf=-1+(1.0f/500.0f)*x;
        }
        //std::cout<<xf<<std::endl;

        if(y>=500){
            yf=0.0f+(1.0f/500.0f)*(500-y);
        }else{
            yf=-1.0f+(1.0f/500.0f)*(1000-y);
        }
        //std::cout<<yf<<std::endl;



            glm::vec4 point1=glm::vec4(body1.v[0].x,body1.v[0].y,0.0f,0.0f)*bodyMove;
            glm::vec4 point2=glm::vec4(body1.v[1].x,body1.v[1].y,0.0f,0.0f)*bodyMove;
            glm::vec4 point4=glm::vec4(body1.v[2].x,body1.v[2].y,0.0f,0.0f)*bodyMove;
            glm::vec4 point3=glm::vec4(body2.v[1].x,body2.v[1].y,0.0f,0.0f)*bodyMove;
            std::vector<glm::vec4> points;

            float rebro_lenght=sqrt(pow(point1.x-point2.x,2)+pow(point1.y-point2.y,2));
            std::cout<<std::to_string(rebro_lenght)<<" ";
            float diag=sqrt(pow(point1.x-point4.x,2)+pow(point1.y-point4.y,2));
            //std::cout<<diag;
            std::vector<float> squares;

           squares.push_back(
             abs( ((point1.x-xf)*(point2.y-yf))-((point2.x-xf)*(point1.y-yf)) ) *0.5f
             );
           squares.push_back(
             abs( ((point2.x-xf)*(point3.y-yf))-((point3.x-xf)*(point2.y-yf)) ) *0.5f
             );
           squares.push_back(
             abs( ((point3.x-xf)*(point4.y-yf))-((point4.x-xf)*(point3.y-yf)) ) *0.5f
             );
           squares.push_back(
             abs( ((point4.x-xf)*(point1.y-yf))-((point1.x-xf)*(point4.y-yf)) ) *0.5f
             );
            float square=0;
            for(auto i:squares){
                square+=i;
            }
            std::cout<<std::to_string(square)<<std::endl;


            if(rebro_lenght*rebro_lenght-0.00001<=square && square<=rebro_lenght*rebro_lenght+0.00001){
                //std::cout<<"sas"<<std::endl;
                if(colorDun==0.0f){
                    colorDun=1.0f;
                }else{
                    colorDun=0.0f;
                }
            }









    }
    void swapBuffers(){
        SDL_GL_SwapWindow(window);
    }
    void initfTriagBackground(){
        background1.v[0].x=-1;
        background1.v[0].y=-1;
        background1.v[0].tex_x=0.0;
        background1.v[0].tex_y=1.0;
                //2
                background1.v[1].x=-1;
                background1.v[1].y=1;
                background1.v[1].tex_x=0.0;
                background1.v[1].tex_y=0.0;
                //3
                background1.v[2].x=1;
                background1.v[2].y=-1;
                background1.v[2].tex_x=1.0;
                background1.v[2].tex_y=1.0;
                //
                background2.v[0].x=-1;
                    background2.v[0].y=1;
                    background2.v[0].tex_x=0.0;
                    background2.v[0].tex_y=0.0;
                    //2
               background2.v[1].x=1;
               background2.v[1].y=1;
               background2.v[1].tex_x=1.0;
               background2.v[1].tex_y=0.0;
                    //3
               background2.v[2].x=1;
               background2.v[2].y=-1;
               background2.v[2].tex_x=1.0;
               background2.v[2].tex_y=1.0;
    }
    std::string initProgramBackground(){
        std::stringstream serr;
        //vertex
        GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
        OM_GL_CHECK()
        std::string_view vertex_shader_src = R"(
                                             #version 300 es

                                             layout(location=0)in vec4 vPosition;
                                             in vec2 a_tex_coord;

                                             out vec2 v_tex_coord;

                                             void main()
                                             {
                                                 v_tex_coord=a_tex_coord;
                                                 gl_Position=vPosition;//vertex pos
                                             }
                                        )";
        const char* source            = vertex_shader_src.data();
        glShaderSource(vert_shader, 1, &source, nullptr);
        OM_GL_CHECK()

        glCompileShader(vert_shader);
        OM_GL_CHECK()

        GLint compiled_status = 0;
        glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &compiled_status);
        OM_GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &info_len);
            OM_GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(vert_shader, info_len, nullptr, info_chars.data());
            OM_GL_CHECK()
            glDeleteShader(vert_shader);
            OM_GL_CHECK()

            std::string shader_type_name = "vertex";
            serr << "Error compiling shader(vertex)\n"
                 << vertex_shader_src << "\n"
                 << info_chars.data();
            return serr.str();
        }
        //fragment
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        OM_GL_CHECK()
        std::string_view fragment_shader_src = R"(
                                               #version 300 es
                                               precision mediump float;

                                               in vec2 v_tex_coord;

                                               uniform sampler2D s_texture;

                                               out vec4 frag_color;
                                               void main()
                                               {
                                                    vec4 color=texture(s_texture, v_tex_coord);

                                                 frag_color = color;
                                               }
                          )";
        /**/
        source                          = fragment_shader_src.data();
        glShaderSource(fragment_shader, 1, &source, nullptr);
        OM_GL_CHECK()

        glCompileShader(fragment_shader);
        OM_GL_CHECK()

        compiled_status = 0;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled_status);
        OM_GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &info_len);
            OM_GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(
                fragment_shader, info_len, nullptr, info_chars.data());
            OM_GL_CHECK()
            glDeleteShader(fragment_shader);
            OM_GL_CHECK()

            serr << "Error compiling shader(fragment)\n"
                 << vertex_shader_src << "\n"
                 << info_chars.data();
            return serr.str();
        }
        //union shaderis in one program

        program_id_simple = glCreateProgram();
        OM_GL_CHECK()
        if (0 == program_id_simple)
        {
            serr << "failed to create gl program";
            return serr.str();
        }

        glAttachShader(program_id_simple, vert_shader);
        OM_GL_CHECK()
        glAttachShader(program_id_simple, fragment_shader);
        OM_GL_CHECK()

        // bind attribute location
        glBindAttribLocation(program_id_simple, 0, "a_position");
        OM_GL_CHECK()
        // link program after binding attribute locations
        glLinkProgram(program_id_simple);
        OM_GL_CHECK()
        // Check the link status
        GLint linked_status = 0;
        glGetProgramiv(program_id_simple, GL_LINK_STATUS, &linked_status);
        OM_GL_CHECK()
        if (linked_status == 0)
            {
                GLint infoLen = 0;
                glGetProgramiv(program_id_simple, GL_INFO_LOG_LENGTH, &infoLen);
                OM_GL_CHECK()
                    std::vector<char> infoLog(static_cast<size_t>(infoLen));
                glGetProgramInfoLog(program_id_simple, infoLen, nullptr, infoLog.data());
                OM_GL_CHECK()
                serr << "Error linking program:\n" << infoLog.data();
                glDeleteProgram(program_id_simple);
                OM_GL_CHECK()
                return serr.str();
            }
        return "";

    }
    std::string initProgramBody(){
        std::stringstream serr;
        //vertex
        GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
        OM_GL_CHECK()
        std::string_view vertex_shader_src = R"(
                                             #version 300 es
                                             uniform mat4 rot_matrix;

                                             layout(location=0)in vec4 vPosition;
                                             in vec2 a_tex_coord;



                                             out vec2 v_tex_coord;
                                             out vec2 u_resolution;

                                             void main()
                                             {
                                                 v_tex_coord=a_tex_coord;


                                                 gl_Position=rot_matrix * vPosition;//vertex pos
                                             }
                                        )";
        const char* source            = vertex_shader_src.data();
        glShaderSource(vert_shader, 1, &source, nullptr);
        OM_GL_CHECK()

        glCompileShader(vert_shader);
        OM_GL_CHECK()

        GLint compiled_status = 0;
        glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &compiled_status);
        OM_GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &info_len);
            OM_GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(vert_shader, info_len, nullptr, info_chars.data());
            OM_GL_CHECK()
            glDeleteShader(vert_shader);
            OM_GL_CHECK()

            std::string shader_type_name = "vertex";
            serr << "Error compiling shader(vertex)\n"
                 << vertex_shader_src << "\n"
                 << info_chars.data();
            return serr.str();
        }
        //fragment
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        OM_GL_CHECK()
        std::string_view fragment_shader_src = R"(

                                               #version 300 es
                                               precision mediump float;
                                               in vec2 v_tex_coord;
                                               in vec2 u_resolution;
                                               uniform sampler2D s_texture;
                                               uniform float color_dub;
                                               uniform float range_of_veg;
                                               out vec4 frag_color;
                                               void main()
                                               {
                                                    vec4 color=texture(s_texture, v_tex_coord);
                                                    vec2 pos=(v_tex_coord/1.0)-vec2(0.5);

                                                    float dist=length(pos);

                                                    color.rgb=(color.rgb-(1.0-smoothstep(range_of_veg,0.2,dist)));

                                                    color.g*=color_dub;
                                                    color.b*=color_dub;


                                                    frag_color = color;

                                               }
                )";
        /**/
        source                          = fragment_shader_src.data();
        glShaderSource(fragment_shader, 1, &source, nullptr);
        OM_GL_CHECK()

        glCompileShader(fragment_shader);
        OM_GL_CHECK()

        compiled_status = 0;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled_status);
        OM_GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &info_len);
            OM_GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(
                fragment_shader, info_len, nullptr, info_chars.data());
            OM_GL_CHECK()
            glDeleteShader(fragment_shader);
            OM_GL_CHECK()

            serr << "Error compiling shader(fragment)\n"
                 << vertex_shader_src << "\n"
                 << info_chars.data();
            return serr.str();
        }
        //union shaderis in one program

        program_id_body = glCreateProgram();
        OM_GL_CHECK()
        if (0 == program_id_body)
        {
            serr << "failed to create gl program";
            return serr.str();
        }

        glAttachShader(program_id_body, vert_shader);
        OM_GL_CHECK()
        glAttachShader(program_id_body, fragment_shader);
        OM_GL_CHECK()

        // bind attribute location
        glBindAttribLocation(program_id_body, 0, "a_position");
        OM_GL_CHECK()
        // link program after binding attribute locations
        glLinkProgram(program_id_body);
        OM_GL_CHECK()
        // Check the link status
        GLint linked_status = 0;
        glGetProgramiv(program_id_body, GL_LINK_STATUS, &linked_status);
        OM_GL_CHECK()
        if (linked_status == 0)
            {
                GLint infoLen = 0;
                glGetProgramiv(program_id_body, GL_INFO_LOG_LENGTH, &infoLen);
                OM_GL_CHECK()
                    std::vector<char> infoLog(static_cast<size_t>(infoLen));
                glGetProgramInfoLog(program_id_body, infoLen, nullptr, infoLog.data());
                OM_GL_CHECK()
                serr << "Error linking program:\n" << infoLog.data();
                glDeleteProgram(program_id_body);
                OM_GL_CHECK()
                return serr.str();
            }

        return "";

    }
    std::string initProgramBodyNoVig(){
        std::stringstream serr;
        //vertex
        GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
        OM_GL_CHECK()
        std::string_view vertex_shader_src = R"(
                                             #version 300 es
                                             uniform mat4 rot_matrix;
                                             layout(location=0)in vec4 vPosition;
                                             in vec2 a_tex_coord;

                                             out vec2 v_tex_coord;

                                             void main()
                                             {

                                                 v_tex_coord=a_tex_coord;
                                                 gl_Position=rot_matrix * vPosition;//vertex pos
                                             }
                                        )";
        const char* source            = vertex_shader_src.data();
        glShaderSource(vert_shader, 1, &source, nullptr);
        OM_GL_CHECK()

        glCompileShader(vert_shader);
        OM_GL_CHECK()

        GLint compiled_status = 0;
        glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &compiled_status);
        OM_GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(vert_shader, GL_INFO_LOG_LENGTH, &info_len);
            OM_GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(vert_shader, info_len, nullptr, info_chars.data());
            OM_GL_CHECK()
            glDeleteShader(vert_shader);
            OM_GL_CHECK()

            std::string shader_type_name = "vertex";
            serr << "Error compiling shader(vertex)\n"
                 << vertex_shader_src << "\n"
                 << info_chars.data();
            return serr.str();
        }
        //fragment
        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        OM_GL_CHECK()
        std::string_view fragment_shader_src = R"(
                                               #version 300 es
                                               precision mediump float;

                                               in vec2 v_tex_coord;

                                               uniform sampler2D s_texture;
                                               uniform float color_dub;

                                               out vec4 frag_color;
                                               void main()
                                               {
                                               vec4 color=texture(s_texture, v_tex_coord);
                                                 color.g*=color_dub;
                                                 color.b*=color_dub;
                                                 frag_color = color;
                                               }
                          )";
        /**/
        source                          = fragment_shader_src.data();
        glShaderSource(fragment_shader, 1, &source, nullptr);
        OM_GL_CHECK()

        glCompileShader(fragment_shader);
        OM_GL_CHECK()

        compiled_status = 0;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled_status);
        OM_GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &info_len);
            OM_GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(
                fragment_shader, info_len, nullptr, info_chars.data());
            OM_GL_CHECK()
            glDeleteShader(fragment_shader);
            OM_GL_CHECK()

            serr << "Error compiling shader(fragment)\n"
                 << vertex_shader_src << "\n"
                 << info_chars.data();
            return serr.str();
        }
        //union shaderis in one program

        program_id_body_NoVig = glCreateProgram();
        OM_GL_CHECK()
        if (0 == program_id_body_NoVig)
        {
            serr << "failed to create gl program";
            return serr.str();
        }

        glAttachShader(program_id_body_NoVig, vert_shader);
        OM_GL_CHECK()
        glAttachShader(program_id_body_NoVig, fragment_shader);
        OM_GL_CHECK()

        // bind attribute location
        glBindAttribLocation(program_id_body_NoVig, 0, "a_position");
        OM_GL_CHECK()
        // link program after binding attribute locations
        glLinkProgram(program_id_body_NoVig);
        OM_GL_CHECK()
        // Check the link status
        GLint linked_status = 0;
        glGetProgramiv(program_id_body_NoVig, GL_LINK_STATUS, &linked_status);
        OM_GL_CHECK()
        if (linked_status == 0)
            {
                GLint infoLen = 0;
                glGetProgramiv(program_id_body_NoVig, GL_INFO_LOG_LENGTH, &infoLen);
                OM_GL_CHECK()
                    std::vector<char> infoLog(static_cast<size_t>(infoLen));
                glGetProgramInfoLog(program_id_body_NoVig, infoLen, nullptr, infoLog.data());
                OM_GL_CHECK()
                serr << "Error linking program:\n" << infoLog.data();
                glDeleteProgram(program_id_body_NoVig);
                OM_GL_CHECK()
                return serr.str();
            }
        return "";
    }
    std::string activateProgBackground(uint8_t text_num){
        glUseProgram(program_id_simple);
        OM_GL_CHECK()

        int location = glGetUniformLocation(program_id_simple, "s_texture");
        OM_GL_CHECK()
        assert(-1 != location);
        int texture_unit = text_num;//number of texture
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        OM_GL_CHECK()

        glUniform1i(location, 0 + texture_unit);
        OM_GL_CHECK()

        glEnable(GL_BLEND);
        OM_GL_CHECK()
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        OM_GL_CHECK()

        return "";
    }
    std::string activateProgBody(uint8_t text_num, glm::mat4 mat_in){
        // turn on rendering with just created shader program
        glUseProgram(program_id_body);
        OM_GL_CHECK()

        GLuint mum=glGetUniformLocation(program_id_body,"color_dub");
        glUniform1f(mum,colorDun);

        GLuint mim=glGetUniformLocation(program_id_body,"range_of_veg");
        glUniform1f(mim,range_of_veg);




        GLuint mem=glGetUniformLocation(program_id_body,"rot_matrix");
        OM_GL_CHECK()

        glUniformMatrix4fv(mem,1,GL_FALSE,glm::value_ptr(mat_in));
                OM_GL_CHECK()


        int location = glGetUniformLocation(program_id_body, "s_texture");
        OM_GL_CHECK()
        assert(-1 != location);
        int texture_unit = text_num;//number of texture
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        OM_GL_CHECK()

        glUniform1i(location, 0 + texture_unit);
        OM_GL_CHECK()

        glEnable(GL_BLEND);
        OM_GL_CHECK()
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        OM_GL_CHECK()

        return "";
    }
    std::string activateProgBodyNoVig(uint8_t text_num, glm::mat4 mat_in){
        // turn on rendering with just created shader program
        glUseProgram(program_id_body_NoVig);
        OM_GL_CHECK()

                GLuint mum=glGetUniformLocation(program_id_body,"color_dub");
                glUniform1f(mum,colorDun);



        GLuint mem=glGetUniformLocation(program_id_body_NoVig,"rot_matrix");
        OM_GL_CHECK()

        glUniformMatrix4fv(mem,1,GL_FALSE,glm::value_ptr(mat_in));
                OM_GL_CHECK()


        int location = glGetUniformLocation(program_id_body_NoVig, "s_texture");
        OM_GL_CHECK()
        assert(-1 != location);
        int texture_unit = text_num;//number of texture
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        OM_GL_CHECK()

        glUniform1i(location, 0 + texture_unit);
        OM_GL_CHECK()

        glEnable(GL_BLEND);
        OM_GL_CHECK()
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        OM_GL_CHECK()

        return "";
    }
    void render_triangle(const triangle& t){
        //vertexs
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                              &t.v[0].x);
        OM_GL_CHECK()

        glEnableVertexAttribArray(0);
        OM_GL_CHECK()
        //textures
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                             &t.v[0].tex_x);
        OM_GL_CHECK()
        glEnableVertexAttribArray(1);
        OM_GL_CHECK()

        glDrawArrays(GL_TRIANGLES, 0, 3);
        OM_GL_CHECK()

    }

    void render_background(){
       activateProgBackground(0);
       render_triangle(background1);
       render_triangle(background2);

    }
    void render_numbackground(u_int8_t textnum){
        activateProgBackground(textnum);
        render_triangle(background1);
        render_triangle(background2);

    }
    void render_texture(u_int8_t textnum){
        activateProgBody(textnum,bodyMove);
        render_triangle(body1);
        render_triangle(body2);
    }

    void render(){
        render_background();



        if(isvignette==true){
            activateProgBody(1,bodyMove);
            render_triangle(body1);
            render_triangle(body2);
        }else{
            activateProgBodyNoVig(1,bodyMove);
            render_triangle(body1);
            render_triangle(body2);
        }

        //imgui part
        imgui_newframe(window);
        imgui_window(bodyMove,buffMat,isvignette,&colorDun,&istimer,&range_of_veg);
        imgui_render();

        if(istimer==true){
            if(colorDun>=1.0f){
                colorDun=0.0f;
            }else{
                colorDun+=0.005;
            }
            if(range_of_veg>=1.0){
                range_of_veg=0.25f;
            }else{
                range_of_veg+=0.005;
            }

        }





    }

};
//tests for gl functions
static const char* source_to_strv(GLenum source)
{
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            return "API";
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            return "SHADER_COMPILER";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            return "WINDOW_SYSTEM";
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            return "THIRD_PARTY";
        case GL_DEBUG_SOURCE_APPLICATION:
            return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER:
            return "OTHER";
    }
    return "unknown";
}

static const char* type_to_strv(GLenum type)
{
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_MARKER:
            return "MARKER";
        case GL_DEBUG_TYPE_PUSH_GROUP:
            return "PUSH_GROUP";
        case GL_DEBUG_TYPE_POP_GROUP:
            return "POP_GROUP";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
    }
    return "unknown";
}
static const char* severity_to_strv(GLenum severity)
{
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            return "HIGH";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "MEDIUM";
        case GL_DEBUG_SEVERITY_LOW:
            return "LOW";
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            return "NOTIFICATION";
    }
    return "unknown";
}

static std::array<char, GL_MAX_DEBUG_MESSAGE_LENGTH> local_log_buff;

static void APIENTRY callback_opengl_debug(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, [[maybe_unused]] const void* userParam)
{
    // The memory formessageis owned and managed by the GL, and should onlybe
    // considered valid for the duration of the function call.The behavior of
    // calling any GL or window system function from within thecallback function
    // is undefined and may lead to program termination.Care must also be taken
    // in securing debug callbacks for use with asynchronousdebug output by
    // multi-threaded GL implementations.  Section 18.8 describes thisin further
    // detail.

    auto& buff{ local_log_buff };
    int   num_chars = std::snprintf(
        buff.data(), buff.size(), "%s %s %d %s %.*s\n", source_to_strv(source),
        type_to_strv(type), id, severity_to_strv(severity), length, message);

    if (num_chars > 0)
    {
        // TODO use https://en.cppreference.com/w/cpp/io/basic_osyncstream
        // to fix possible data races
        // now we use GL_DEBUG_OUTPUT_SYNCHRONOUS to garantie call in main
        // thread
        std::cerr.write(buff.data(), num_chars);
    }
}

