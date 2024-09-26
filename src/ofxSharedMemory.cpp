#include "ofxSharedMemory.h"

// this almost wrap libsharedmemory, but difference is use first byte as mutex lock

namespace ofxSharedMemory {
    
    Reader::Reader(const std::string& name, size_t size, bool isPersistent)
        : name(name), size(size), isPersistent_(isPersistent), isAvailable_(false)
    {
        auto res = lsm::SharedMemoryReadStream::tryCreate(name, size, isPersistent_);
        if (res.first) {
            readStream = res.second;
            isAvailable_ = true;

            // check is mutex lock is available
            bool isMutexLockNeedsToBeCreated = true;
            auto res2 = lsm::SharedMemoryReadStream::tryCreate(name + "_mutex_lock", 1, isPersistent_);
            if (res2.first) {
                mutexReadStream = res2.second;
                isMutexLockNeedsToBeCreated = false;
            }

            // create write stream for mutex lock
            auto res3 = lsm::SharedMemoryWriteStream::tryCreate(name + "_mutex_lock", 1, isPersistent_);
            
            if (res3.first) {
                mutexWriteStream = res3.second;

                if (isMutexLockNeedsToBeCreated) {
                    std::string mutex_data = {0};
                    mutexWriteStream->write(mutex_data);

                    // get reader
                    auto res4 = lsm::SharedMemoryReadStream::tryCreate(name + "_mutex_lock", 1, isPersistent_);
                    if (res4.first) {
                        mutexReadStream = res4.second;
                    } else {
                        ofLogError() << "Failed to create shared memory reader for mutex lock";
                        isAvailable_ = false;
                        readStream.reset();
                        mutexWriteStream.reset();
                    }
                }
            } else {
                ofLogError() << "Failed to create shared memory writer for mutex lock";
                isAvailable_ = false;
                readStream.reset();
            }
        }else {
            ofLogError() << "Failed to create shared memory reader";
        }
    }

    Reader::~Reader() {
        readStream.reset();
        mutexReadStream.reset();
        mutexWriteStream.reset();
    }

    std::string Reader::readString() {
        if (!isAvailable_) {
            ofLogError() << "Shared memory reader is not available";
            return "";
        }

        // read mutex
        while (true) {
            char mutex = mutexReadStream->readString()[0];
            if (mutex == 0) {
                break;
            }
            ofSleepMillis(1);
        }

        // lock mutex
        std::string mutex_data = {1};
        mutexWriteStream->write(mutex_data);

        // read data
        std::string data = readStream->readString();

        // unlock mutex
        mutex_data = {0};
        mutexWriteStream->write(mutex_data);

        return data;
    }

    ofBuffer Reader::readBytes() {
        if (!isAvailable_) {
            ofLogError() << "Shared memory reader is not available";
            return ofBuffer();
        }

        // read mutex
        while (true) {
            char mutex = mutexReadStream->readString()[0];
            if (mutex == 0) {
                break;
            }
            ofSleepMillis(1);
        }

        // lock mutex
        std::string mutex_data = {1};
        mutexWriteStream->write(mutex_data);

        // read data
        ofBuffer data = readStream->readBytes();

        // unlock mutex
        mutex_data = {0};
        mutexWriteStream->write(mutex_data);

        return data;
    }

    std::vector<float> Reader::readFloatArray() {
        if (!isAvailable_) {
            ofLogError() << "Shared memory reader is not available";
            return std::vector<float>();
        }

        // read mutex
        while (true) {
            char mutex = mutexReadStream->readString()[0];
            if (mutex == 0) {
                break;
            }
            ofSleepMillis(1);
        }

        // lock mutex
        std::string mutex_data = {1};
        mutexWriteStream->write(mutex_data);

        // read data
        std::vector<float> data = readStream->readFloatArray();

        // unlock mutex
        mutex_data = {0};
        mutexWriteStream->write(mutex_data);

        return data;
    }

    std::vector<double> Reader::readDoubleArray() {
        if (!isAvailable_) {
            ofLogError() << "Shared memory reader is not available";
            return std::vector<double>();
        }

        // read mutex
        while (true) {
            char mutex = mutexReadStream->readString()[0];
            if (mutex == 0) {
                break;
            }
            ofSleepMillis(1);
        }

        // lock mutex
        std::string mutex_data = {1};
        mutexWriteStream->write(mutex_data);

        // read data
        std::vector<double> data = readStream->readDoubleArray();

        // unlock mutex
        mutex_data = {0};
        mutexWriteStream->write(mutex_data);

        return data;
    }

    bool Reader::isAvailable() {
        return isAvailable_;
    }

