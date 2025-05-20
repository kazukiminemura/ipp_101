#include <iostream>
#include <ipp.h>
#include <chrono>

int main() {
    const int width = 1024;
    const int height = 1024;
    const int kernelSize = 5;

    Ipp32f sigma = 1.5f;
    IppDataType dataType = ipp8u;
    IppiSize roiSize = { width, height };
    IppiBorderType borderType = ippBorderRepl; // Replicate border value for processing
    int numChannels = 1;
    
    // Allocate memory for source and destination images
    Ipp8u* pSrc = ippsMalloc_8u(width * height);
    Ipp8u* pDst = ippsMalloc_8u(width * height);
    for (int i = 0; i < width * height; ++i) pSrc[i] = rand() % 256;

    //
    // Initialize Gaussian filter parameters
    //
    // Create Gaussian kernel: Get specs and buffer size of Gaussian filter
    int specSize = 0, bufferSize = 0; // this is placeholder for specSize and bufferSize
    ippiFilterGaussianGetBufferSize(roiSize, kernelSize, dataType, numChannels, &specSize, &bufferSize);

    // Allocate buffer
    IppFilterGaussianSpec* pSpec = (IppFilterGaussianSpec*)ippsMalloc_8u(specSize);
    Ipp8u* pBuffer = ippsMalloc_8u(bufferSize);

    // Initialize filter
    IppStatus status = ippiFilterGaussianInit(roiSize, kernelSize, sigma, borderType, dataType, numChannels, pSpec, pBuffer);
    if (status != ippStsNoErr) {
        std::cout << "Init failed: " << status << std::endl;
        ippsFree(pSrc);
        ippsFree(pDst);
        ippsFree(pSpec);
        ippsFree(pBuffer);
        return -1;
    }

    // // Define ROI and step
    int srcStep = width;
    int dstStep = width;

    // Apply Gaussian blur
    auto start = std::chrono::high_resolution_clock::now();
    ippiFilterGaussianBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, pSpec, pBuffer);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "IPP Gaussian blur time: " << elapsed.count() * 1000 << " ms" << std::endl;

    // Free memory
    ippsFree(pSrc);
    ippsFree(pDst);
    ippsFree(pSpec);
    ippsFree(pBuffer);

    return 0;
}
