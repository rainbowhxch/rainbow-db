#include "../include/BPlusTree.h"

BPlusTree::BPlusTree(string file_path)
    :file_path(file_path)
{
    db_fd = fopen(file_path.c_str(), "rb+");
    if (db_fd == nullptr) {
        db_fd = fopen(file_path.c_str(), "wb+");
        fclose(db_fd);
    }
    if (swap_in(&header, OFFSIZE_OF_HEADER) == 0) {
        LeafNode root;
        header = Header(alloc(root));
        swap_out(&header, OFFSIZE_OF_HEADER);
        swap_out(&root, header.meta.root_offset);
    }
}

bool BPlusTree::search(const uint32_t key, Row &row)
{
    LeafNode leaf;
    swap_in(&leaf, search_leaf(key));
    Record *record = leaf.find_record(key);
    if (record != leaf.end() && record->key == key) {
        row.deserialize_row(record->row);
        return true;
    } else {
        return false;
    }
}

off_t BPlusTree::search_leaf(const uint32_t key)
{
    off_t cur_off = header.meta.root_offset;
    int height = header.meta.height;
    while (height >= 1) {
        InternalNode cur_node;
        swap_in(&cur_node, cur_off);
        Index *next = cur_node.find_index(key);
        cur_off = next->child;
        height--;
    }

    return cur_off;
}

bool BPlusTree::remove(const uint32_t key)
{
    off_t leaf_off = search_leaf(key);
    LeafNode leaf;
    swap_in(&leaf, leaf_off);
    Record *record = leaf.find_record(key);
    if (record == leaf.end() || record->key != key)
        return false;
    std::copy(record+1, leaf.end(), record);
    leaf.num_of_records--;

    int min_records_cnt = header.meta.root_offset == leaf_off ? 0 : MAX_NUM_OF_RECORD/2;

    if (leaf.num_of_records < min_records_cnt) {
        bool borrowed = false;
        if (leaf.prev_leaf != 0)
            borrowed = borrow_record(leaf, leaf.prev_leaf);
        if (borrowed == false && leaf.next_leaf != 0)
            borrowed = borrow_record(leaf, leaf.next_leaf);

        if (borrowed == false) {
            InternalNode parent;
            swap_in(&parent, leaf.parent);
            uint32_t need_removed_key;
            if (leaf.prev_leaf != 0 && leaf_off == (parent.end()-1)->child) { /* merge prev with leaf */
                LeafNode prev;
                need_removed_key = leaf.begin()->key;
                swap_in(&prev, leaf.prev_leaf);
                merge_adjacent_leaf(prev, leaf);
                swap_out(&prev, leaf.prev_leaf);
            } else if (leaf.next_leaf != 0 && leaf_off != (parent.end()-1)->child) { /* merge leaf with next */
                LeafNode next;
                need_removed_key = next.begin()->key;
                swap_in(&next, leaf.next_leaf);
                merge_adjacent_leaf(leaf, next);
                swap_out(&next, leaf.next_leaf);
            }
            remove_index(leaf.parent, parent, need_removed_key, true);
        } else {
            swap_out(&leaf, leaf_off);
        }
    } else {
        swap_out(&leaf, leaf_off);
    }

    return true;
}

