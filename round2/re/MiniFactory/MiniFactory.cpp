#include <bits/stdc++.h>

const int MAGIC_SIZE = 42;
const unsigned char MAGIC[MAGIC_SIZE] = {
    0x08, 0x8B, 0xDE, 0xBF, 0x9D, 0xA9, 0x04, 0x12, 0x8B, 0x12, 
    0x12, 0x9A, 0x62, 0xA9, 0x51, 0x47, 0xEA, 0xEA, 0x30, 0x8B, 
    0x26, 0x02, 0x4B, 0x77, 0x32, 0x74, 0x41, 0x7D, 0x4A, 0x7C, 
    0x4F, 0x0B, 0x4F, 0x7B, 0x05, 0x6C, 0x1B, 0x7D, 0x0B, 0x63, 
    0x14, 0x70
};

struct FactoryData {
    size_t size;
    unsigned char *data;

    FactoryData() : size(0), data(nullptr) {

    }

    FactoryData(const unsigned char *data, size_t size) {
        this->size = 0;
        this->data = nullptr;

        if (size) {
            this->size = size;
            this->data = (unsigned char *)malloc((size >> 3) + 2);
            memset(this->data, 0, (size >> 3) + 2);
            memcpy(this->data, data, size >> 3);

            if (size & 7) {
                this->data[size >> 3] = ((1 << (size & 7)) - 1) & data[size >> 3];
            }
        }
    }

    bool PopData(unsigned char **out) {
        if (data == nullptr) {
            return false;
        }
        
        *out = data;
        data = nullptr;
        
        return true;
    }

    bool PushData(unsigned char *in) {
        if (data != nullptr) {
            return false;
        }

        data = in;
        return true;
    }
};

class VirtualFactory {
public:
    VirtualFactory() {
        numFactoryData = 0;
        numNextFactoryData = 0;
        factoryData[0] = nullptr;
        factoryData[1] = nullptr;
        factoryData[2] = nullptr;
        factoryData[3] = nullptr;
    }

    VirtualFactory(int numFactoryData, int numNextFactoryData) {
        this->numFactoryData = numFactoryData;
        this->numNextFactoryData = numNextFactoryData;
        factoryData[0] = (FactoryData **)malloc(sizeof(FactoryData **) * numFactoryData);
        memset(factoryData[0], 0, sizeof(FactoryData **) * numFactoryData);
        factoryData[1] = (FactoryData **)malloc(sizeof(FactoryData **) * numNextFactoryData);
        memset(factoryData[1], 0,sizeof(FactoryData **) * numNextFactoryData);
        factoryData[2] = (FactoryData **)malloc(sizeof(FactoryData **) * numFactoryData);
        memset(factoryData[2], 0, sizeof(FactoryData **) * numFactoryData);
        factoryData[3] = (FactoryData **)malloc(sizeof(FactoryData **) * numNextFactoryData);
        memset(factoryData[3], 0, sizeof(FactoryData **) * numNextFactoryData);
    }

    ~VirtualFactory() {
        free(factoryData[0]);
        free(factoryData[1]);
        free(factoryData[2]);
        free(factoryData[3]);
    }

    virtual void Process(unsigned char *data, size_t size) {
        // overridden
    }

    virtual void Work() {
        size_t size = 0;
        unsigned char *data = GetFactoryData(size);

        if (size) {
            Process(data, size);
            GiveDataToNextFactory(data, size);
            free(data);
        }
    }

    bool PopFactoryData(int id, FactoryData **out) {
        return factoryData[0][id]->PopData((unsigned char **)out);
    }

    bool PushNextFactoryData(int id, FactoryData *in) {
        return factoryData[1][id]->PushData((unsigned char *)in);
    }

    void SetFactoryData(FactoryData *data, int id) {
        if (id < numFactoryData) {
            factoryData[0][id] = data;
        }
    }

    void SetNextFactoryData(FactoryData *data, int id) {
        if (id < numNextFactoryData) {
            factoryData[1][id] = data;
        }
    }

    void LinkFactories(VirtualFactory *src, int src_data_id, VirtualFactory *dst, int dst_data_id, FactoryData *data) {
        src->SetNextFactoryData(data, src_data_id);
        dst->SetFactoryData(data, dst_data_id);
    }

    unsigned char *GetFactoryData(size_t &size) {
        size_t total_size = 0;

        for (int i = 0; i < numFactoryData; ++i) {
            if (!PopFactoryData(i, &factoryData[2][i])) {
                for (int j = 0; j < numFactoryData; ++j) {
                    if (factoryData[2][j]) {
                        free(factoryData[2][j]);
                    }
                }
                
                memset(factoryData[2], 0, sizeof(FactoryData **) * numFactoryData);
                return nullptr;
            }

            total_size += factoryData[2][i]->size;
        }

        unsigned char *data = (unsigned char *)malloc((total_size >> 3) + 2);
        memset(data, 0, (total_size >> 3) + 2);

        for (int i = 0; i < numFactoryData; ++i) {
            size_t k = size >> 3, x;

            if (factoryData[2][i]->size & 7) {
                x = (factoryData[2][i]->size >> 3) + 1;
            } else {
                x = factoryData[2][i]->size >> 3;
            }
            
            for (size_t j = 0; j < x; ++j) {
                if (size & 7) {
                    data[k++] |= factoryData[2][i]->data[j] << (size & 7);
                    data[k] |= factoryData[2][i]->data[j] >> (8 - (size & 7));
                } else {
                    data[k++] = factoryData[2][i]->data[j];
                }
            }
            
            size += factoryData[2][i]->size;
        }

        for (int i = 0; i < numFactoryData; ++i) {
            if (factoryData[2][i]) {
                free(factoryData[2][i]);
            }
        }
        
        memset(factoryData[2], 0, sizeof(FactoryData **) * numFactoryData);
        return data;
    }

