#pragma once

#include <algorithm>
#include <compare>
#include <initializer_list>
#include <iostream>
#include <memory>

namespace treeset {

    const bool BLACK = false;
    const bool RED = true;

    namespace detail {

        template <typename T>
        struct Node {
            T key;
            bool color;
            Node* parent;
            Node* left;
            Node* right;

            Node(
                T key_,
                bool color_ = RED,
                Node* parent_ = 0,
                Node* left_ = 0,
                Node* right_ = 0)
                : key(key_),
                  color(color_),
                  parent(parent_),
                  left(left_),
                  right(right_){};

            auto operator<=>(const Node& rhs) const {
                return key <=> rhs.key;
            }
            bool operator==(const Node& rhs) const {
                return key == rhs.key;
            }
        };
    }  // namespace detail

    template <typename T>
    class Set {
       private:
        detail::Node<T>* root;
        detail::Node<T>* null_node;
        detail::Node<T>** min_;
        detail::Node<T>** max_;
        std::size_t size_;

        detail::Node<T>* min(detail::Node<T>* node) const {
            while (node->left != null_node) {
                node = node->left;
            }
            return node;
        }

        detail::Node<T>* grandparent(detail::Node<T>* node) const {
            return node->parent->parent;
        }

        detail::Node<T>* uncle(detail::Node<T>* node) const {
            auto grandpa = grandparent(node);
            if (grandpa->left == node->parent) {
                return grandpa->right;
            } else {
                return grandpa->left;
            }
        }

        detail::Node<T>* second_child(
            detail::Node<T>* parent,
            detail::Node<T>* child) const {
            if (parent->left == child) {
                return parent->right;
            } else if (parent->right == child) {
                return parent->left;
            } else {
                return null_node;
            }
        }

        void clear(detail::Node<T>* node) {
            if (node == null_node) {
                return;
            }
            clear(node->left);
            clear(node->right);
            --size_;
            delete node;
            node = null_node;
        }

        detail::Node<T>* find(T key, detail::Node<T>* node) const {
            while (node != null_node) {
                if (node->key < key) {
                    if (node->right == null_node) {
                        return node;
                    }
                    node = node->right;
                } else if (node->key > key) {
                    if (node->left == null_node) {
                        return node;
                    }
                    node = node->left;
                } else {
                    return node;
                }
            }
            return null_node;
        }

        void rotate_left(detail::Node<T>* node) {
            auto right = node->right;
            if (node == null_node || right == null_node) {
                return;
            }
            node->right = right->left;
            if (right->left != null_node) {
                right->left->parent = node;
            }

            right->left = node;
            right->parent = node->parent;
            if (right->left->key == (*min_)->key) {
                min_ = &right->left;
            }
            node->parent = right;

            if (right->parent != null_node) {
                if (right->parent->right == node) {
                    right->parent->right = right;
                } else {
                    right->parent->left = right;
                }
            } else {
                root = right;
            }
        }

        void rotate_right(detail::Node<T>* root) {
            auto left = root->left;
            if (root == null_node || left == null_node) {
                return;
            }
            root->left = left->right;
            if (left->right != null_node) {
                left->right->parent = root;
            }

            left->right = root;
            left->parent = root->parent;
            if (left->right->key == (*max_)->key) {
                max_ = &left->right;
            }
            root->parent = left;

            if (left->parent != null_node) {
                if (left->parent->right == root) {
                    left->parent->right = left;
                } else {
                    left->parent->left = left;
                }
            } else {
                this->root = left;
            }
        }

        void add_autobalance(detail::Node<T>* node) {
            if (node) {
                if (node == this->root) {
                    node->color = BLACK;
                    return;
                }
                auto parent = node->parent;
                if (parent->color == BLACK) {
                    return;
                } else {
                    auto unc = uncle(node);
                    auto grandpa = grandparent(node);

                    if (unc->color == RED) {
                        unc->color = BLACK;
                        parent->color = BLACK;
                        grandpa->color = RED;
                        add_autobalance(grandpa);
                    } else {
                        if (parent->right == node && grandpa->left == parent) {
                            rotate_left(parent);
                            node = node->left;
                        } else if (
                            parent->left == node && grandpa->right == parent) {
                            rotate_right(parent);
                            node = node->right;
                        }

                        parent = node->parent;
                        grandpa = parent->parent;

                        parent->color = BLACK;
                        grandpa->color = RED;

                        if (parent->left == node && grandpa->left == parent) {
                            rotate_right(grandpa);
                        } else if (
                            parent->right == node && grandpa->right == parent) {
                            rotate_left(grandpa);
                        }
                    }
                }
            }
        }

