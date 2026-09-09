#ifndef B_TREE_MULTISET_HPP
#define B_TREE_MULTISET_HPP

#include <vector>
#include <algorithm>

/**
 * @brief A cache-conscious multiset implemented as a flat, pointer-free B-Tree.
 * All nodes are stored sequentially in a single vector to maximize L1/L2 cache locality.
 */
class BTreeMultiset {
private:
    static constexpr int BLOCK_DEGREE = 4; 
    static constexpr int MAX_KEYS = 2 * BLOCK_DEGREE - 1;   // 7 keys per block
    static constexpr int MAX_CHILDREN = 2 * BLOCK_DEGREE;  // 8 children per block

    struct DataBlock {
        int keys[MAX_KEYS];                 
        int child_indices[MAX_CHILDREN];    // Array offsets (integers instead of raw pointers)
        int num_keys = 0;
        int subtree_size = 0;
        bool is_leaf = true;
    };

    std::vector<DataBlock> storage_pool_;
    int root_index_;
    size_t total_elements_ = 0; // O(1) Size Tracking Variable

    // Helper to calculate exact O(1) subtree size for tracking
    void update_subtree_size(int block_idx) {
        if (block_idx == -1) return;
        DataBlock& block = storage_pool_[block_idx];
        block.subtree_size = block.num_keys;
        if (!block.is_leaf) {
            for (int i = 0; i <= block.num_keys; ++i) {
                block.subtree_size += storage_pool_[block.child_indices[i]].subtree_size;
            }
        }
    }

    // Counts keys strictly smaller than 'val' inside a subtree
    int count_keys_less_than(int block_idx, int val) const {
        if (block_idx == -1) return 0;
        
        const DataBlock& block = storage_pool_[block_idx];
        int i = 0;
        
        // 100% cache-local linear scan.
        while (i < block.num_keys && block.keys[i] < val) {
            i++;
        }

        if (block.is_leaf) return i;

        int lower_count = 0;
        // Optimization: O(1) size lookups using the augmented Order Statistic property
        for (int j = 0; j < i; j++) {
            lower_count += storage_pool_[block.child_indices[j]].subtree_size;
        }
        lower_count += i;
        lower_count += count_keys_less_than(block.child_indices[i], val);

        return lower_count;
    }

    void split_full_block(int parent_idx, int i, int child_idx) {
        DataBlock new_block;
        new_block.is_leaf = storage_pool_[child_idx].is_leaf;
        new_block.num_keys = BLOCK_DEGREE - 1;

        for (int j = 0; j < BLOCK_DEGREE - 1; j++) {
            new_block.keys[j] = storage_pool_[child_idx].keys[j + BLOCK_DEGREE];
        }

        if (!storage_pool_[child_idx].is_leaf) {
            for (int j = 0; j < BLOCK_DEGREE; j++) {
                new_block.child_indices[j] = storage_pool_[child_idx].child_indices[j + BLOCK_DEGREE];
            }
        }

        storage_pool_[child_idx].num_keys = BLOCK_DEGREE - 1;
        storage_pool_.push_back(new_block);
        int new_block_idx = storage_pool_.size() - 1;
        
        update_subtree_size(child_idx);
        update_subtree_size(new_block_idx);

        for (int j = storage_pool_[parent_idx].num_keys; j >= i + 1; j--) {
            storage_pool_[parent_idx].child_indices[j + 1] = storage_pool_[parent_idx].child_indices[j];
        }
        storage_pool_[parent_idx].child_indices[i + 1] = new_block_idx;

        for (int j = storage_pool_[parent_idx].num_keys - 1; j >= i; j--) {
            storage_pool_[parent_idx].keys[j + 1] = storage_pool_[parent_idx].keys[j];
        }
        
        storage_pool_[parent_idx].keys[i] = storage_pool_[child_idx].keys[BLOCK_DEGREE - 1];
        storage_pool_[parent_idx].num_keys++;
    }

    void insert_into_non_full_block(int block_idx, int val) {
        storage_pool_[block_idx].subtree_size++;
        int i = storage_pool_[block_idx].num_keys - 1;

        if (storage_pool_[block_idx].is_leaf) {
            while (i >= 0 && storage_pool_[block_idx].keys[i] > val) {
                storage_pool_[block_idx].keys[i + 1] = storage_pool_[block_idx].keys[i];
                i--;
            }
            storage_pool_[block_idx].keys[i + 1] = val;
            storage_pool_[block_idx].num_keys++;
        } else {
            while (i >= 0 && storage_pool_[block_idx].keys[i] > val) {
                i--;
            }
            i++;
            int child_idx = storage_pool_[block_idx].child_indices[i];
            if (storage_pool_[child_idx].num_keys == MAX_KEYS) {
                split_full_block(block_idx, i, child_idx);
                if (storage_pool_[block_idx].keys[i] < val) {
                    i++;
                }
            }
            insert_into_non_full_block(storage_pool_[block_idx].child_indices[i], val);
        }
    }

public:
    BTreeMultiset() {
        DataBlock root_block;
        std::fill(std::begin(root_block.child_indices), std::end(root_block.child_indices), -1);
        storage_pool_.push_back(root_block);
        root_index_ = 0;
    }

    // Inserts an item: O(log N)
    void insert(int val) {
        if (storage_pool_[root_index_].num_keys == MAX_KEYS) {
            DataBlock new_root;
            std::fill(std::begin(new_root.child_indices), std::end(new_root.child_indices), -1);
            new_root.is_leaf = false;
            new_root.child_indices[0] = root_index_; 
            new_root.subtree_size = storage_pool_[root_index_].subtree_size;
            storage_pool_.push_back(new_root);
            
            int old_root_idx = root_index_;
            root_index_ = storage_pool_.size() - 1;
            split_full_block(root_index_, 0, old_root_idx);
        }
        insert_into_non_full_block(root_index_, val);
        total_elements_++; // Increment global size tracker
    }

    // Total elements in the multiset: NOW O(1) 🚀
    size_t size() const {
        return total_elements_;
    }

    // Check if the multiset is empty: O(1)
    bool empty() const {
        return total_elements_ == 0;
    }

    // Counts occurrences of a specific element: O(log N)
    int count(int val) const {
        return lower_bound_count(val + 1) - lower_bound_count(val);
    }

    // Returns number of elements strictly less than 'val': O(log N)
    int lower_bound_count(int val) const {
        return count_keys_less_than(root_index_, val);
    }

    // Returns number of elements strictly greater than 'val': NOW O(log N) 🚀
    int upper_bound_count(int val) const {
        return total_elements_ - lower_bound_count(val + 1);
    }
};

#endif // B_TREE_MULTISET_HPP
