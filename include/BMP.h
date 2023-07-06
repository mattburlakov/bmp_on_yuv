#ifndef BMP_H
#define BMP_H

#include <iostream>
#include <fstream>

#include "YUV.h"

#pragma pack(push, 1) //change compiler padding

struct BMP_File_Header {
    uint16_t signature = 0;

    uint32_t filesize  = 0;

    uint16_t reserved1 = 0;
    uint16_t reserved2 = 0;

    uint32_t bitOffset = 0;
};

struct BMP_Info_Header {
    uint32_t headerSize  = 0;

    int32_t width        = 0;
    int32_t height       = 0;

    uint16_t planes      = 0;
    uint16_t bitCount    = 0;

    uint32_t compression = 0;
    uint32_t size_image  = 0;
};

class Bitmap {
    private:
        BMP_File_Header     file_header;
        BMP_Info_Header     info_header;

        uint8_t* data;

        uint8_t* frame;

    public:
        Bitmap(const char* path){
            std::cout << ">Opening stream...\n";

            std::ifstream BMP_stream;
            BMP_stream.open(path, std::ios_base::binary);
            
            if(BMP_stream.is_open()){
                std::cout << "--Loading BMP image--\n\n";

                std::cout << "\t>Stream open\n";

                //read file header
                BMP_stream.read((char *)&file_header, sizeof(BMP_File_Header));
                std::cout << "\t>Got FILE HEADER\n\n";
                
                //read info header
                BMP_stream.read((char *)&info_header, sizeof(BMP_Info_Header));
                std::cout << "\t>Bit depth: " << info_header.bitCount << "\n\n";
                std::cout << "\t>Got INFO HEADER\n";

                if(info_header.headerSize == 12){throw std::runtime_error("CORE version is unsupported");}
                if(info_header.bitCount < 24)   {throw std::runtime_error("Bit depth less than 24 is unsupported");}
                if(info_header.compression != 0 && info_header.compression != 3 && info_header.compression != 6){throw std::runtime_error("Compression is unsupported");}
                
                //create pixel data container, for test_1.bmp hexdump shows size_image 0 at 0x22
                int sizeImg = 0;
                (info_header.size_image != 0) ? sizeImg = info_header.size_image : sizeImg = info_header.width * info_header.height * (info_header.bitCount / 8);
                data = new uint8_t[sizeImg];

                //jump to pixel data
                //add: padding removal
                BMP_stream.seekg(file_header.bitOffset, BMP_stream.beg);

                std::cout << "\n\t>Storing pixel data...\n";
                BMP_stream.read((char *)data, sizeImg);
                std::cout << "\t>Pixel data stored\n";

                std::cout << "\n--Finished loading BMP image--\n";

                std::cout << "Flipping image...\n"; //flip image so that the origin point is in upper left
                flipData();
                std::cout << "Image flipped\n";

                //translating to YUV
                YUV YUV_frame(data, info_header.width, info_header.height);
                frame = YUV_frame.getFrame();

#if 0
                //testing
                std::ofstream ppmFile("test.ppm");
                int area = info_header.width * std::abs(info_header.height);
                int c    = info_header.bitCount / 8;
                ppmFile << "P3\n" << info_header.width << ' ' << info_header.height << "\n255\n";

                for(int i = 0; i < area*3; i++){
                    ppmFile     << (int)data[(c * i + 2)]
                                << ' '
                                << (int)data[(c * i + 1)]
                                << ' '
                                << (int)data[(c * i + 0)]
                                << '\n';
                }

                ppmFile.close();
                //testing
#endif

            }
            else{
                std::cout << "Unable to open file at specified path\n";
            }

            BMP_stream.close();
        }

        void flipData(){ //flip data to match conventional format
            int size = info_header.width * info_header.bitCount / 8;
            int area = info_header.width * info_header.height * info_header.bitCount / 8;
            uint8_t* tmp = new uint8_t[size];

            for(int i = 0; i < info_header.height / 2; i++){
              fillDataFromPoint(tmp, data, 0, size * i, size);
              fillDataFromPoint(data, data, size * i, area - size * (i + 1), size);
              fillDataFromPoint(data, tmp, area - size * (i + 1), 0, size);
            }
        }

        void fillDataFromPoint(uint8_t* dst, uint8_t* src, int d_point, int s_point, int size){
            for(int i = 0; i < size; i++){
                dst[i + d_point] = src[s_point + i];
            }
        }

        int getWidth(){
            return info_header.width;
        }

        int getHeight(){
            return info_header.height;
        }

        uint8_t* getYUV(){
            return frame;
        }

        ~Bitmap() {
            delete data;
            delete frame;
        }
};

#pragma pack(pop)

#endif