#include <iostream>
#include <chrono>
#include <math.h>
#include <cuda_runtime.h>

__global__ void eval_polynomial(float* array, float* poly, int degree, int n){
    int idx = blockIdx.x * blockDim.x + threadIdx.x; // copied from test + notes
    if (idx<n) {
      float x = array[idx];
      float val = 0.0;
      float exponent = 1.0;
      for (int i=0; i<=degree; i++){
        val += poly[i]*exponent;
        exponent*=x;
      }
    array[idx] = val;
  }
} 

//void polynomial_expansion (float* poly, int degree, int n, float* array, int threadsPerBlock) {
void polynomial_expansion (float* poly, int degree, int n, float* array) {
  //TODO: Write code to use the GPU here!
  //code should write the output back to array
  float *gpu_array, *gpu_poly;
  cudaMalloc(&gpu_array, n*sizeof(float));
  cudaMalloc(&gpu_poly, (degree+1)*sizeof(float));
  cudaMemcpy(gpu_array, array, n*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(gpu_poly, poly, (degree+1)*sizeof(float), cudaMemcpyHostToDevice);

  int threadsPerBlock = 512;
  int blocksPerGrid = (n + threadsPerBlock - 1)/threadsPerBlock; 

  eval_polynomial<<<blocksPerGrid, threadsPerBlock>>>(gpu_array, gpu_poly, degree, n);
  cudaDeviceSynchronize();
  cudaMemcpy(array, gpu_array, n*sizeof(float), cudaMemcpyDeviceToHost);
  cudaFree(gpu_array);
  cudaFree(gpu_poly);
}

int main (int argc, char* argv[]) {
  //if (argc < 4) { // only for question 2
  if (argc < 3) {
     std::cerr<<"usage: "<<argv[0]<<" n degree"<<std::endl;
     //std::cerr<<"usage: "<<argv[0]<<" n degree threadsPerBlock"<<std::endl;
     return -1;
  }

  int n = std::stoi(argv[1]); //TODO: atoi is an unsafe function
  int degree = std::stoi(argv[2]);
  //int threadsPerBlock = std::stoi(argv[3]); // only for question 2
  int nbiter = 1;

  float* array = new float[n];
  float* poly = new float[degree+1];
  for (int i=0; i<n; ++i)
    array[i] = 1.;

  for (int i=0; i<degree+1; ++i)
    poly[i] = 1.;

  std::chrono::time_point<std::chrono::system_clock> begin, end;
  begin = std::chrono::system_clock::now();
  
  for (int iter = 0; iter<nbiter; ++iter)
    polynomial_expansion (poly, degree, n, array);
    //polynomial_expansion (poly, degree, n, array, threadsPerBlock);



  end = std::chrono::system_clock::now();
  std::chrono::duration<double> totaltime = (end-begin)/nbiter;

  {
    bool correct = true;
    int ind;
    for (int i=0; i< n; ++i) {
      if (fabs(array[i]-(degree+1))>0.01) {
        correct = false;
	ind = i;
      }
    }
    if (!correct)
      std::cerr<<"Result is incorrect. In particular array["<<ind<<"] should be "<<degree+1<<" not "<< array[ind]<<std::endl;
  }
  
  std::cerr<<array[0]<<std::endl;
  std::cout<<n<<" "<<degree<<" "<<totaltime.count()<<std::endl;

  delete[] array;
  delete[] poly;

  return 0;
}
