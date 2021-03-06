#ifndef __GV_RBTREE_H__
#define __GV_RBTREE_H__


#include "gv_platform.h"

GV_NS_BEGIN

#define GV_RB_RED   0
#define GV_RB_BLACK 1

class rbtree {
public:
	struct node {
		unsigned long _parent_color;
		node *_right;
		node *_left;

		unsigned color() const noexcept {
			return _parent_color & 1;
		}

		bool is_red() const noexcept {
			return !(_parent_color & 1);
		}

		bool is_black() const noexcept {
			return _parent_color & 1;
		}

		void set_red() noexcept {
			_parent_color &= ~1;
		}

		void set_black() noexcept {
			_parent_color |= 1;
		}

        node *parent() noexcept {
            return (node*)(_parent_color & ~3);
        }
            
        void parent(node *parent) noexcept {
			_parent_color = (_parent_color & 3) | (unsigned long)parent;
		}

		void set_color(int color) noexcept {
			_parent_color = (_parent_color & ~1) | color;
		}

		node *next() noexcept {
			register node *parent, *node;

			if (this->parent() == this){
				return nullptr;
			}

			if ((node = _right)){
				while (node->_left){
					node = node->_left;
				}
				return node;
			}

			node = this;
			while ((parent = node->parent())&& node == parent->_right){
				node = parent;
			}

			return parent;
		}

		node *prev() noexcept {
			register node *parent, *node;

            if (this->parent() == node){
				return nullptr;
			}

			if ((node = _left)){
				while (node->_right){
					node = node->_right;
				}
				return node;
			}

			node = this;
			while ((parent = node->parent())&& node == parent->_left){
				node = parent;
			}

			return parent;
		}
	};

private:
	void rotate_left(register node *n) noexcept;
	void rotate_right(register node *n) noexcept;
	void erase_color(node *n, node *parent) noexcept;

public:
	rbtree() noexcept : _root(){}

	bool empty() const noexcept {
		return !_root;
	}

	void init() noexcept {
		_root = nullptr;
	}

	node *root() noexcept {
		return _root;
	}

	node *front() const noexcept {
		register node *node = _root;
		if (node){
			while (node->_left){
				node = node->_left;
			}
		}
		return node;
	}

	node *back() const noexcept {
		register node *node = _root;
		if (node){
			while (node->_right){
				node = node->_right;
			}
		}
		return node;
	}

	static void link(node *n, node *parent, node **link) noexcept {
		n->_parent_color = (unsigned long)parent;
		n->_left = n->_right = nullptr;
		*link = n;
	}

	void insert(node *n) noexcept ;
	void remove(node *n) noexcept ;
	void replace(node *victim, node *new_node) noexcept ;

public:
	node *_root;
};


GV_NS_END


#endif
