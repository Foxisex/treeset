#pragma once

#include <algorithm>
#include <compare>
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
        std::size_t size_;

        detail::Node<T>* min(detail::Node<T>* node) {
            while (node->left != null_node) {
                node = node->left;
            }
            return node;
        }

        detail::Node<T>* max(detail::Node<T>* node) {
            while (node->right != null_node) {
                node = node->right;
            }
            return node;
        }

        detail::Node<T>* grandparent(detail::Node<T>* node) {
            return node->parent->parent;
        }

        detail::Node<T>* uncle(detail::Node<T>* node) {
            auto grandpa = grandparent(node);
            if (grandpa->left == node->parent) {
                return grandpa->right;
            } else {
                return grandpa->left;
            }
        }

        detail::Node<T>* second_child(
            detail::Node<T>* parent,
            detail::Node<T>* child) {
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

        // void clear(std::shared_ptr<detail::Node<T>> node) {
        //     node = null_node;
        //     node->left = null_node;
        //     node->right = null_node;
        //     size_ = 0;

        //     std::cout << node->key << std::endl;
        // }

        detail::Node<T>* find(T key, detail::Node<T>* node) {
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

       public:
        Set() : root(0), null_node(new detail::Node<T>(0, BLACK)), size_(0) {
            null_node->parent = null_node;
            null_node->left = null_node;
            null_node->right = null_node;
            root = null_node;
        };

        Set(T key)
            : root(new detail::Node<T>(key, BLACK)),
              null_node(new detail::Node<T>(0, BLACK)),
              size_(1) {
            null_node->parent = null_node;
            null_node->left = null_node;
            null_node->right = null_node;
            root->left = null_node;
            root->right = null_node;
            root->parent = null_node;
        };

        void print_tree(detail::Node<T>* root, std::string path) {
            if (root == null_node) {
                return;
            }
            print_tree(root->left, path + "l");
            std::cout << root->key << " " << path << std::endl;
            print_tree(root->right, path + "r");
        }

        void print() {
            std::cout << root << " " << root->key << " " << root->left << " "
                      << root->right << std::endl;
            // print_tree(root, "m");
        }

        // Set(Set& rhs) noexcept {
        //     root = rhs.root;
        //     null_node = rhs.null_node;
        //     size_ = rhs.size_;
        //     rhs.root = nullptr;
        //     rhs.null_node = nullptr;
        //     rhs.size_ = 0;
        // }

        // Set& operator=(const Set& rhs) {
        //     if (this == &rhs) {
        //         return *this;
        //     }
        //     if (size_) {
        //         clear();
        //     }
        //     delete null_node;
        //     root = rhs.root;
        //     null_node = rhs.null_node;
        //     size_ = rhs.size_;

        //     return *this;
        // }

        // Set& operator=(Set rhs) noexcept {
        //     if (this == &rhs) {
        //         return *this;
        //     }
        //     root = std::move(rhs.root);
        //     null_node = std::move(rhs.null_node);
        //     size_ = std::move(rhs.size_);

        //     return *this;
        // }

        //             //Конструктор копирования:
        // Set(const Set& other) : root(0), null_node(new detail::Node<T>(0,
        // BLACK)), size_(other.size_) {
        //             //root = copy(other.root);
        //             null_node->parent = other.null_node->parent;
        //             null_node->left = other.null_node->left;
        //             null_node->right = other.null_node->right;
        //             null_node = other.null_node;
        //             root = copy(other.root, other.null_node);
        //             // for(int i = size)
        //         };
        Set(const Set<T>& other) : root(nullptr), null_node(nullptr), size_(0) {
            if (other.root != nullptr) {
                // создаем новый корневой узел
                // root = new detail::Node<T>(other.root->key,
                // other.root->color);
                null_node = new detail::Node<T>(0);
                // size_ = 1;

                // копируем остальные узлы рекурсивно
                copyNodes(&root, other.root, &null_node, other.null_node);
            }
        }

        void copyNodes(
            detail::Node<T>** newNode,
            const detail::Node<T>* oldNode,
            detail::Node<T>** parent,
            const detail::Node<T>* oth_null) {
            if (oldNode == oth_null) {
                *newNode = null_node;
                return;
            }

            *newNode =
                new detail::Node<T>(oldNode->key, oldNode->color, *parent);
            size_++;

            copyNodes(&(*newNode)->left, oldNode->left, newNode, oth_null);
            copyNodes(&(*newNode)->right, oldNode->right, newNode, oth_null);
        }

        //Оператор присваивания:
        Set& operator=(const Set& other) {
            if (this != &other) {
                clear();
                size_ = other.size_;
                root = (other.root);
            }
            return *this;
        };

        //Конструктор перемещения:
        Set(Set&& other)
            : root(other.root), null_node(other.null_node), size_(other.size_) {
            other.root = nullptr;
            other.null_node = nullptr;
            other.size_ = 0;
        };

        //Оператор перемещения:
        Set& operator=(Set&& other) {
            if (this != &other) {
                clear();
                root = other.root;
                null_node = other.null_node;
                size_ = other.size_;
                other.root = nullptr;
                other.null_node = nullptr;
                other.size_ = 0;
            }
            return *this;
        };

        virtual ~Set() {
            std::cout << "VERTOLET" << this << std::endl;
            if (size_) {
                clear();
            }
            if (null_node) {
                delete null_node;
            }
        }

        detail::Node<T>* copy(
            detail::Node<T>* root,
            detail::Node<T>* null_node) {
            if (root == null_node) {
                return null_node;
            }
            detail::Node<T>* node(0);
            node->left = copy(root->left, null_node);
            node->right = copy(root->right, null_node);

            return node;
        }

        void clear() {
            clear(root);
        }

        // void clear() {
        //     root = null_node;
        //     size_ = 0;
        //     std::cout << root->key << std::endl;
        // }

        bool contains(T key) {
            detail::Node<T>* tmp = find(key, root);
            return (tmp && tmp->key == key);
        }

        void erase(T key) {
            remove(key, root);
        }

        bool empty() {
            return !size_;
        }

        size_t size() {
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

            const Iterator& operator++() const {
                return ++(const_cast<Iterator>(*this));
            }

            // постфиксный инкремент
            const Iterator operator++(int) {
                auto old = *this;
                ++(*this);
                return old;
            }

            //префиксный декремент
            Iterator operator--() {
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

            const Iterator& operator--() const {
                return --(const_cast<Iterator>(*this));
            }

            // постфиксный декремент
            const Iterator operator--(int) {
                auto old = *this;
                --(*this);
                return old;
            }

            reference operator*() const {
                return current_->key;
            }

            bool operator<=>(const Iterator& rhs) const {
                return current_ <=> rhs.current_;
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

        const Iterator<T> begin() const {
            detail::Node<T>* min = root;
            if (min == null_node) {
                return Iterator<T>(null_node, null_node, null_node, null_node);
            }
            while (min->left != null_node) {
                min = min->left;
            }
            return Iterator<T>(min, null_node, null_node, root);
        }

        const Iterator<T> end() const {
            return Iterator<T>(null_node, null_node, null_node, root);
        }

        Iterator<T> find(const T& key) {
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
                return std::make_pair(find(key), true);
            }

            detail::Node<T>* tmp = find(key, root);
            if (tmp != null_node && tmp->key != key) {
                if (tmp->key < key) {
                    tmp->right = new detail::Node<T>(
                        key, RED, tmp, null_node, null_node);
                    add_autobalance(tmp->right);
                } else {
                    tmp->left = new detail::Node<T>(
                        key, RED, tmp, null_node, null_node);
                    add_autobalance(tmp->left);
                }
                ++size_;
                return std::make_pair(find(key), true);
            }
            return std::make_pair(find(key), false);
        }
    };

}  // namespace treeset