        void rem_autobalance(detail::Node<T>* parent, detail::Node<T>* node) {
            if (parent == null_node) {
                return;
            }

            auto brother = second_child(parent, node);
            if (brother->color == RED) {
                parent->color = RED;
                brother->color = BLACK;
                if (node == parent->right) {
                    rotate_right(parent);
                } else {
                    rotate_left(parent);
                }
            }

            brother = second_child(parent, node);
            if (brother->color == BLACK &&
                brother->left->color == brother->right->color &&
                brother->left->color == BLACK && parent->color == BLACK) {
                brother->color = RED;
                rem_autobalance(parent->parent, parent);
                return;
            }

            brother = second_child(parent, node);
            if (brother->color == BLACK &&
                brother->left->color == brother->right->color &&
                brother->left->color == BLACK && parent->color == RED) {
                brother->color = RED;
                parent->color = BLACK;
                return;
            }

            brother = second_child(parent, node);
            if (brother->color == BLACK) {
                if (node == parent->left && brother->right->color == BLACK &&
                    brother->left->color == RED) {
                    brother->color = RED;
                    brother->left->color = BLACK;
                    rotate_right(brother);
                } else if (
                    node == parent->right && brother->left->color == BLACK &&
                    brother->left->color == RED) {
                    brother->color = RED;
                    brother->right->color = BLACK;
                    rotate_left(brother);
                }
            }

            brother = second_child(parent, node);
            brother->color = parent->color;
            parent->color = BLACK;

            if (node == parent->left) {
                brother->right->color = BLACK;
                rotate_left(parent);
            } else {
                brother->left->color = BLACK;
                rotate_right(parent);
            }
        }

        bool remove(T key, detail::Node<T>* root) {
            auto node = find(key, root);
            if (node && node->key == key) {
                if (node->left != null_node && node->right != null_node) {
                    auto tmp = min(node->right);
                    node->key = tmp->key;
                    return remove(tmp->key, tmp);
                } else if (node->left != null_node) {
                    auto parent = node->parent;
                    if (parent->left == node) {
                        parent->left = node->left;
                    } else if (parent->right == node) {
                        parent->right = node->left;
                    }
                    node->left->parent = parent;
                    if (parent == null_node) {
                        this->root = node->left;
                    }
                    if (node->color == BLACK) {
                        if (node->left->color == RED) {
                            node->left->color = BLACK;
                        } else {
                            rem_autobalance(parent, node->left);
                        }
                    }
                    delete node;
                    --size_;
                    return true;
                } else {
                    auto parent = node->parent;
                    if (parent->left == node) {
                        parent->left = node->right;
                    } else if (parent->right == node) {
                        parent->right = node->right;
                    }
                    if (node->right != null_node) {
                        node->right->parent = parent;
                    }
                    if (parent == null_node) {
                        root = node->right;
                    }

                    if (node->color == BLACK) {
                        if (node->right->color == RED) {
                            node->right->color = BLACK;
                        } else {
                            rem_autobalance(parent, node->right);
                        }
                    }
                    delete node;
                    --size_;
                    return true;
                }
            }
            return false;
        }

        void copy_nodes(
            detail::Node<T>** newNode,
            const detail::Node<T>* oldNode,
            detail::Node<T>** parent,
            const Set<T>& other) {
            if (oldNode == other.null_node) {
                *newNode = null_node;
                return;
            }

            *newNode =
                new detail::Node<T>(oldNode->key, oldNode->color, *parent);
            size_++;

            copy_nodes(&(*newNode)->left, oldNode->left, newNode, other);
            copy_nodes(&(*newNode)->right, oldNode->right, newNode, other);
            if (oldNode->left == *(other.min_)) {
                min_ = &(*newNode)->left;
            }
            if (oldNode->right == *(other.max_)) {
                max_ = &(*newNode)->right;
            }
        }

        void print_tree(detail::Node<T>* root, std::string path) const {
            if (root == null_node) {
                return;
            }
            print_tree(root->left, path + "l");
            std::cout << root->key << " " << path << std::endl;
            print_tree(root->right, path + "r");
        }

       public:
        Set()
            : root(nullptr),
              null_node(new detail::Node<T>(0, BLACK)),
              min_(nullptr),
              max_(nullptr),
              size_(0) {
            null_node->parent = null_node;
            null_node->left = null_node;
            null_node->right = null_node;
            root = null_node;
            min_ = &null_node;
            max_ = &null_node;
        };

        Set(T key)
            : root(new detail::Node<T>(key, BLACK)),
              null_node(new detail::Node<T>(0, BLACK)),
              min_(&root),
              max_(&root),
              size_(1) {
            null_node->parent = null_node;
            null_node->left = null_node;
            null_node->right = null_node;
            root->left = null_node;
            root->right = null_node;
            root->parent = null_node;
        };

