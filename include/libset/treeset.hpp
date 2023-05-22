#pragma once

#include <iostream>

namespace treeset {

    const bool BLACK = false;
    const bool RED = true;

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

        auto operator<=>(const Node<T>& rhs) const {
            return key <=> rhs.key;
        }
    };

    template <typename T>
    class Set {
       private:
        Node<T>* root;
        Node<T>* null_node;
        std::size_t size;

       public:
        Set() : root(0), null_node(new Node<T>(0, BLACK)), size(0) {
            null_node->parent = null_node;
            null_node->left = null_node;
            null_node->right = null_node;
            root = null_node;
        };
        Set(T key) : root(key), null_node(new Node<T>(0, BLACK)), size(1){};
        ~Set() {
            if (size) {
                clear();
            }
            delete null_node;
        }

        Node<T>* grandparent(Node<T>* node) {
            return node->parent->parent;
        }

        Node<T>* uncle(Node<T>* node) {
            auto grandpa = grandparent(node);
            if (grandpa->left == node->parent) {
                return grandpa->right;
            } else {
                return grandpa->left;
            }
        }

        Node<T>* second_child(Node<T>* parent, Node<T>* child) {
            if (parent->left == child) {
                return parent->right;
            } else if (parent->right == child) {
                return parent->left;
            } else {
                return null_node;
            }
        }
        void clear(Node<T>* node) {
            if (node == null_node) {
                return;
            }
            clear(node->left);
            clear(node->right);
            --size;
            delete node;
            node = null_node;
        }
        void clear() {
            clear(root);
        }
        Node<T>* find(T key, Node<T>* node);

        bool contains(T key) {
            Node<T>* tmp = find(key, root);
            return (tmp && tmp->key == key);
        }

        void rotate_left(Node<T>* node) {
            Node<T>* right = node->right;
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

        void rotate_right(Node<T>* root) {
            Node<T>* left = root->left;
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

        void add_autobalance(Node<T>* node) {
            if (node) {
                if (node == this->root) {
                    node->color = BLACK;
                    return;
                }
                Node<T>* parent = node->parent;
                if (parent->color == BLACK) {
                    return;
                } else {
                    Node<T>* unc = uncle(node);
                    Node<T>* grandpa = grandparent(node);

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

        void rem_autobalance(Node<T>* parent, Node<T>* node) {
            if (parent == null_node) {
                return;
            }

            Node<T>* brother = second_child(parent, node);
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

        void add(T key) {
            if (root == null_node) {
                root = new Node<T>(key, BLACK, null_node, null_node, null_node);
                ++size;
                return;
            }

            Node<T>* tmp = find(key, root);
            if (tmp != null_node && tmp->key != key) {
                if (tmp->key < key) {
                    tmp->right =
                        new Node<T>(key, RED, tmp, null_node, null_node);
                    add_autobalance(tmp->right);
                } else {
                    tmp->left =
                        new Node<T>(key, RED, tmp, null_node, null_node);
                    add_autobalance(tmp->left);
                }
                ++size;
            }
        }

        Node<T>* min(Node<T>* node) {
            while (node->left != null_node) {
                node = node->left;
            }
            return node;
        }

        Node<T>* max(Node<T>* node) {
            while (node->right != null_node) {
                node = node->right;
            }
            return node;
        }

        bool remove(T key, Node<T>* root) {
            Node<T>* node = find(key, root);
            if (node && node->key == key) {
                if (node->left != null_node && node->right != null_node) {
                    Node<T>* tmp = min(node->right);
                    node->key = tmp->key;
                    return remove(tmp->key, tmp);
                } else if (node->left != null_node) {
                    Node<T>* parent = node->parent;
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
                    --size;
                    return true;
                } else {
                    Node<T>* parent = node->parent;
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
                    --size;
                    return true;
                }
            }
            return false;
        }

        void erase(T key) {
            remove(key, root);
        }

        void print_tree(Node<T>* root, std::string str) {
            if (root->left != null_node) {
                print_tree(root->left, std::string(str + "l"));
            }
            std::cout << root->color << " " << str << ": " << root->key
                      << std::endl;
            if (root->right != null_node) {
                print_tree(root->right, std::string(str + "r"));
            }
        }
        void print_tree() {
            std::cout << size << std::endl;
            print_tree(root, "m");
        }
    };
    // Set() : root(nullptr);

}  // namespace treeset