    void GiveDataToNextFactory(const unsigned char *data, size_t size) {     
        size_t n;

        if (size % numNextFactoryData) {
            n = size / numNextFactoryData + 1;
        } else {
            n = size / numNextFactoryData;
        }

        unsigned char *buf = (unsigned char *)malloc(n + 2);
        size_t total_size = 0;
        size_t i = 0;

        while (total_size < size) {
            if (i && i == size % numNextFactoryData) {
                --n;
            }

            memset(buf, 0, n);
            size_t j = total_size >> 3, x;

            if (n & 7) {
                x = (n >> 3) + 1;
            } else {
                x = n >> 3;
            }

            for (size_t k = 0; k < x; ++k) {
                buf[k] = data[j++] >> (total_size & 7);

                if (total_size & 7) {
                    buf[k] |= data[j] << (8 - (total_size & 7));
                }
            }

            total_size += n;
            PushNextFactoryData(i++, new FactoryData(buf, n));
        }

        free(buf);
    }

private:
    int numFactoryData;
    int numNextFactoryData;
    FactoryData **factoryData[4];
};

class InputFactory : public VirtualFactory {
public:
    InputFactory() : VirtualFactory(1, 2) {

    }
};

class HiddenFactory : public VirtualFactory {
public:
    HiddenFactory() : VirtualFactory(1, 1) {

    }
};

class OutputFactory : public VirtualFactory {
public:
    OutputFactory() : VirtualFactory(2, 1) {

    }
};

class CAESAR : public InputFactory {
public:
    virtual void Process(unsigned char *data, size_t size) {
        if (size & 7) {
            size = (size >> 3) + 1;
        } else {
            size >>= 3;
        }
        
        for (size_t i = 0; i < size; ++i) {
            data[i] += 3;
        }
    }    
};

class XOR_BIT2 : public HiddenFactory {
public:
    virtual void Process(unsigned char *data, size_t size) {
        for (size_t i = 1; i < (size >> 3); ++i) {
            data[i] ^= data[i - 1];
        }
    }
};

class XOR_BIT3 : public HiddenFactory {
public:
    virtual void Process(unsigned char *data, size_t size) {
        data[0] ^= data[0] << 4;

        for (size_t i = 1; i < (size >> 3); ++i) {
            data[i] ^= data[i - 1] >> 4;
            data[i] ^= data[i] << 4;
        }
    }
};

class REVERSE : public OutputFactory {
public:
    virtual void Process(unsigned char *data, size_t size) {
        if (size >> 3) {
            size = (size >> 3) - 1;
        } else {
            size = 0;
        }
        
        for (size_t i = 0, j = size; i < j; ++i, --j) {
            char tmp = data[i];
            data[i] = data[j];
            data[j] = tmp;
        }
    }  
};

class BinFactory : public VirtualFactory {
public:
    BinFactory() : VirtualFactory(1, 1) {
        caesar = new CAESAR;
        xor_bit2 = new XOR_BIT2;
        xor_bit3 = new XOR_BIT3;
        reverse = new REVERSE;
        factories[0] = (VirtualFactory *)caesar;
        factories[1] = (VirtualFactory *)xor_bit2;
        factories[2] = (VirtualFactory *)xor_bit3;
        factories[3] = (VirtualFactory *)reverse;
        LinkFactories(this, 0, factories[0], 0, new FactoryData);
        LinkFactories(factories[0], 0, factories[1], 0, new FactoryData);
        LinkFactories(factories[0], 1, factories[2], 0, new FactoryData);
        LinkFactories(factories[1], 0, factories[3], 0, new FactoryData);
        LinkFactories(factories[2], 0, factories[3], 1, new FactoryData);
        LinkFactories(factories[3], 0, this, 0, new FactoryData);
    }

    bool Check(const char *flag, size_t size) {
        bool ok = false;
        size_t result_size = 0;
        unsigned char *result = nullptr;
        GiveDataToNextFactory((const unsigned char *)flag, size << 3);

        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 4; ++j) {
                factories[j]->Work();
            }

            result_size = 0;
            result = GetFactoryData(result_size);

            if (result_size) {
                break;
            }
        }

        if (result_size && (result_size >> 3) == MAGIC_SIZE) {
            ok = true;

            for (int i = 0; i < MAGIC_SIZE; ++i) {
                if (result[i] != MAGIC[i]) {
                    ok = false;
                    break;
                }
            }
        }

        if (ok) {
            std::cout << "Congratulation! You got the challange :)) Happy RE.";
        } else {
            std::cout << "Sorry! Wrong flag.";
        }

        std::cout << std::endl;
        return ok;
    }

private:
    CAESAR *caesar;
    XOR_BIT2 *xor_bit2;
    XOR_BIT3 *xor_bit3;
    REVERSE *reverse;
    VirtualFactory *factories[4];
};

int main() {
    BinFactory *binFactory = new BinFactory;
    std::string flag;

    do {
        std::cout << "Enter your flag: ";
        std::cin >> flag;
    } while (!binFactory->Check(flag.c_str(), flag.size()));
}
