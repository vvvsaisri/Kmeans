#define CL_HPP_CL_1_2_DEFAULT_BUILD
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY 1

#include <CL/cl2.hpp>

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>
#include <string>
#include <filesystem>

#include "kmeans_birch_header.h"

using namespace std;

namespace {

const std::vector<std::string> kRawSearchOrder = {
    "/home/ramu/workspace/kmeans_birch/data/test_image.raw",   // user-provided absolute path
    "/workspace/kmeans_birch/data/test_image.raw",             // container-mounted path
    "../data/test_image.raw",
    "../../data/test_image.raw",
    "./data/test_image.raw",
    "./test_image.raw"
};

// Simple function to generate test pattern if image file not found
void generate_test_pattern(unsigned char *data, unsigned int height, unsigned int width) {
    for (unsigned int i = 0; i < height; i++) {
        for (unsigned int j = 0; j < width; j++) {
            int idx = (i * width + j) * DIM;
            // Generate a simple gradient pattern
            data[idx + 0] = (unsigned char)((i * 255) / height);      // B
            data[idx + 1] = (unsigned char)((j * 255) / width);       // G
            data[idx + 2] = (unsigned char)(((i + j) * 255) / (height + width)); // R
        }
    }
}

bool load_raw_image(unsigned char *dst, size_t size_in_bytes) {
    for (const auto& path : kRawSearchOrder) {
        std::ifstream img(path, std::ifstream::binary);
        if (!img.good()) {
            std::cout << "INFO: raw image not accessible at " << path << std::endl;
            continue;
        }
        img.seekg(0, img.end);
        size_t file_size = static_cast<size_t>(img.tellg());
        img.seekg(0, img.beg);
        if (file_size < size_in_bytes) {
            std::cout << "WARNING: raw file '" << path << "' has only "
                      << file_size << " bytes; expected " << size_in_bytes << std::endl;
            continue;
        }
        img.read(reinterpret_cast<char*>(dst), size_in_bytes);
        img.close();
        std::cout << "Loaded raw image: " << path << std::endl;
        return true;
    }
    return false;
}

void ensure_executable_directory_cwd(const char *argv0) {
    std::error_code ec;
    std::filesystem::path exe_path = std::filesystem::canonical(argv0, ec);
    if (ec) {
        std::cout << "WARNING: Unable to resolve executable path from '" << argv0
                  << "': " << ec.message() << std::endl;
        return;
    }
    std::filesystem::path exe_dir = exe_path.parent_path();
    std::filesystem::current_path(exe_dir, ec);
    if (ec) {
        std::cout << "WARNING: Failed to change working directory to "
                  << exe_dir << ": " << ec.message() << std::endl;
    } else {
        std::cout << "Working directory set to executable directory: "
                  << exe_dir << std::endl;
    }
}

bool write_ppm(const std::string &filename, unsigned char *data) {
    std::ofstream ppm(filename, std::ios::binary);
    if (!ppm.good()) {
        std::cout << "ERROR: Unable to open output file " << filename << std::endl;
        return false;
    }
    ppm << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
    for (unsigned int i = 0; i < HEIGHT; ++i) {
        for (unsigned int j = 0; j < WIDTH; ++j) {
            int idx = (i * WIDTH + j) * DIM;
            ppm.write(reinterpret_cast<char*>(&data[idx + 2]), 1); // R
            ppm.write(reinterpret_cast<char*>(&data[idx + 1]), 1); // G
            ppm.write(reinterpret_cast<char*>(&data[idx + 0]), 1); // B
        }
    }
    ppm.close();
    std::cout << "Wrote image: " << filename << std::endl;
    return true;
}

} // namespace

