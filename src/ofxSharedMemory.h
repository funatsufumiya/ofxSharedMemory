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
        vector<float> readFloatArray();
        vector<double> readDoubleArray();
        bool isAvailable();
        bool isPersistent();
        std::string getName();
        size_t getSize();

        static std::pair<bool, std::shared_ptr<Reader>> tryCreate(const std::string name, const std::size_t bufferSize, const bool isPersistent) {
            auto reader = std::make_shared<Reader>(name, bufferSize, isPersistent);
            bool isAvailable = reader->isAvailable();
            if (!isAvailable) {
                return std::make_pair(false, nullptr);
            }
            return std::make_pair(true, reader);
        }

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
        void write(const vector<float>& data);
        void write(const vector<double>& data);

        bool isAvailable();
        bool isPersistent();
        std::string getName();
        size_t getSize();

        static std::pair<bool, std::shared_ptr<Writer>> tryCreate(const std::string name, const std::size_t bufferSize, const bool isPersistent) {
            auto writer = std::make_shared<Writer>(name, bufferSize, isPersistent);
            bool isAvailable = writer->isAvailable();
            if (!isAvailable) {
                return std::make_pair(false, nullptr);
            }
            return std::make_pair(true, writer);
        }
        
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
