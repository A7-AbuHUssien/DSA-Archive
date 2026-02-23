#include <iostream>
#include <cassert>
#include <cmath>

#include <vector>		// for debug
#include <algorithm>
#include <set>
#include <sstream>
using namespace std;

struct Node {
	int data { };
	Node* next { };
	Node(int data) : data(data) {}
	~Node() {
		cout << "Destroy value: " << data <<" at address "<< this<< "\n";
	}
};

class LinkedList {
private:
	Node *head { };
	Node *tail { };
	int length = 0;	// let's maintain how many nodes

	vector<Node*> debug_data;	// add/remove nodes you use

	void debug_add_node(Node* node) {
		debug_data.push_back(node);
	}
	void debug_remove_node(Node* node) {
		auto it = std::find(debug_data.begin(), debug_data.end(), node);
		if (it == debug_data.end())
			cout << "Node does not exist\n";
		else
			debug_data.erase(it);
	}

public:

	// Below 2 deletes prevent copy and assign to avoid this mistake
	LinkedList() {
	}
	LinkedList(const LinkedList&) = delete;
	LinkedList &operator=(const LinkedList &another) = delete;

	void debug_print_address() {
		for (Node* cur = head; cur; cur = cur->next)
			cout << cur << "," << cur->data << "\t";
		cout << "\n";
	}

	void debug_print_node(Node* node, bool is_seperate = false) {
		if (is_seperate)
			cout << "Sep: ";
		if (node == nullptr) {
			cout << "nullptr\n";
			return;
		}
		cout << node->data << " ";
		if (node->next == nullptr)
			cout << "X ";
		else
			cout << node->next->data << " ";

		if (node == head)
			cout << "head\n";
		else if (node == tail)
			cout << "tail\n";
		else
			cout << "\n";
	}
	void debug_print_list(string msg = "") {
		if (msg != "")
			cout << msg << "\n";
		for (int i = 0; i < (int) debug_data.size(); ++i)
			debug_print_node(debug_data[i]);
		cout << "************\n"<<flush;
	}

	string debug_to_string() {
		if (length == 0)
			return "";
		ostringstream oss;
		for (Node* cur = head; cur; cur = cur->next) {
			oss << cur->data;
			if (cur->next)
				oss << " ";
		}
		return oss.str();
	}

	void debug_verify_data_integrity() {
		if (length == 0) {
			assert(head == nullptr);
			assert(tail == nullptr);
		} else {
			assert(head != nullptr);
			assert(tail != nullptr);
			if (length == 1)
				assert(head == tail);
			else
				assert(head != tail);
			assert(!tail->next);
		}
		int len = 0;
		Node* prev = nullptr;
		for (Node* cur = head; cur; prev = cur, cur = cur->next, len++)
			assert(len < 10000);	// Consider infinite cycle?
		assert(tail == prev);
		assert(length == len);
		assert(length == (int)debug_data.size());
	}

	////////////////////////////////////////////////////////////

	void print() {
		for (Node* cur = head; cur; cur = cur->next)
			cout << cur->data << " ";
		cout << "\n";
	}

	// These 2 simple functions just to not forget changing the vector and length
	void delete_node(Node* node) {
		debug_remove_node(node);
		--length;
		delete node;
	}

	void add_node(Node* node) {
		debug_add_node(node);
		++length;
	}

	void insert_end(int value) {
		Node* item = new Node(value);
		add_node(item);

		if (!head)
			head = tail = item;
		else
			tail->next = item, tail = item;
	}

	////////////////////////////////////////////////////////////
	void delete_first() {
		if (head) {
			//Move to next in the list
			// and remove current
			Node* cur = head;
			head = head->next;

			delete_node(cur);

			if (!head)	// data integrity!
				tail = nullptr;
		}
		debug_verify_data_integrity();
	}

	void delete_next_node(Node* node) {
		Node* to_delete = node->next;
		bool is_tail = to_delete == tail;
		// node->next in middle to delete
		node->next = node->next->next;
		delete_node(to_delete);

		if(is_tail)
			tail = node;
	}

	void delete_node_with_key(int value) {	// O(n) time - O(1) memory
		if (!length)
			cout << "Empty list!\n";
		else if (head->data == value)
			delete_first();
		else {
			for (Node *cur = head, *prv = nullptr; cur; prv = cur, cur = cur->next) {
				if(cur->data == value) {
					delete_next_node(prv);	// connect prv with after
					break;
				}
			}
			cout<<"Value not found!\n";
		}
		debug_verify_data_integrity();
	}

