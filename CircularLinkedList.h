#pragma once
template<typename T>
struct CircularLinkedList {
	CircularLinkedList() : size(0) {}
	template<typename T>
	struct CircularLinkedListNode {
		T data;
		CircularLinkedListNode() {};
		CircularLinkedListNode<T>* next = nullptr;
		CircularLinkedListNode<T>* previous = nullptr;
		CircularLinkedListNode<T>* get_next() const {
			return next;
		}
		CircularLinkedListNode<T>* get_previous() const {
			return previous;
		}
		T& get_data() {
			return data;
		}
	};
	std::size_t size = 0;
	CircularLinkedListNode<T>* head = nullptr;
	CircularLinkedListNode<T>* get_head() const {
		return head;
	}
	void add(T data) {
		if (head == nullptr) {
			head = new CircularLinkedListNode<T>();
			head->data = data;
			head->next = head;
			head->previous = head;
			++size;
		} else {
			auto new_node = new CircularLinkedListNode<T>();
			new_node->data = data;
			new_node->previous = head->previous;
			new_node->next = head;
			head->previous->next = new_node;
			head->previous = new_node;
			++size;
		}
	}
	void remove(T& data) {
		auto ptr = head;
		for (int i = 0; i < size; ++i) {
			if (ptr->data == data) {
				if (ptr == head) {
					if (head->next == head) {
						delete head;
						head = nullptr;
					} else {
						head->previous->next = head->next;
						head->next->previous = head->previous;
						auto tmp = head;
						head = head->next;
						delete tmp;
					}
				}
				--size;
				return;
			}
			ptr = ptr->next;
		}
	}
};