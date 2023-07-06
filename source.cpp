#include <iostream>

#include "include/BMP.h"

void frameOnFrame(uint8_t* dst, uint8_t* src, int dW, int dH, int dU_offset, int dV_offset, int sW, int sH, int sU_offset, int sV_offset){
    int c = 0;
    for(int i = 0; i < sH; i++){
        for(int j = 0; j < sW; j++){
            dst[j + dW * i] = src[c++]; //pos good

            if(j < sW / 2 && i % 2){
                dst[dU_offset + j + (i >> 1) * (dW >> 1)] = src[sU_offset++]; //pos good
                dst[dV_offset + j + (i >> 1) * (dW >> 1)] = src[sV_offset++];
            }
        }
    }
}

void imageOnVideo(const char* path, int frames, int vW, int vH, uint8_t* image, int iW, int iH){
    int v_u_offset = vW * vH;
    int v_v_offset = v_u_offset + (v_u_offset >> 2);
    int v_byteSize = v_u_offset + (v_u_offset >> 1);

    int i_u_offset = iW * iH;
    int i_v_offset = i_u_offset + (i_u_offset >> 2);

    std::fstream YUV_stream;
    std::cout << "\n--Opening video file--\n\n";
    YUV_stream.open(path, std::ios::out | std::ios::in | std::ios::binary);

    if(YUV_stream.is_open()){
        std::cout << "\t>File open\n";

        uint8_t* vFrame = new uint8_t[v_byteSize];
        int cF = 0;

        while(cF <= frames){
            YUV_stream.seekg(v_byteSize * cF, YUV_stream.beg); //set cursor
            YUV_stream.read((char *)vFrame, v_byteSize);
            //
            frameOnFrame(vFrame, image, vW, vH, v_u_offset, v_v_offset, iW, iH, i_u_offset, i_v_offset);
            //
            YUV_stream.seekg(v_byteSize * cF, YUV_stream.beg); //set cursor back for writing
            YUV_stream.write((char *)vFrame, v_byteSize);
            cF++;
        }

        std::cout << "\t>Finished imposing image\n";
    }
    else{
        std::cout << "\t>Unable to open file\n"; 
    }

    YUV_stream.close();
}

int main()
{
    //YUV420 video info
    int width   = 352;
    int height  = 288;
    int frames  = 299;

    // open bitmap, get and convert data, close | translate to ppm for testing
    Bitmap map("test_2.bmp");

    // open YUV, impose data, close
    imageOnVideo("bowing_cif.yuv", frames, width, height, map.getYUV(), map.getWidth(), map.getHeight());

    return 0;
}