	void reverse() {
		if (length <= 1) return;
		tail = head;
		Node*prev =head;
		head = head->next;
		while (head) {
			Node* next = head->next;
			head->next = prev;
			prev = head;
			head = next;
		}
		head = prev;
		tail->next = nullptr;
	}

	void delete_even_positions() {
		Node* cur = head;

		while (cur && cur->next) {
			Node* temp = cur->next;
			cur->next = temp->next;

			if (temp == tail)
				tail = cur;

			delete temp;
			cur = cur->next;
		}
	}
	// 2,3,4,5    (1)
	void insert_to_be_sorted(int value) {
		if (!length) {
			insert_end(value);
			return;
		};


		Node* cur = head->next;
		Node* prev = head;
		Node* newNode = new Node(value);

		if (head->data > value) {
			newNode->next = prev;
			head = newNode;
			return;
		}
		if (value >= tail->data) {
			insert_end(value);
			return;
		}

		while (cur) {
			if (prev->data < value && cur->data > value) {
				prev->next = newNode;
				newNode->next = cur;
				return;
			}
			prev = cur;
			cur = cur->next;
		}
	}

	void swap_head_and_tail() {
		if (length <= 1) return;
		if (length == 2) {
			swap(head, tail);
			head->next = tail;
			tail->next = nullptr;
			return;
		}
		Node* prev_tail = head;
		while (prev_tail->next->next) {
			prev_tail = prev_tail->next;
		}
		prev_tail->next = head;
		tail->next = head->next;
		head->next = nullptr;

		Node* temp = head;
		swap(head, tail);
	}

	void left_rotate(int k) {
		if (k == 0 || k % length == 0) return;
		k%= length;
		Node* wanted_element;
		Node* temp = head;
		for (int i = 0; i < k; i++) {
			wanted_element = temp;
			temp = temp->next;
		}
		tail->next = head;
		tail = wanted_element;
		head = wanted_element->next;
		tail->next = nullptr;
	}
	void remove_duplicates() {
		for (Node* cur1 = head; cur1; cur1 = cur1->next) {
			Node* prev = cur1;
			for (Node* cur2 = cur1->next; cur2; cur2 = cur2->next) {
				if (cur2->data == cur1->data) {
					delete_next_node(prev);
					cur2 = prev;
				}
				else {
					prev = cur2;
				}
			}
		}
	}
	void delete_nth_node(int nth) {
		if (nth == 0) {
			return;
		}
		if (nth > length) {
			cout << "Error! Nth node out of range!\n";
			return;
		}
		if (nth == 1) {
			delete_first(); return;
		}
		nth--;
		Node* cur = head;
		Node* prev = nullptr;
		while (nth >= 0) {
			if (nth == 0) {
				delete_next_node(prev);
			}
			nth--;
			prev = cur;
			cur = prev->next;
		}
	}
	void remove_last_occurance(int k) {
		int nth = 0;
		int counter = 1;
		for (Node* cur = head; cur; cur = cur->next) {
			if (cur->data == k) {
				nth = counter;
			}
			counter++;
		}
		delete_nth_node(nth);
	}
	bool All_next_is_equal(Node* start, int k) {

		while (start != nullptr) {
			if (start->data == k) {
				start = start->next;
			}
			else {
				return false;
			}
		}
		return true;
	}

	void move_to_back(int k) {
		if (length <= 1) {return;}

		Node* prev = head;
		for (Node* cur = head->next; cur->next; cur = cur->next) {
			if (All_next_is_equal(cur,k)) {
				break;
			}
			if (cur->data == k) {
				prev->next = cur->next;
				tail->next = cur;
				cur->next = nullptr;
				tail = cur;

				cur = prev;
			}
			prev = cur;
		}
		if (head->data == k) {
			Node* new_head = head->next;
			tail->next = head;
			head->next = nullptr;

			head = new_head;
		}
	}

	int max(Node* head = nullptr, bool is_first_call = true) {
		if (is_first_call) {
			head = this->head;
		}
		if (head == nullptr) {return INT_MIN;}
		return std::max(head->data, max(head->next,false));
	}
	void arrange_odd_and_even() {
		LinkedList even_list;
		Node* next = nullptr;
		for (Node* cur = head; cur; cur = cur->next) {
			next = cur->next;
			if (next != nullptr) {
				even_list.insert_end(next->data);
				if (next->next) {
					cur->next = next->next;
					delete next;
				}
				else {
					delete next;
					cur->next = even_list.head;
					return;
				}
			}
			else {
				cur->next = even_list.head;
				return;
			}
		}
	}