int main(int argc, char* argv[])
{
  int status = 0;

  unsigned int DATA_SIZE = HEIGHT * WIDTH * DIM;
  size_t size_in_bytes = DATA_SIZE * sizeof(unsigned char);

  std::cout << "Image size: " << HEIGHT << "x" << WIDTH << std::endl;
  std::cout << "Data size in bytes = " << size_in_bytes << std::endl;

  if(argc != 2) {
    std::cout << "Usage: " << argv[0] << " <xclbin>" << std::endl;
    return EXIT_FAILURE;
  }

  ensure_executable_directory_cwd(argv[0]);

  char* xclbinFilename = argv[1];
  std::vector<cl::Device> devices;
  cl::Device device;
  std::vector<cl::Platform> platforms;
  bool found_device = false;
  cl::Platform::get(&platforms);
  for(size_t i = 0; (i < platforms.size() ) & (found_device == false) ;i++){
    cl::Platform platform = platforms[i];
    std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();
    if ( platformName == "Xilinx"){
      devices.clear();
      platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
      if (devices.size()){
        device = devices[0];
        found_device = true;
        break;
      }
    }
  }
  if (found_device == false){
    std::cout << "Error: Unable to find Target Device "
             << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    return EXIT_FAILURE;
  }

  cl::Context context(device);
  cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);

  std::cout << "Loading: '" << xclbinFilename << "'\n";
  std::ifstream bin_file(xclbinFilename, std::ifstream::binary);
  bin_file.seekg (0, bin_file.end);
  unsigned nb = bin_file.tellg();
  bin_file.seekg (0, bin_file.beg);
  char *buf = new char [nb];
  bin_file.read(buf, nb);
  cl::Program::Binaries bins;
  bins.push_back({buf,nb});
  devices.resize(1);
  cl::Program program(context, devices, bins);
  cl::Kernel krnl_kmeans_birch(program,"kmeans_birch_kernel");

  cl::Buffer buffer_in(context,  CL_MEM_READ_ONLY, size_in_bytes);
  cl::Buffer buffer_kmeans_out(context, CL_MEM_WRITE_ONLY, size_in_bytes);
  cl::Buffer buffer_birch_out(context, CL_MEM_WRITE_ONLY, size_in_bytes);

  int narg=0;
  krnl_kmeans_birch.setArg(narg++, buffer_in);
  krnl_kmeans_birch.setArg(narg++, buffer_kmeans_out);
  krnl_kmeans_birch.setArg(narg++, buffer_birch_out);
  krnl_kmeans_birch.setArg(narg++, (unsigned int)HEIGHT);
  krnl_kmeans_birch.setArg(narg++, (unsigned int)WIDTH);

  unsigned char *ptr_in = (unsigned char *) q.enqueueMapBuffer (buffer_in, CL_TRUE, CL_MAP_WRITE, 0, size_in_bytes);
  unsigned char *ptr_kmeans_out = (unsigned char *) q.enqueueMapBuffer (buffer_kmeans_out, CL_TRUE, CL_MAP_READ, 0, size_in_bytes);
  unsigned char *ptr_birch_out = (unsigned char *) q.enqueueMapBuffer (buffer_birch_out, CL_TRUE, CL_MAP_READ, 0, size_in_bytes);

  bool image_loaded = load_raw_image(ptr_in, size_in_bytes);
  if (!image_loaded) {
    std::cout << "Falling back to synthetic test pattern." << std::endl;
    std::cout << "To use your own test image, convert it with:" << std::endl;
    std::cout << "  convert test_image.jpg -resize 493x612 -depth 8 rgb:test_image.raw" << std::endl;
    std::cout << "and place it under /workspace/kmeans_birch/data or ./data" << std::endl;
    generate_test_pattern(ptr_in, HEIGHT, WIDTH);
  }

  std::cout << "Starting kernel execution..." << std::endl;

  q.enqueueMigrateMemObjects({buffer_in}, 0/* 0 means from host*/);
  q.enqueueTask(krnl_kmeans_birch);
  q.enqueueMigrateMemObjects({buffer_kmeans_out, buffer_birch_out}, CL_MIGRATE_MEM_OBJECT_HOST);
  q.finish();

  std::cout << "Kernel execution completed." << std::endl;

  write_ppm("kmeans_output.ppm", ptr_kmeans_out);
  write_ppm("birch_output.ppm", ptr_birch_out);

  std::cout << "\nOutput files location: Current working directory" << std::endl;
  std::cout << "  - kmeans_output.ppm (OpenCL K-means)" << std::endl;
  std::cout << "  - birch_output.ppm  (OpenCL BIRCH)" << std::endl;
  std::cout << "Bye kmeans_birch" << std::endl;

  q.enqueueUnmapMemObject(buffer_in, ptr_in);
  q.enqueueUnmapMemObject(buffer_kmeans_out, ptr_kmeans_out);
  q.enqueueUnmapMemObject(buffer_birch_out, ptr_birch_out);
  q.finish();

  delete[] buf;
  return status;
}

