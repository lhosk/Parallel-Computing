#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <pthread.h>

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

struct ThreadData {
    int functionid;
    float lower_bound;
    float upper_bound;
    int num_points;
    int intensity;
    int start;
    int end;
    float partial_sum;
};

void* thread_integration(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    float initial_val = (data->upper_bound - data->lower_bound) / data->num_points;
    data->partial_sum = 0.0;

    for (int i = data->start; i < data->end; i++) {
        float x = data->lower_bound + (i + 0.5) * initial_val;
        data->partial_sum += f1(x, data->intensity);
    }
    pthread_exit(nullptr);
}

float numerical_integration(int functionid, float lower_bound, float upper_bound, int num_points, int intensity, int num_threads) {
    float total_sum = 0.0;
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    int ppt = num_points / num_threads;
     
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].lower_bound = lower_bound;
        thread_data[i].upper_bound = upper_bound;
        thread_data[i].num_points = num_points;
        thread_data[i].intensity = intensity;
        thread_data[i].start = i * ppt;
        thread_data[i].end = (i == num_threads - 1) ? num_points : thread_data[i].start + ppt;
        pthread_create(&threads[i], nullptr, thread_integration, &thread_data[i]);
    }
    
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
        total_sum += thread_data[i].partial_sum;
    }

    float initial_val = (upper_bound - lower_bound) / num_points;
    return total_sum * initial_val;
}

int main(int argc, char* argv[]) {
  
    int functionid = std::stoi(argv[1]);
    float lower_bound = std::stod(argv[2]);
    float upper_bound = std::stod(argv[3]);
    int num_points = std::stoi(argv[4]);
    int intensity = std::stoi(argv[5]);
    int num_threads = std::stoi(argv[6]);

    // start timing
    auto start_time = std::chrono::high_resolution_clock::now();
    // run function
    float result = numerical_integration(functionid, lower_bound, upper_bound, num_points, intensity, num_threads);
    // end timing
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed_time = end_time - start_time;

    std::cout << result << std::endl;
    std::cerr << elapsed_time.count() << std::endl;    

    return 0;
}