void BPlusTree::remove_index(off_t internal_off, InternalNode &internal, uint32_t need_removed_key, bool children_is_leaf)
{
    Index *index = lower_bound(internal.begin(), internal.end(), need_removed_key,
            [](const Index &index, const uint32_t key){
                return index.key < key;
            });
    if (index != internal.end()) {
        (index+1)->child = index->child;
        std::copy(index+1, internal.end(), index);
    }
    internal.num_of_index--;

    if (internal.num_of_index == 1 && header.meta.root_offset == internal_off) {
        header.meta.root_offset = internal.begin()->child;
        header.meta.height--;
        swap_out(&header, OFFSIZE_OF_HEADER);
        /* TODO: unalloc root internal node */
        return;
    }

    int min_index_cnt = header.meta.root_offset == internal_off ? 1 : MAX_NUM_OF_INDEX/2;

    if (internal.num_of_index < min_index_cnt) {
        bool borrowed = false;
        if (internal.prev_interval != 0)
            borrowed = borrow_index(internal, internal.prev_interval, children_is_leaf);
        if (!borrowed && internal.next_interval != 0)
            borrowed = borrow_index(internal, internal.next_interval, children_is_leaf);
        if (!borrowed) {
            InternalNode parent;
            swap_in(&parent, internal.parent);
            uint32_t need_removed_key;
            if (internal_off == (parent.end()-1)->child) { /* merge prev with internal */
                InternalNode prev;
                need_removed_key = internal.begin()->key;
                swap_in(&prev, internal.prev_interval);
                merge_adjacent_internal(prev, internal, children_is_leaf);
                swap_out(&prev, internal.prev_interval);
            } else { /* merge internal with next */
                InternalNode next;
                need_removed_key =next.begin()->key;
                swap_in(&next, internal.next_interval);
                merge_adjacent_internal(next, internal, children_is_leaf);
                swap_out(&next, internal.next_interval);
            }
            remove_index(internal.parent, parent, need_removed_key, false);
        } else {
            swap_out(&internal, internal_off);
        }
    } else {
        swap_out(&internal, internal_off);
    }
}

void BPlusTree::merge_adjacent_leaf(LeafNode &des, LeafNode &src)
{
    std::copy(src.begin(), src.end(), des.end());
    des.num_of_records += src.num_of_records;
    des.next_leaf = src.next_leaf;
    if (src.next_leaf != 0) {
        LeafNode src_next_leaf;
        swap_in(&src_next_leaf, src.next_leaf);
        src_next_leaf.prev_leaf = src.prev_leaf;
        swap_out(&src_next_leaf, src.next_leaf);
    }
    /* TODO: unalloc src node */
}

void BPlusTree::merge_adjacent_internal(InternalNode &des, InternalNode &src, bool children_is_leaf)
{
    std::copy(src.begin(), src.end(), des.end());
    des.num_of_index += src.num_of_index;
    des.next_interval = src.next_interval;
    if (src.next_interval != 0) {
        InternalNode src_next_internal;
        swap_in(&src_next_internal, src.next_interval);
        src_next_internal.prev_interval = src.prev_interval;
        swap_out(&src_next_internal, src.next_interval);
    }
    update_parent(src, src.prev_interval, children_is_leaf);
    /* TODO: unalloc src node */
}


bool BPlusTree::borrow_record(LeafNode &node, off_t borrowed_leaf_off)
{
    LeafNode borrowed_node;
    swap_in(&borrowed_node, borrowed_leaf_off);

    Record *where_to_put, *where_be_borrowed;
    if (borrowed_node.num_of_records != MAX_NUM_OF_RECORD/2) {
        if (node.prev_leaf == borrowed_leaf_off) { /* borrowed prev leaf */
            where_to_put = node.begin();
            where_be_borrowed = borrowed_node.end()-1;
            change_internal_key(borrowed_node.parent, borrowed_node.begin()->key, where_be_borrowed->key, true);
        } else { /* borrowed next leaf */
            where_to_put = node.end();
            where_be_borrowed = borrowed_node.begin();
            change_internal_key(node.parent, node.begin()->key, (where_be_borrowed+1)->key, true);
        }

        std::copy_backward(where_to_put, node.end(), node.end()+1);
        *where_to_put = *where_be_borrowed;
        node.num_of_records++;
        std::copy(where_be_borrowed+1, borrowed_node.end(), where_be_borrowed);
        borrowed_node.num_of_records--;
        swap_out(&borrowed_node, borrowed_leaf_off);

        return true;
    }

    return false;
}

