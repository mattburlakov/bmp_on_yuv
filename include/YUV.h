#ifndef YUV_H
#define YUV_H

#include <iostream>
#include <fstream>
#include <thread>

//works only with width and height divisible by 2

uint8_t RGB2Y(uint8_t r, uint8_t g, uint8_t b)
{
    return ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
}

void BGR2YUV(uint8_t* f, const uint8_t* data, const int d_offsets[4], const int f_offsets[4], int origin, int width, int height){ //outputs data x3 bigger
    int area = width * height;
    int u_offset = area + (origin >> 2);
    int v_offset = area + (area >> 2) + (origin >> 2);

    uint8_t avR = 0;
    uint8_t avG = 0;
    uint8_t avB = 0;

    for (int x = 0; x < width; x += 2)
    {
        for(int i = 0; i < 4; i++){
            uint8_t r = data[origin * 3 + 2 + d_offsets[i]]; // BGR
            uint8_t g = data[origin * 3 + 1 + d_offsets[i]];
            uint8_t b = data[origin * 3 + 0 + d_offsets[i]];
            f[origin + f_offsets[i]] = RGB2Y(r, g, b);

            avR += r;
            avG += g;
            avB += b;
        }

        avR = avR >> 2; // get average
        avG = avG >> 2;
        avB = avB >> 2;

        f[u_offset++] = ((-38 * avR + -74 * avG + 112 * avB + 128) >> 8) + 128;
        f[v_offset++] = ((112 * avR + -94 * avG + -18 * avB + 128) >> 8) + 128;

        avR = 0;
        avG = 0;
        avB = 0;

        origin += 2; // get to next block
    }
    
}

class YUV{
    private: //return separation?
        uint8_t* frame;
        int byteSize = 0;

    public:
        YUV(uint8_t* data, int width, int height){
            byteSize = width * height;
            byteSize += byteSize >> 1;

            frame = new uint8_t[byteSize];

            for(int i = 0; i < byteSize; i++){ // debug
                frame[i] = 0xaa;
            }

            const int d_offsets[4] = {0, 3, width * 3, width * 3 + 3}; //offsets for 2x2 blocks of pixel data
            const int f_offsets[4] = {0, 1, width, width + 1};

            std::cout << "--Translating to YUV--\n";

            for(int i = 0; i < height; i += 2){ // add: send to queue
                BGR2YUV(frame, data, d_offsets, f_offsets, i*width, width, height); // i*width : scanline byte offset
            }

            std::cout << "\n--Translation complete--\n";
            //saveFile(frame);  //debug

        }

        void saveFile(uint8_t* fts){
            std::cout << "\n--Saving YUV data--\n";

            std::ofstream file("yuv_test.yuv", std::ios::out | std::ios::binary);

            if(file.is_open()){
                std::cout << "\n\t>File open\n";

                file.write((char *)fts, byteSize);

                std::cout<<"\t>File saved\n";
            }
            else{
                std::cout << "\n\t>Can't open file for YUV writing\n";
            }
            
            file.close();
        }

        uint8_t* getFrame(){
            return frame;
        }

        ~YUV() {

        }
};

#endif