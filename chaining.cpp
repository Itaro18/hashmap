#include <iostream>
#include <cstring>  // for memcpy
#include <cstdint>   // For uint32_t
#include <string>
#include <type_traits> // For std::is_same
#include "MurmurHash3.h"  // Include MurmurHash3 header

template <typename T1, typename T2>
struct node {
    uint32_t h_key;
    T1 key;
    T2 value;
    node* next;  // Implicitly means node<T1, T2>*
    
    node(const uint32_t& h,const T1& k, const T2& v) : h_key(h),key(k), value(v), next(nullptr) {}
};
template <typename T1, typename T2>
struct bucket_node{
    node<T1,T2>* head;
    node<T1,T2>* tail;

    bucket_node() : head(nullptr), tail(nullptr) {}
};
template <typename T1,typename T2>
class hashmap{
public:
    hashmap() { 
        T1 init_key;
        T2 init_value;
        bucket = new bucket_node<T1,T2>[bucket_size]();
        std::cout << "Hashmap created with bucket size: " << bucket_size << std::endl;
    }
    ~hashmap() {
        delete[] bucket;  // Free memory to prevent leaks
        std::cout << "Hashmap destroyed!" << std::endl;
    }
    
    
    void resize(int newSize) {
        int new_bucket_size = newSize ;
        bucket_node<T1, T2>* new_bucket = new bucket_node<T1, T2>[new_bucket_size](); // Allocate new array

        // Rehash all existing elements into the new bucket array
        for (int i = 0; i < bucket_size; i++) {
            node<T1, T2>* current = bucket[i].head;

            while (current) {
                // Compute new index in the resized bucket
                uint32_t new_index = current->h_key & (new_bucket_size - 1);

                // Insert into new bucket (preserving order)
                if (new_bucket[new_index].head == nullptr) {
                    new_bucket[new_index].head = new_bucket[new_index].tail = current;
                }
                else {
                    new_bucket[new_index].tail->next = current;
                    new_bucket[new_index].tail = current;
                }

                current = current->next;  // Move to the next node
            }
        }

        // Free the old bucket array
        delete[] bucket;

        // Update internal bucket variables
        bucket = new_bucket;
        bucket_size = new_bucket_size;

        std::cout << "Hashmap resized to new bucket size: " << bucket_size << std::endl;
    }
    
    

    void insert(const T1 key,const T2 value){
        uint32_t hash_key = generate_murmur_hash(key);
        std::cout<<hash_key<<std::endl;
        int moded_index = hash_key & (bucket_size-1);
        std::cout<<moded_index<<std::endl;
        node<T1,T2>* newNode= new node<T1,T2>(hash_key,key,value);
        if (bucket[moded_index].head == nullptr) {
            bucket[moded_index].head = bucket[moded_index].tail = newNode;
        }
        else {
            bucket[moded_index].tail->next = newNode;
            bucket[moded_index].tail = newNode;
        }
        filled_spaces++;
        if((3*filled_spaces/bucket_size)>=(bucket_size/2)){//load factor > 0.5
            resize(bucket_size*2);
        }
    }

    T2 get(const T1 key){
        uint32_t hash_key = generate_murmur_hash(key);
        std::cout<<hash_key<<std::endl;
        int moded_index = hash_key & (bucket_size-1);
        node<T1,T2>* head = bucket[moded_index].head;
        while(head!=nullptr){
            if(head->key==key){
                return head->value;
            }
            head= head->next;
        }

        throw std::runtime_error("Key not found in hashmap");
    }
    void erase(const T1& key) {
        uint32_t hash_key = generate_murmur_hash(key);
        int moded_index = hash_key & (bucket_size - 1);

        node<T1, T2>* head = bucket[moded_index].head;
        node<T1, T2>* prev = nullptr;

        while (head != nullptr) {
            if (head->key == key) {  // Key found
                if (prev == nullptr) {
                    // Deleting the head node
                    bucket[moded_index].head = head->next;
                    if (bucket[moded_index].tail == head) {
                        bucket[moded_index].tail = nullptr;  // If it was the only node, update tail
                    }
                }
                else {
                    // Deleting a middle or last node
                    prev->next = head->next;
                    if (bucket[moded_index].tail == head) {
                        bucket[moded_index].tail = prev;  // Update tail if last node is deleted
                    }
                }

                delete head;  // Free memory
                std::cout << "Key " << key << " deleted from hashmap." << std::endl;
                return;
            }
            prev = head;
            head = head->next;
        }
        filled_spaces--;
        if((filled_spaces)<bucket_size/8){
            resize(bucket_size/2);
        }
    }
private:

    int bucket_size=16;
    bucket_node<T1,T2>* bucket;
    int filled_spaces=0;
    // Generic function template for MurmurHash3 (32-bit)
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

int main() {
    hashmap <int,int> m;
    float data = 22.45;
    m.insert(10, 100);
    std::cout<<m.get(10)<<std::endl;
    //m.insert("10",100);
    //uint32_t hash_value = m.generate_murmur_hash(data);
    //std::cout << "MurmurHash3: " << hash_value << std::endl;

    return 0;
}

