#include "engine.hxx"

int main(int, char **){

    engine eng;


    //initialization of opengl programs
    std::string error = eng.initProgramBackground();//init opengl programs
    if (!error.empty())
    {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }
    error = eng.initProgramBody();//init opengl programs
    if (!error.empty())
    {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }
    //loading textures
    if (!eng.load_texture("background.png",0)){
        std::cout<<"failed to load chernarus2.png"<<std::endl;
        return EXIT_FAILURE;
    }
    if (!eng.load_texture("logo2.png",1)){
        std::cout<<"failed to load bip.png"<<std::endl;
        return EXIT_FAILURE;
    }


    eng.initData();
    eng.initfTriagBackground();


    bool q=true;
    while(q){
        q=eng.event();

        eng.render();
        eng.swapBuffers();
        //std::cout<<"sas"<<std::endl;
    }
  
}
