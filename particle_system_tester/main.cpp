#define BLAND_NAME WonderRabbitProject
#define APP_NAME   particle_system_tester
#include "particle_system.hxx"

#include <iostream>

int main(int ac, char** av){

  if(ac < 2){
    std::cerr << "Usage: " << av[0] << " num_of_loop\n";
    return -1;
  }

  const size_t num_of_loop = std::atoi(av[1]);
  
  using WonderRabbitProject::particle_system_tester::particle_system;

  particle_system ps(
    {1920, 1080},
    { 160,   90}
  );

  for(size_t c = 0; c < num_of_loop; ++c){
    std::cout << c << std::endl << "\x1b[1A";
    ps.update();
  }

}

