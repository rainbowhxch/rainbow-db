#ifndef _BPLUSTREE_H_
#define _BPLUSTREE_H_

#include <algorithm>
#include <bits/stdint-uintn.h>

#include "Row.h"

using std::upper_bound;
using std::lower_bound;


using std::string;

class BPlusTree
{
public:
    BPlusTree(string file_path);
    bool search(const uint32_t key, Row &row);
    bool remove(const uint32_t key);
    bool insert(const uint32_t key, Row row);
    bool update(const uint32_t key, Row row);
private:
    static constexpr uint32_t SIZE_OF_NODE = 4096;
    static constexpr uint32_t SIZE_OF_PARENT = sizeof(uint32_t);
    static constexpr uint32_t SIZE_OF_NUM = sizeof(uint32_t);
    struct Meta {
        uint32_t height;
        off_t root_offset;
        off_t first_leaf_offset;
        off_t file_length;
        Meta() = default;
        Meta(uint32_t height, off_t root_offset, off_t first_leaf_offset, off_t file_length)
            : height(height), root_offset(root_offset), first_leaf_offset(first_leaf_offset), file_length(file_length) {};
    }__attribute__((packed));
    static constexpr uint32_t SIZE_OF_META = sizeof(Meta);
    static constexpr uint32_t SIZE_OF_HEADER_PADDING = SIZE_OF_NODE - SIZE_OF_META;
    struct Header {
        Meta meta;
        uint8_t padding[SIZE_OF_HEADER_PADDING];
        Header() = default;
        Header(off_t root_offset) : meta(0, root_offset, root_offset, SIZE_OF_HEADER) {}
    }__attribute__((packed));
    struct Index {
        uint32_t key;
        off_t child;
    }__attribute__((packed));
    static constexpr off_t OFFSIZE_OF_HEADER = 0;
    static constexpr uint32_t SIZE_OF_HEADER = sizeof(Header);
    static constexpr uint32_t SIZE_OF_INDEX = sizeof(Index);
    static constexpr uint32_t SIZE_OF_PREV_INTERVAL = sizeof(off_t);
    static constexpr uint32_t SIZE_OF_NEXT_INTERVAL = sizeof(off_t);
    static constexpr uint32_t INDEX_SPACE = SIZE_OF_NODE-SIZE_OF_PARENT-SIZE_OF_PREV_INTERVAL-SIZE_OF_NEXT_INTERVAL-SIZE_OF_NUM;
    static constexpr uint32_t MAX_NUM_OF_INDEX = INDEX_SPACE / SIZE_OF_INDEX;
    static constexpr uint32_t SIZE_OF_INTERNAL_LEFT_SPLIT = (MAX_NUM_OF_INDEX-1) / 2;
    static constexpr uint32_t SIZE_OF_INTERNAL_RIGHT_SPLIT = MAX_NUM_OF_INDEX - SIZE_OF_INTERNAL_LEFT_SPLIT;
    static constexpr uint32_t SIZE_OF_INTERNAL_NODE_PADDING = INDEX_SPACE - (SIZE_OF_INDEX*MAX_NUM_OF_INDEX);
    struct InternalNode {
        off_t parent;
        off_t prev_interval;
        off_t next_interval;
        uint32_t num_of_index;
        Index children[MAX_NUM_OF_INDEX];
        uint8_t padding[SIZE_OF_INTERNAL_NODE_PADDING];
        InternalNode() {
            this->parent = this->prev_interval = this->next_interval = 0;
            this->num_of_index = 1;
        }
        Index *begin() { return this->children; }
        Index *end() { return this->children+this->num_of_index; }
        Index *find_index(const uint32_t key) {
            Index *next = upper_bound(this->begin(), this->end()-1, key,
                    [](const uint32_t key, const Index &index){
                        return index.key < key;
                    });
            return next;
        }
    }__attribute__((packed));
    struct Record {
        uint32_t key;
        uint8_t row[Row::SIZE_OF_ROW];
    }__attribute__((packed));
    static constexpr uint32_t SIZE_OF_RECORD = sizeof(Record);
    static constexpr uint32_t SIZE_OF_PREV_LEAF = sizeof(off_t);
    static constexpr uint32_t SIZE_OF_NEXT_LEAF = sizeof(off_t);
    static constexpr uint32_t RECORD_SPACE = SIZE_OF_NODE-SIZE_OF_PARENT-SIZE_OF_PREV_LEAF-SIZE_OF_NEXT_LEAF-SIZE_OF_NUM;
    static constexpr uint32_t MAX_NUM_OF_RECORD = RECORD_SPACE / SIZE_OF_RECORD;
    static constexpr uint32_t SIZE_OF_LEAF_LEFT_SPLIT = MAX_NUM_OF_RECORD / 2;
    static constexpr uint32_t SIZE_OF_LEAF_RIGHT_SPLIT = MAX_NUM_OF_RECORD - SIZE_OF_LEAF_LEFT_SPLIT;
    static constexpr uint32_t SIZE_OF_LEAF_NODE_PADDING = RECORD_SPACE - (MAX_NUM_OF_RECORD*SIZE_OF_RECORD);
    struct LeafNode {
        off_t parent;
        off_t prev_leaf;
        off_t next_leaf;
        uint32_t num_of_records;
        Record records[MAX_NUM_OF_RECORD];
        uint8_t padding[SIZE_OF_LEAF_NODE_PADDING];
        LeafNode() {
            this->parent = this->prev_leaf = this->next_leaf = this->num_of_records = 0;
        }
        Record *begin() { return this->records; }
        Record *end() { return this->records+this->num_of_records; }
        Record *find_record(const uint32_t key) {
            Record *record = lower_bound(this->begin(), this->end(), key,
                    [](const Record &record, const uint32_t key){
                        return record.key < key;
                    });
            return record;
        }
    }__attribute__((packed));

