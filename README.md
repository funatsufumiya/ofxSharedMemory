# ofxSharedMemory

Shared Memory addon for OpenFrameworks 0.11/0.12, using [kyr0/libsharedmemory](https://github.com/kyr0/libsharedmemory).

## Usage

- ofApp.h
    
    ```cpp
    std::shared_ptr<lsm::SharedMemoryReadStream> reader;
	std::shared_ptr<lsm::SharedMemoryWriteStream> writer;
    ```

- ofApp.cpp

    ```cpp
    std::string dataToTransfer = "Hello World!";
        
    writer = std::make_shared<SharedMemoryWriteStream>("strPipe", 65535, false); // name, size, isPersistent
    writer->write(dataToTransfer);
    ofLogNotice() << "Data wrote: " << dataToTransfer;

    reader = std::make_shared<SharedMemoryReadStream>("strPipe", 65535, false);
    std::string data = reader->readString();
    ofLogNotice() << "Data read: " << data;
    ```
