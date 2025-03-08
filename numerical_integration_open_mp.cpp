#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <omp.h>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

float numerical_integration(int functionid, float lower_bound, float upper_bound, int num_points, int intensity) {
    // Calculates equation given in HW
    float sum_value = 0.0;    
    // initializing for loop value
    float initial_val = (upper_bound - lower_bound) / num_points;

    #pragma omp parallel for reduction(+:sum_value) schedule(dynamic)

    for (int i = 0; i < num_points; i++) {
        // goes from 0 to n-1
        float x = lower_bound + (i + 0.5) * initial_val;
        // everything within the parentheses, excluding "intensity"
        sum_value += f1(x, intensity);
        // adds all summation terms
    }
    return sum_value * initial_val;
}
  
int main(int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }
  
    int functionid = std::stoi(argv[1]);
    float lower_bound = std::stod(argv[2]);
    float upper_bound = std::stod(argv[3]);
    int num_points = std::stoi(argv[4]);
    int intensity = std::stoi(argv[5]);
    int num_threads = std::stoi(argv[6]);
    omp_set_num_threads(num_threads);


    // start timing
    auto start_time = std::chrono::high_resolution_clock::now();
    // run function
    float final = numerical_integration(functionid, lower_bound, upper_bound, num_points, intensity);
    // end timing
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed_time = end_time - start_time;

    std::cout << final << std::endl;
    std::cerr << elapsed_time.count() << std::endl;    

  return 0;
}