	void insert_alternating(LinkedList& list2) {
		Node* head1 = head;
		Node* head2 = list2.head;

		if (!head1) { head = head2; return; }
		if (!head2) return;

		while (head1 && head2) {

			Node* temp_next_head1 = head1->next;
			Node* temp_next_head2 = head2->next;

			head1->next = head2;
			if (temp_next_head1 == nullptr)
				head2->next = temp_next_head2;
			else
				head2->next = temp_next_head1;

			head1 = temp_next_head1;
			head2 = temp_next_head2;
		}
	}
	string flip(string& s1,string& s2) {
		string flipped1 = "";
		string flipped2 = "";
		for (int i = s1.length()-1; i >= 0 ; i--) {
			flipped1 += s1[i];
		}
		for (int i = s2.length()-1; i >= 0 ; i--) {
			flipped2 += s2[i];
		}

		int result = stoi(flipped1) + stoi(flipped2);
		return to_string(result);
	}
	void add_two_numbers(LinkedList& list2) {
		LinkedList newList;
		Node* head1 = head;
		Node* head2 = list2.head;
		int carry = 0;
		while (head1 || head2) {
			int sum = carry;
		 	if (head1) {
		 		sum += head1->data;
		 		head1 = head1->next;
		 	}
		 	if (head2) {
		 		sum += head2->data;
				head2 = head2->next;
		 	}
			carry = sum/10;
		 	newList.insert_end(sum % 10);
		 }
		if (carry != 0 ) {
			newList.insert_end(carry);
		}
		head = newList.head;
	}
	// 1 2 3 3 3 4 4
	void remove_duplicates_from_sorted_list() {
		if (!head) return;
		Node* old = head;
		LinkedList new_list;
		new_list.insert_end(head->data);
		int last_value = head->data;
		bool is_tail_deleted = false;
		for (Node* cur = head->next; cur; cur = cur->next) {
			if (last_value == cur->data) {
				if (!is_tail_deleted) {
					new_list.delete_tail();
					is_tail_deleted = true;
				}
			}
			else {
				is_tail_deleted = false;
				last_value = cur->data;
				new_list.insert_end(cur->data);
			}
		}
		while (old) {
			Node* next = old->next;
			delete old;
			old = next;
		}
		head = new_list.head;
	}
	void delete_tail() {
		if (!head) return;

		if (!head->next) {
			delete head;
			head = tail = nullptr;
			return;
		}

		Node* cur = head;
		while (cur->next->next) {
			cur = cur->next;
		}

		delete cur->next;
		cur->next = nullptr;
		tail = cur;
	}

};


void test1() {
	cout << "\n\ntest1\n";
	LinkedList list;

	list.insert_end(1);
	list.insert_end(2);
	list.insert_end(3);
	list.insert_end(4);
	list.insert_end(5);
	list.insert_end(6);
	list.delete_node_with_key(2);
	list.delete_node_with_key(6);
	// some actions
	list.print();

	string expected = "1 3 4 5";
	string result = list.debug_to_string();
	if (expected != result) {
		cout << "no match:\nExpected: " <<
				expected << "\nResult  : " << result << "\n";
		assert(false);
	}
	list.debug_print_list("********");
}
void test2() {
	cout << "\n\ntest2\n";
	LinkedList list;

	list.insert_end(1);
	list.insert_end(2);
	list.delete_node_with_key(10);
	list.delete_node_with_key(1);
	list.delete_node_with_key(2);

	// some actions
	list.print();

	string expected = "";
	string result = list.debug_to_string();
	if (expected != result) {
		cout << "no match:\nExpected: " << expected << "\nResult  : " << result << "\n";
		assert(false);
	}
	list.debug_print_list("********");
}
void test3() {
	cout << "\n\ntest3\n";
	LinkedList list;

	list.insert_end(1);
	list.insert_end(2);
	list.insert_end(3);
	list.insert_end(4);
	list.delete_node_with_key(4);

	// some actions
	list.print();

	string expected = "1 2 3";
	string result = list.debug_to_string();
	if (expected != result) {
		cout << "no match:\nExpected: " << expected << "\nResult  : " << result << "\n";
		assert(false);
	}
	list.debug_print_list("********");
}

int main() {

	LinkedList list;
	list.insert_end(1);
	list.insert_end(1);
	list.insert_end(2);
	list.insert_end(3);
	list.insert_end(3);
	list.insert_end(3);
	list.insert_end(4);
	list.insert_end(4);
	list.insert_end(4);
	list.insert_end(4);

	cout << "List 1: " ; list.print();
	list.remove_duplicates_from_sorted_list();
	cout << "After : " ; list.print();

	return 0;
}