        Set(std::initializer_list<T> list)
            : root(nullptr),
              null_node(new detail::Node<T>(0, BLACK)),
              min_(nullptr),
              max_(nullptr),
              size_(0) {
            null_node->parent = null_node;
            null_node->left = null_node;
            null_node->right = null_node;
            root = null_node;
            min_ = &null_node;
            max_ = &null_node;

            for (const auto& lElem : list) {
                insert(lElem);
            }
        }

        void print() const {
            std::cout << (*min_)->key << std::endl;
            std::cout << (*max_)->key << std::endl;
            print_tree(root, "m");
        }

        Set(const Set<T>& other)
            : root(nullptr),
              null_node(nullptr),
              min_(nullptr),
              max_(nullptr),
              size_(0) {
            if (other.root != nullptr) {
                null_node = new detail::Node<T>(0);
                copy_nodes(&root, other.root, &null_node, other);
            }
        }

        //Оператор копирования:
        Set& operator=(const Set& other) {
            if (this != &other) {
                clear();
                size_ = 0;
                copy_nodes(&root, other.root, &null_node, other);
            }
            return *this;
        };

        //Конструктор перемещения:
        Set(Set&& other)
            : root(other.root),
              null_node(other.null_node),
              min_(other.min_),
              max_(other.max_),
              size_(other.size_) {
            other.root = nullptr;
            other.null_node = nullptr;
            other.min_ = nullptr;
            other.max_ = nullptr;
            other.size_ = 0;
        };

        //Оператор перемещения:
        Set& operator=(Set&& other) {
            if (this != &other) {
                clear();
                root = other.root;
                null_node = other.null_node;
                min_ = other.min_;
                max_ = other.max_;
                size_ = other.size_;
                other.root = nullptr;
                other.null_node = nullptr;
                other.min_ = nullptr;
                other.max_ = nullptr;
                other.size_ = 0;
            }
            return *this;
        };

        ~Set() {
            clear();
            if (null_node) {
                delete null_node;
            }
        }

        void clear() {
            if (size_) {
                clear(root);
            }
        }

        bool contains(T key) const {
            detail::Node<T>* tmp = find(key, root);
            return (tmp && tmp->key == key);
        }

        void erase(T key) {
            remove(key, root);
        }

        bool empty() const {
            return !size_;
        }

        std::size_t size() const {
            return size_;
        }

        template <typename Value_type>
        class Iterator {
           public:
            using difference_type = std::ptrdiff_t;
            using value_type = Value_type;
            using pointer = Value_type*;
            using reference = Value_type&;
            using iterator_category = std::bidirectional_iterator_tag;

            Iterator(
                detail::Node<T>* current,
                detail::Node<T>* prev,
                detail::Node<T>* null_node,
                detail::Node<T>* root)
                : current_(current),
                  prev_(prev),
                  null_node_(null_node),
                  root_(root){};
            Iterator(const Iterator& it)
                : current_(it.current_),
                  prev_(it.prev_),
                  null_node_(it.null_node_),
                  root_(it.root_){};

            Iterator(Iterator&& it)
                : current_(it.current_),
                  prev_(it.prev_),
                  null_node_(it.null_node_),
                  root_(it.root_) {
                it.current_ = nullptr;
                it.prev_ = nullptr;
                it.null_node_ = nullptr;
                it.root_ = nullptr;
            }

            Iterator& operator=(const Iterator& it) {
                this->current_ = it.current_;
                this->prev_ = it.prev_;
                this->null_node_ = it.null_node_;
                this->root_ = it.root_;
                return *this;
            }

            Iterator& operator=(Iterator&& it) {
                current_ = it.current_;
                prev_ = it.prev_;
                null_node_ = it.null_node_;
                root_ = it.root_;
                it.current_ = nullptr;
                it.prev_ = nullptr;
                it.null_node_ = nullptr;
                it.root_ = nullptr;
                return *this;
            }

            ~Iterator() {
            }

            std::pair<Iterator<T>, bool>& operator=(
                const std::pair<Iterator<T>, bool>& other) {
                this->first = other.first;
                this->second = other.second;
                return *this;
            }

            // префиксный инкремент
            Iterator& operator++() {
                if (current_->right != null_node_) {
                    current_ = current_->right;
                    while (current_->left != null_node_) {
                        current_ = current_->left;
                    }
                } else {
                    auto tmp = current_->parent;
                    while (current_ == tmp->right) {
                        current_ = tmp;
                        tmp = tmp->parent;
                    }
                    if (current_->right != tmp) {
                        current_ = tmp;
                    }
                }
                return *this;
            }

            // постфиксный инкремент
            Iterator operator++(int) {
                auto old = *this;
                ++(*this);
                return old;
            }