    bool Reader::isPersistent() {
        return isPersistent_;
    }

    std::string Reader::getName() {
        return name;
    }

    size_t Reader::getSize() {
        return size;
    }

    Writer::Writer(const std::string& name, size_t size, bool isPersistent)
        : name(name), size(size), isPersistent_(isPersistent), isAvailable_(false)
    {
        auto res = lsm::SharedMemoryWriteStream::tryCreate(name, size, isPersistent_);
        if (res.first) {
            writeStream = res.second;
            isAvailable_ = true;

            // check is mutex lock is available
            bool isMutexLockNeedsToBeCreated = true;
            auto res2 = lsm::SharedMemoryReadStream::tryCreate(name + "_mutex_lock", 1, isPersistent_);
            if (res2.first) {
                mutexReadStream = res2.second;
                isMutexLockNeedsToBeCreated = false;
            }

            // create write stream for mutex lock
            auto res3 = lsm::SharedMemoryWriteStream::tryCreate(name + "_mutex_lock", 1, isPersistent_);
            
            if (res3.first) {
                mutexWriteStream = res3.second;

                if (isMutexLockNeedsToBeCreated) {
                    std::string mutex_data = {0};
                    mutexWriteStream->write(mutex_data);

                    // get reader
                    auto res4 = lsm::SharedMemoryReadStream::tryCreate(name + "_mutex_lock", 1, isPersistent_);
                    if (res4.first) {
                        mutexReadStream = res4.second;
                    } else {
                        ofLogError() << "Failed to create shared memory reader for mutex lock";
                        isAvailable_ = false;
                        mutexWriteStream.reset();
                        writeStream.reset();
                    }
                }
            } else {
                ofLogError() << "Failed to create shared memory writer for mutex lock";
                isAvailable_ = false;
                writeStream.reset();
            }
        }else {
            ofLogError() << "Failed to create shared memory writer";
        }
    }

    Writer::~Writer() {
        writeStream.reset();
        mutexReadStream.reset();
        mutexWriteStream.reset();
    }

    void Writer::write(const std::string& data) {
        if (!isAvailable_) {
            ofLogError() << "Shared memory writer is not available";
            return;
        }

        // read mutex
        while (true) {
            char mutex = mutexReadStream->readString()[0];
            if (mutex == 0) {
                break;
            }
            ofSleepMillis(1);
        }

        // lock mutex
        std::string mutex_data = {1};
        mutexWriteStream->write(mutex_data);

        // write data
        writeStream->write(data);

        // unlock mutex
        mutex_data = {0};
        mutexWriteStream->write(mutex_data);
    }

    void Writer::write(const ofBuffer& data) {
        if (!isAvailable_) {
            ofLogError() << "Shared memory writer is not available";
            return;
        }

        // read mutex
        while (true) {
            char mutex = mutexReadStream->readString()[0];
            if (mutex == 0) {
                break;
            }
            ofSleepMillis(1);
        }

        // lock mutex
        std::string mutex_data = {1};
        mutexWriteStream->write(mutex_data);

        // write data
        writeStream->write(data);

        // unlock mutex
        mutex_data = {0};
        mutexWriteStream->write(mutex_data);
    }

    void Writer::write(const std::vector<float>& data) {
        if (!isAvailable_) {
            ofLogError() << "Shared memory writer is not available";
            return;
        }

        // read mutex
        while (true) {
            char mutex = mutexReadStream->readString()[0];
            if (mutex == 0) {
                break;
            }
            ofSleepMillis(1);
        }

        // lock mutex
        std::string mutex_data = {1};
        mutexWriteStream->write(mutex_data);

        // write data
        writeStream->write(data);

        // unlock mutex
        mutex_data = {0};
        mutexWriteStream->write(mutex_data);
    }

    void Writer::write(const std::vector<double>& data) {
        if (!isAvailable_) {
            ofLogError() << "Shared memory writer is not available";
            return;
        }

        // read mutex
        while (true) {
            char mutex = mutexReadStream->readString()[0];
            if (mutex == 0) {
                break;
            }
            ofSleepMillis(1);
        }

        // lock mutex
        std::string mutex_data = {1};
        mutexWriteStream->write(mutex_data);

        // write data
        writeStream->write(data);

        // unlock mutex
        mutex_data = {0};
        mutexWriteStream->write(mutex_data);
    }

    bool Writer::isAvailable() {
        return isAvailable_;
    }

    bool Writer::isPersistent() {
        return isPersistent_;
    }

    std::string Writer::getName() {
        return name;
    }

    size_t Writer::getSize() {
        return size;
    }
    
} // namespace ofxSharedMemory
