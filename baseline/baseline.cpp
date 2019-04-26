#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <time.h>
#include <unistd.h>

#include "baseline.h"

file_reader::file_reader(int capacity, std::string dataset_path,
    int dataset_size, bool nvme_support)
{
    m_capacity = capacity;
    m_nvme_support = nvme_support;
    m_dataset_path = dataset_path;
    m_dataset_size = dataset_size;
    m_num_memory_hit = 1;
    m_time_memory_hit = 100;

    // initialize a random seed
    srand(time(NULL));
}

std::unordered_map<int, std::vector<char>*> file_reader::get_file_content_map()
{
    return m_file_content_map;
}
std::vector<char>* file_reader::get_content_by_file(int file_id)
{
    return m_file_content_map.at(file_id);
}

int file_reader::get_capacity()
{
    return m_capacity;
}

int file_reader::get_dataset_size()
{
    return m_dataset_size;
}

bool file_reader::has_nvme_support()
{
    return m_nvme_support;
}

void file_reader::prefetch_files()
{
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 1; i <= m_dataset_size; i++)
    {
        if(!has_memory_capacity() && !has_nvme_support())
        {
            break;
        }
        bool in_memory = true;
        if(has_nvme_support())
        {
            if (i > m_capacity)
            {
                in_memory = false;
            }
        }
        m_in_memory_map[i] = in_memory;
        fetch_file(i);
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Prefetch: " << std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count() << " ns" << std::endl;
}

void file_reader::random_file_seek()
{
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 1; i <= 1000; i++)
    {
        int file_id = get_random_file_id();
        try
        {
            int delay = (int) m_time_memory_hit/m_num_memory_hit * 3;
            auto start = std::chrono::high_resolution_clock::now();
            std::vector<char>* file_content = m_file_content_map.at(file_id);
            auto finish_mem = std::chrono::high_resolution_clock::now();

            if(has_nvme_support() && is_file_on_nvme(file_id))
            {
                // std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
                // std::cout << "NVMe: Delay: " << delay << ": ";
                int count = 0;
                while(count < delay/4)
                {
                    count++;
                }
            }

            auto finish = std::chrono::high_resolution_clock::now();

            int mem_time = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_mem-start).count();
            m_time_memory_hit += mem_time;
            m_num_memory_hit++;

            // std::cout << "Read 1 file from MEM: " 
            //     << std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count()
            //     << " ns" << std::endl;
        }
        catch(std::exception& e)
        {
            // remove the first element
            m_file_content_map.erase(m_file_content_map.begin());

            // read from HDD
            fetch_file(file_id);
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "Read 1000 files: " << std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count() << " ns" << std::endl;
}

bool file_reader::has_memory_capacity()
{
    return m_file_content_map.size() < m_capacity;
}

bool file_reader::read_file(const std::string filename, std::vector<char>& buf)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::ifstream file(filename, std::ios::binary);
    if (!file.good())
    {
        return false;
    }

    // unsets the flag to skip white space
    file.unsetf(std::ios::skipws);

    file.seekg(0, std::ios::end);
    const std::streampos file_size = file.tellg();

    file.seekg(0, std::ios::beg);

    buf.resize(file_size);

    file.read(buf.data(), file_size);

    auto finish = std::chrono::high_resolution_clock::now();
    // std::cout << "Read 1 file from HDD: " 
    //     << std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count()
    //     << " ns" << std::endl;

    return true;
}

int file_reader::get_random_file_id()
{
    return rand() % m_dataset_size + 1;
}

void file_reader::fetch_file(int file_id)
{
    std::string file_path = m_dataset_path + "/" + std::to_string(file_id) + ".png";
    std::vector<char>* file_content = new std::vector<char>();
    read_file(file_path, *file_content);
    m_file_content_map[file_id] = file_content;
}

bool file_reader::is_file_on_nvme(int file_id)
{
    return !m_in_memory_map.at(file_id);
}

int main(int argc, char** argv)
{
    // take from argv
    std::string dataset_path = "dummy";
    int capacity = 100;
    int dataset_size = 300;
    bool has_nvme_support = false;
    for (int i = 0; i < argc; i++)
    {
        std::string argv_str(argv[i]);
        if(argv_str == "--enable-nvme")
        {
            has_nvme_support = true;
        }
        if(argv_str == "--capacity")
        {
            capacity = std::stoi(std::string(argv[i+1]));
        }
        if(argv_str == "--dataset-size")
        {
            dataset_size = std::stoi(std::string(argv[i+2]));
        }
    }

    file_reader _file_reader(capacity, dataset_path, dataset_size, has_nvme_support);
    _file_reader.prefetch_files();
    // std::cout << _file_reader.get_file_content_map().size() << std::endl;
    // std::cout << _file_reader.get_content_by_file(10).data() << std::endl;
    _file_reader.random_file_seek();
}
