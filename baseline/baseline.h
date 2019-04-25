#include <unordered_map>
#include <vector>

class file_reader
{
public:
    file_reader(int capacity = 100, std::string dataset_path = "dummy",
        int dataset_size = 300, bool nvme_support = false);

    std::unordered_map<int, std::vector<char>*> get_file_content_map();
    std::vector<char>* get_content_by_file(int file_id);
    int get_capacity();
    std::string get_dataset_path();
    int get_dataset_size();
    bool has_nvme_support();

    void prefetch_files();
    void random_file_seek();

private:
    bool has_memory_capacity();
    bool read_file(const std::string filename, std::vector<char>& buf);
    int get_random_file_id();
    void fetch_file(int file_id);
    bool is_file_on_nvme(int file_id);

    std::unordered_map<int, std::vector<char>*> m_file_content_map;
    std::unordered_map<int, bool> m_in_memory_map;
    int m_capacity;
    std::string m_dataset_path;
    int m_dataset_size;
    bool m_nvme_support;
    int m_time_memory_hit;
    int m_num_memory_hit;
};
