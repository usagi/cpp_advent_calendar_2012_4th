#define BLAND_NAME WonderRabbitProject
#define APP_NAME   particle_system_tester
#include "particle_system.hxx"

#include <iostream>

int main(){
  
  using WonderRabbitProject::particle_system_tester::particle_system;

  particle_system ps(
    {1920, 1080},
    { 160,   90}
  );

  for(size_t c = 0; c < 1000; ++c){
    std::cout << c << std::endl << "\x1b[1A";
    ps.update();
  }

}