bool BPlusTree::borrow_index(InternalNode &node, off_t borrowed_internal_off, bool children_is_leaf)
{
    InternalNode borrowed_node;
    swap_in(&borrowed_node, borrowed_internal_off);

    Index *where_to_put, *where_be_borrowed;
    off_t need_updated_parent_off;
    if (borrowed_node.num_of_index != MAX_NUM_OF_INDEX/2) {
        if (node.prev_interval == borrowed_internal_off) { /* borrowed prev leaf */
            need_updated_parent_off = borrowed_node.next_interval;
            where_to_put = node.begin();
            where_be_borrowed = borrowed_node.end()-1;
            change_internal_key(borrowed_node.parent, borrowed_node.begin()->key, (where_be_borrowed-1)->key, true);
        } else { /* borrowed next leaf */
            need_updated_parent_off = borrowed_node.prev_interval;
            where_to_put = node.end();
            where_be_borrowed = borrowed_node.begin();
            change_internal_key(node.parent, (node.end()-1)->key, where_be_borrowed->key, false);
        }

        std::copy_backward(where_to_put, node.begin(), node.end()+1);
        *where_to_put = *where_be_borrowed;
        node.num_of_index++;
        update_parent(where_be_borrowed, where_be_borrowed+1, need_updated_parent_off, children_is_leaf);
        std::copy(where_be_borrowed+1, borrowed_node.end(), where_be_borrowed);
        borrowed_node.num_of_index--;
        swap_out(&borrowed_node, borrowed_internal_off);

        return true;
    }

    return false;
}

void BPlusTree::change_internal_key(off_t internal_off, uint32_t old_key, uint32_t new_key, bool is_pass_old_key)
{
    InternalNode internal;
    swap_in(&internal, internal_off);
    Index *index;
    if (is_pass_old_key) {
        index = internal.find_index(old_key);
    } else {
        index = lower_bound(internal.begin(), internal.end()-1, old_key,
                [](const Index &index, const uint32_t key){
                    return index.key < key;
                });
    }
    index->key = new_key;
    swap_out(&internal, internal_off);
    if (index == internal.end()-1)
        change_internal_key(internal.parent, old_key, new_key, true);
}

bool BPlusTree::insert(const uint32_t key, Row row)
{
    off_t searched_leaf_off = search_leaf(key);
    LeafNode searched_leaf;
    swap_in(&searched_leaf, searched_leaf_off);
    Record *record = searched_leaf.find_record(key);
    if (record != searched_leaf.end())
        return false;
    if (searched_leaf.num_of_records == MAX_NUM_OF_RECORD) {
        LeafNode next_leaf;
        split_node(searched_leaf_off, searched_leaf, next_leaf, key, row);
        swap_out(&searched_leaf, searched_leaf_off);
        swap_out(&next_leaf, searched_leaf.next_leaf);
        insert_index_to_internal_and_split(searched_leaf.parent, next_leaf.records[0].key, searched_leaf_off,searched_leaf.next_leaf, true);
    } else {
        insert_record_to_leaf(searched_leaf, key, row);
        swap_out(&searched_leaf, searched_leaf_off);
    }

    return true;
}

void BPlusTree::split_node(off_t cur_leaf_off, LeafNode &cur_leaf, LeafNode &next_leaf, uint32_t key, Row row)
{
    next_leaf.parent = cur_leaf.parent;
    next_leaf.prev_leaf = cur_leaf_off;
    next_leaf.next_leaf = cur_leaf.next_leaf;
    cur_leaf.next_leaf = alloc(&next_leaf);
    if (next_leaf.next_leaf != 0) {
        LeafNode next_next_leaf;
        swap_in(&next_next_leaf, next_leaf.next_leaf);
        next_next_leaf.prev_leaf = cur_leaf.next_leaf;
        swap_out(&next_next_leaf, next_leaf.next_leaf);
    }
    std::copy(cur_leaf.records+SIZE_OF_LEAF_LEFT_SPLIT, cur_leaf.end(), next_leaf.begin());
    cur_leaf.num_of_records = SIZE_OF_LEAF_LEFT_SPLIT;
    next_leaf.num_of_records = SIZE_OF_LEAF_RIGHT_SPLIT;
    if (cur_leaf.records[SIZE_OF_LEAF_LEFT_SPLIT-1].key > key) {
        insert_record_to_leaf(cur_leaf, key, row);
    } else {
        insert_record_to_leaf(next_leaf, key, row);
    }
}

void BPlusTree::insert_record_to_leaf(LeafNode &leaf, const uint32_t key, Row row) {
    Record *record = leaf.find_record(key);
    std::copy_backward(record, leaf.end(), leaf.end()+1);
    record->key = key;
    row.serialize_row(record->row);
    leaf.num_of_records++;
}