    string file_path;
    FILE *db_fd;
    Header header;
    /* TODO: Need to reference from pager(not implemented yet) */
    template<typename T>
    size_t swap_in(T *page, off_t offset);
    template<typename T>
    size_t swap_out(T *page, off_t offset);
    template<typename T>
    off_t alloc(T node);
    /* ------------------------------------------------------- */
    inline Index *internal_begin(InternalNode &node);
    inline Index *internal_end(InternalNode &node);
    inline Record *leaf_begin(LeafNode &node);
    inline Record *leaf_end(LeafNode &node);
    off_t search_leaf(const uint32_t key);
    void split_node(off_t cur_leaf_off, LeafNode &cur_leaf, LeafNode &next_leaf, uint32_t key, Row row);
    uint32_t split_node(off_t cur_interval_off, InternalNode &cur_interval, InternalNode &next_interval, uint32_t key, off_t child);
    void insert_record_to_leaf(LeafNode &node, const uint32_t key, Row row);
    void insert_index_to_internal(InternalNode &node, uint32_t key, off_t next_off);
    void insert_index_to_internal_and_split(off_t offset, uint32_t key, off_t old_off, off_t next_off, bool is_leaf_update);
    void update_parent(InternalNode &node, off_t parent, bool is_leaf_update);
    void update_parent(Index *begin, Index *end, off_t parent, bool is_leaf_update);
    bool borrow_record(LeafNode &node, off_t borrowed_leaf_off);
    bool borrow_index(InternalNode &node, off_t borrowed_internal_off, bool children_is_leaf);
    void change_internal_key(off_t internal_off, uint32_t old_key, uint32_t new_key, bool is_pass_old_key);
    void merge_adjacent_leaf(LeafNode &des, LeafNode &src);
    void merge_adjacent_internal(InternalNode &des, InternalNode &src, bool children_is_leaf);
    void remove_index(off_t parent_off, InternalNode &parent, uint32_t need_removed_key, bool children_is_leaf);
};

inline BPlusTree::Index *BPlusTree::internal_begin(InternalNode &node) {
    return node.children;
}
inline BPlusTree::Index *BPlusTree::internal_end(InternalNode &node) {
    return node.children + node.num_of_index;
}
inline BPlusTree::Record *BPlusTree::leaf_begin(LeafNode &node) {
    return node.records;
}
inline BPlusTree::Record *BPlusTree::leaf_end(LeafNode &node) {
    return node.records + node.num_of_records;
}

template<typename T>
size_t BPlusTree::swap_in(T *page, off_t offset)
{
    db_fd = fopen(file_path.c_str(), "rb+");
    fseek(db_fd, offset, SEEK_SET);
    size_t rsize = fread(page, sizeof(T), 1, db_fd);
    fclose(db_fd);
    return rsize;
}

template<typename T>
size_t BPlusTree::swap_out(T *page, off_t offset)
{
    db_fd = fopen(file_path.c_str(), "rb+");
    fseek(db_fd, offset, SEEK_SET);
    size_t wsize = fwrite(page, sizeof(T), 1, db_fd);
    fclose(db_fd);
    return wsize;
}

template<typename T>
off_t BPlusTree::alloc(T node)
{
    off_t file_length = header.meta.file_length;
    header.meta.file_length += sizeof(T);
    return file_length;
}

#endif
