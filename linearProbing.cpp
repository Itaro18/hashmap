#include<iostream>
#include<string>
#include<cstdint>
#include "MurmurHash3.h"
#include <type_traits> 

template <typename T1,typename T2>
struct node{
    uint32_t h_key;
    T1 key;
    T2 value;
    bool empty = true;
    bool erased = false;
};


template <typename T1,typename T2>
class hashmap{

    public:

        hashmap(){
            bucket = new node<T1,T2>[bucket_size];
        }

        ~hashmap(){
            delete[] bucket;
        }

        void resize(int new_size){
            node<T1,T2>* new_bucket=new node<T1,T2>[new_size];
            for(int i=0;i<bucket_size;i++){
                if(bucket[i].empty == false and bucket[i].erased == false){
                    T1 key = bucket[i].key;
                    T2 value = bucket[i].value;
                    uint32_t h_key=generate_murmur_hash(key);
                    int moded_index=h_key & (new_size-1);
                    int index=0;
                    while(!new_bucket[(moded_index+index)%new_size].empty && !new_bucket[(moded_index+index)%new_size].erased){
                        index++;
                        if (index==new_size){
                            throw std::runtime_error("resize fuckup");
                        }
                    }
                    new_bucket[(moded_index+index)%new_size].key=key;
                    new_bucket[(moded_index+index)%new_size].value=value;
                    new_bucket[(moded_index+index)%new_size].h_key=h_key;
                    new_bucket[(moded_index+index)%new_size].empty=false;
                }
            }
            delete[] bucket;
            bucket_size = new_size;
            bucket = new_bucket;
            
        }

        void insert(const T1 key ,const T2 value){
            uint32_t h_key=generate_murmur_hash(key);
            int moded_index=h_key & (bucket_size-1);
            int index = 0;
            while(!bucket[(moded_index+index)%bucket_size].empty && !bucket[(moded_index+index)%bucket_size].erased){
                if (!bucket[(moded_index+index) % bucket_size].empty &&
                !bucket[(moded_index+index) % bucket_size].erased &&
                bucket[(moded_index+index) % bucket_size].key == key) {
                    // Key already exists update and return.
                    bucket[(moded_index+index) % bucket_size].value = value;
                    return;
                }
                index++;
                if(index == bucket_size){
                    throw std::runtime_error("insert failed: table full");
                }
            }
            int pos = (moded_index + index) % bucket_size;
            bucket[pos].key = key;
            bucket[pos].value = value;
            bucket[pos].h_key = h_key;
            bucket[pos].empty = false;
            filled_spaces++;
            double load_factor = static_cast<double>(filled_spaces) / bucket_size; 
            if(load_factor>=0.5){ 
                resize(bucket_size*2);
            }
            
        }

        void erase(const T1 key){
            uint32_t h_key=generate_murmur_hash(key);
            int moded_index=h_key & (bucket_size-1);
            int index=0;
            while(!bucket[(moded_index+index)%bucket_size].empty) {
                if(bucket[(moded_index+index)%bucket_size].key == key) {
                    bucket[(moded_index+index)%bucket_size].erased = true;
                    filled_spaces--;
                    double load_factor = static_cast<double>(filled_spaces) / bucket_size;
                    if (load_factor < 0.125){
                        resize(bucket_size/2);
                    }
                    return;
                }
                index++;
                if(index == bucket_size){
                    throw std::runtime_error("erase failed: key not found");
                }
            }
            
        }
        
        T2 get(const T1 key){
            uint32_t h_key=generate_murmur_hash(key);
            int moded_index=h_key & (bucket_size-1);
            int index=0;
            while( (bucket[(moded_index+index)%bucket_size].empty == false &&
                bucket[(moded_index+index)%bucket_size].erased == false &&
                bucket[(moded_index+index)%bucket_size].key != key) ) {
                index++;
                if (index == bucket_size) {
                    throw std::runtime_error("Key not found in hashmap");
                }
            }
            if(bucket[(moded_index+index)%bucket_size].empty == false &&
            bucket[(moded_index+index)%bucket_size].erased == false &&
            bucket[(moded_index+index)%bucket_size].key == key ){
                return bucket[(moded_index+index)%bucket_size].value;
            }
            throw std::runtime_error("Key not found in hashmap");            

        }
    private:
        
        int bucket_size=16;
        node<T1,T2>* bucket;
        int filled_spaces=0;

        uint32_t generate_murmur_hash(const T1& input) {
            uint32_t hash;  // Output hash variable
            if constexpr (std::is_same<T1, std::string>::value) {
                // Handle std::string
                MurmurHash3_x86_32(input.data(), input.size(), 42, &hash);
            } else {
                // Handle int, float, and other POD (Plain Old Data) types
                MurmurHash3_x86_32(&input, sizeof(T1), 42, &hash);
            }
            return hash;
        }
};

int main(){
    hashmap <int,int> m;
    float data = 22.45;
    m.insert(10, 100);
    std::cout<<m.get(10)<<std::endl;
    //m.insert("10",100);
    //uint32_t hash_value = m.generate_murmur_hash(data);
    //std::cout << "MurmurHash3: " << hash_value << std::endl;

    return 0;
}
