#pragma once

#include "ofMain.h"

#include "libsharedmemory.hpp"

// this almost wrap libsharedmemory, but difference is use first byte as mutex lock

namespace ofxSharedMemory {
    
    class Reader {
    public:
        Reader(const std::string& name, size_t size, bool isPersistent = false);
        ~Reader();
        
        std::string readString();
        ofBuffer readBytes();
        bool isAvailable();
        bool isPersistent();
        std::string getName();
        size_t getSize();

    private:
        std::string name;
        size_t size;
        bool isPersistent_;
        bool isAvailable_;

        std::shared_ptr<lsm::SharedMemoryReadStream> readStream;
        std::shared_ptr<lsm::SharedMemoryWriteStream> mutexWriteStream; // for mutex lock
        std::shared_ptr<lsm::SharedMemoryReadStream> mutexReadStream; // for mutex lock
    };

    class Writer {
    public:
        Writer(const std::string& name, size_t size, bool isPersistent = false);
        ~Writer();
        
        void write(const std::string& data);
        void write(const ofBuffer& data);

        bool isAvailable();
        bool isPersistent();
        std::string getName();
        size_t getSize();
        
    private:
        std::string name;
        size_t size;
        bool isPersistent_;
        bool isAvailable_;
        
        std::shared_ptr<lsm::SharedMemoryWriteStream> writeStream;
        std::shared_ptr<lsm::SharedMemoryWriteStream> mutexWriteStream; // for mutex lock
        std::shared_ptr<lsm::SharedMemoryReadStream> mutexReadStream; // for mutex lock
    };

} // namespace ofxSharedMemory