void BPlusTree::insert_index_to_internal_and_split(off_t internal_off, uint32_t key, off_t old_child, off_t created_child, bool is_leaf_update)
{
    if (internal_off == 0) {
        InternalNode root;
        root.parent = root.prev_interval = root.next_interval = 0;
        header.meta.root_offset = alloc(&root);
        header.meta.height++;
        root.num_of_index = 2;
        root.children[0].key = key;
        root.children[0].child = old_child;
        root.children[1].child = created_child;
        swap_out(&header, OFFSIZE_OF_HEADER);
        swap_out(&root, header.meta.root_offset);
        update_parent(root, header.meta.root_offset, is_leaf_update);
        return;
    }

    InternalNode cur_internal;
    swap_in(&cur_internal, internal_off);
    if (cur_internal.num_of_index == MAX_NUM_OF_INDEX) {
        InternalNode new_interval;
        uint32_t insert_key_to_parent = split_node(internal_off, cur_internal, new_interval, key, created_child);
        swap_out(&cur_internal, internal_off);
        swap_out(&new_interval, cur_internal.next_interval);
        update_parent(new_interval, cur_internal.next_interval, is_leaf_update);
        insert_index_to_internal_and_split(cur_internal.parent, insert_key_to_parent, internal_off, cur_internal.next_interval, false);
    } else {
        insert_index_to_internal(cur_internal, key, created_child);
        swap_out(&cur_internal, internal_off);
    }
}

uint32_t BPlusTree::split_node(off_t cur_interval_off, InternalNode &cur_interval, InternalNode &next_interval, uint32_t key, off_t child)
{
    next_interval.parent = cur_interval.parent;
    next_interval.next_interval = cur_interval.next_interval;
    cur_interval.next_interval = alloc(&next_interval);
    next_interval.prev_interval = cur_interval_off;
    if (next_interval.next_interval != 0) {
        InternalNode next_next_interval;
        swap_in(&next_next_interval, next_interval.next_interval);
        next_next_interval.prev_interval = cur_interval.next_interval;
        swap_out(&next_next_interval, next_interval.next_interval);
    }
    std::copy(cur_interval.children+SIZE_OF_INTERNAL_LEFT_SPLIT, internal_end(cur_interval), next_interval.begin());
    cur_interval.num_of_index = SIZE_OF_INTERNAL_LEFT_SPLIT;
    next_interval.num_of_index = SIZE_OF_INTERNAL_RIGHT_SPLIT;
    if (cur_interval.children[SIZE_OF_INTERNAL_LEFT_SPLIT-1].key < key) {
        insert_index_to_internal(cur_interval, key, child);
    } else {
        insert_index_to_internal(next_interval, key, child);
    }

    return cur_interval.children[SIZE_OF_INTERNAL_LEFT_SPLIT-1].key;
}

void BPlusTree::insert_index_to_internal(InternalNode &internal, uint32_t key, off_t new_child)
{
    Index *index = internal.find_index(key);
    std::copy_backward(index, internal_end(internal), internal_end(internal)+1);
    index->key = key;
    index->child = (index+1)->child;
    (index+1)->child = new_child;
    internal.num_of_index++;
}

bool BPlusTree::update(const uint32_t key, Row row)
{
    off_t searched_leaf_off = search_leaf(key);
    LeafNode searched_leaf;
    swap_in(&searched_leaf, searched_leaf_off);
    Record *searched_record = searched_leaf.find_record(key);
    if (searched_record == searched_leaf.end() || searched_record->key != key) {
        return false;
    } else {
        row.serialize_row(searched_record->row);
        swap_out(&searched_leaf, searched_leaf_off);
        return true;
    }
}

void BPlusTree::update_parent(InternalNode &parent_node, off_t parent_off, bool is_leaf_update)
{
    update_parent(parent_node.begin(), parent_node.end(), parent_off, is_leaf_update);
}

void BPlusTree::update_parent(Index *begin, Index *end, off_t parent, bool is_leaf_update)
{
    while (begin != end) {
        if (is_leaf_update) {
            LeafNode node;
            swap_in(&node, begin->child);
            node.parent = parent;
            swap_out(&node, begin->child);
        } else {
            InternalNode node;
            swap_in(&node, begin->child);
            node.parent = parent;
            swap_out(&node, begin->child);
        }
        begin++;
    }
}