            Iterator operator+(int value) {
                auto new_iter = *this;
                for (int i = 0; i < value; i++) {
                    new_iter++;
                }
                return new_iter;
            }

            Iterator& operator+=(int value) {
                for (int i = 0; i < value; i++) {
                    (*this)++;
                }
                return *this;
            }

            //префиксный декремент
            Iterator& operator--() {
                if (current_ == null_node_) {
                    auto max = root_;
                    while (max->right != null_node_) {
                        max = max->right;
                    }
                    prev_ = null_node_;
                    current_ = max;
                } else if (
                    prev_ != current_->left && current_->left != null_node_) {
                    prev_ = current_;
                    current_ = current_->left;
                    while (current_->right != null_node_) {
                        prev_ = current_;
                        current_ = current_->right;
                    }
                } else if (
                    prev_ != current_->right && prev_ != current_->left &&
                    current_->right != null_node_) {
                    while (current_->right != null_node_) {
                        prev_ = current_;
                        current_ = current_->right;
                    }

                } else {
                    while (current_ == current_->parent->left &&
                           current_->parent != null_node_) {
                        prev_ = current_;
                        current_ = current_->parent;
                    }
                    if (current_->parent == null_node_) {
                        current_ = null_node_;
                    } else {
                        prev_ = current_;
                        current_ = current_->parent;
                    }
                }
                return *this;
            }

            // постфиксный декремент
            Iterator operator--(int) {
                auto old = *this;
                --(*this);
                return old;
            }

            Iterator operator-(int value) {
                auto new_iter = *this;
                for (int i = 0; i < value; i++) {
                    new_iter--;
                }
                return new_iter;
            }

            Iterator& operator-=(int value) {
                for (int i = 0; i < value; i++) {
                    (*this)--;
                }
                return *this;
            }

            reference operator*() const {
                return current_->key;
            }

            auto operator<=>(const Iterator& rhs) const {
                return current_->key <=> rhs.current_->key;
            }

            bool operator==(const Iterator& rhs) const {
                return current_ == rhs.current_;
            }

           private:
            detail::Node<T>* current_;
            detail::Node<T>* prev_;
            detail::Node<T>* null_node_;
            detail::Node<T>* root_;
        };

        Iterator<T> begin() const {
            return Iterator<T>(*min_, null_node, null_node, root);
        }

        Iterator<T> end() const {
            return Iterator<T>(null_node, null_node, null_node, root);
        }

        Iterator<T> find(const T& key) const {
            auto node = root;
            while (node != null_node) {
                if (node->key < key) {
                    node = node->right;
                } else if (node->key > key) {
                    node = node->left;
                } else {
                    return Iterator<T>(node, null_node, null_node, root);
                }
            }
            return end();
        }

        std::pair<Iterator<T>, bool> insert(T key) {
            if (root == null_node) {
                root = new detail::Node<T>(
                    key, BLACK, null_node, null_node, null_node);
                ++size_;
                min_ = &root;
                max_ = &root;
                return std::make_pair(find(key), true);
            }

            detail::Node<T>* tmp = find(key, root);
            if (tmp != null_node && tmp->key != key) {
                if (tmp->key < key) {
                    tmp->right = new detail::Node<T>(
                        key, RED, tmp, null_node, null_node);
                    if (tmp == *max_) {
                        max_ = &(tmp->right);
                    }
                    add_autobalance(tmp->right);
                } else {
                    tmp->left = new detail::Node<T>(
                        key, RED, tmp, null_node, null_node);
                    if (tmp == *min_) {
                        min_ = &(tmp->left);
                    }
                    add_autobalance(tmp->left);
                }
                ++size_;

                return std::make_pair(find(key), true);
            }
            return std::make_pair(find(key), false);
        }

        Iterator<T> lower_bound(const T& key) const {
            for (auto i = begin(); i != end(); i++) {
                if (*i == key) {
                    return i;
                }
            }
            return end();
        }

        Iterator<T> upper_bound(const T& key) const {
            auto l_bound = lower_bound(key);
            if (l_bound != end()) {
                while (*l_bound == key && l_bound != end()) {
                    l_bound++;
                }
                return l_bound;
            }
            return end();
        }

        std::pair<Iterator<T>, Iterator<T>> equal_range(const T& key) const {
            auto l_bound = lower_bound(key);
            auto u_bound = l_bound;
            if (l_bound != end()) {
                while (*u_bound == key && u_bound != end()) {
                    u_bound++;
                }
                if (*u_bound == *l_bound) {
                    u_bound = end();
                }
                return std::make_pair(l_bound, u_bound);
            }
            return std::make_pair(end(), end());
        }

        void swap(Set<T>& other) {
            std::swap(*this, other);
        }
    };

}  // namespace treeset