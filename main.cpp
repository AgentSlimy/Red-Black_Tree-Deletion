//Author: Nathan Zou
//Date: 5/29/22
//Red Black Tree - Deletion, part 2 of Red-Black Tree project, delete nodes and search for values
//Done with help from: Chris Zou and Stefan Ene
/* Other resources used:
   Previously completed Red-Black_Tree-Insertion: https://github.com/AgentSlimy/Red-Black_Tree-Insertion
   Deletion: https://www.geeksforgeeks.org/red-black-tree-set-3-delete-2/
             https://www.programiz.com/dsa/deletion-from-a-red-black-tree
*/

#include <iostream>
#include <cstring>
#include <fstream>
#include "node.h"

//Colors
#define RESET "\033[0m"
#define RED   "\033[31m"
#define BLUE  "\033[34m" //Substitute for black

using namespace std;

struct Trunk { //Used for printing
    Trunk* previous;
    char* str;

    Trunk(Trunk* previous, char* str) {
        this->previous = previous;
        this->str = str;
    }
};

void showTrunks(Trunk* p) { //Uesd for printing
    if (p == NULL)
        return;

    showTrunks(p->previous);

    cout << p->str;
}

//Basic Tree Functions - Insertion
void PARSE(char* in, int* modify, int& count);
void ADD(Node*& head, Node*& current, Node*& previous, int value);
void FADD(Node*& head);
void PRINT(Node* root, Trunk* previous, bool isLeft);
//Basic Tree Functions - Deletion
bool SEARCH(Node* current, int& data);
void DELETE(Node* &head, Node* &v);

//For Insertion
void BALANCE(Node*& head, Node*& current);
void rotateLeft(Node*& head, Node*& current);
void rotateRight(Node*& head, Node*& current);

//For Deletion
Node* getSibling(Node* &x);
Node* successor(Node* &x);
Node* replaceNode(Node* &x);
bool hasRedChild(Node* &x);
void fixDoubleBlack(Node* &head, Node* &x);
void swapNodeValues(Node* &u, Node* &x);

int main() {
    cout << "Red-Black Tree: Deletion" << endl << endl;
    bool running = true;
    char command[15];
    int searchInput;
    Node* head = NULL;
    while (running == true) {
        cout << "Options: Add, FAdd, Print, Search, Delete, Quit" << endl;
        cin.get(command, 15);
        cin.clear();
        cin.ignore(10000, '\n');
        if (strcmp(command, "Add") == 0) { //Add command, manually add a single value to the tree
            int value;
            cout << "Add value >> ";
            cin >> value;
            cin.clear();
            cin.ignore(10000, '\n');
            Node* current = head;
            Node* previous = NULL;
            ADD(head, current, previous, value);
            if (current != head) {
                BALANCE(head, current);
            }
            cout << endl << value << " added to tree." << endl << endl;
        }
        else if (strcmp(command, "FAdd") == 0) { //File add command, add values from a file to the tree
            FADD(head);
        }
        else if (strcmp(command, "Print") == 0) { //Print command, print out tree
            cout << endl;
            PRINT(head, NULL, false);
            cout << endl;
        }
	else if (strcmp(command, "Search") == 0) { //Search command, search for values in tree
	  if (head == NULL) {
	    cout << endl << "Tree is empty, no values to search for." << endl << endl;
	  }
	  else {
	    cout << "Search Value >> ";
	    cin >> searchInput;
	    cin.clear();
	    cin.ignore(10000, '\n');
	    bool present = SEARCH(head, searchInput);
      if (present == 1) {
        cout << "The element is present in the list!" << endl << endl; 
      }
      else {
        cout << "The element is not present in the list!" << endl << endl;
      }
	  }
	}
	else if (strcmp(command, "Delete") == 0) { //Delete command, delete values from tree
	  if (head == NULL) {
	    cout << endl << "Tree is empty, no values to delete." << endl << endl;
	  }
	  else {
	    int value = 0;
	    cout << "Value to Delete >> ";
	    cin >> value;
	    cin.clear();
	    cin.ignore(10000, '\n');
	    bool checkValue = SEARCH(head, value);
	    if (checkValue == 0) {
	      cout << endl;
	    }
	    else {
	      //Find node and delete
	      Node* v = head;
	      while (v->getData() != value) {
		if (value < v->getData()) {
		  v = v->getLeft();
		}
		else if (value > v->getData()) {
		  v = v->getRight();
		}
	      }
	      DELETE(head, v);
        cout << endl;
	    }
	  }
	}
        else if (strcmp(command, "Quit") == 0) { //Quit command, quits
            cout << endl << "Quitting Red-Black Tree: Deletion Edition" << endl;
            running = false;
        }
        else {
            cout << "Invalid input, try again." << endl << endl;
        }
    }
}

void ADD(Node*& head, Node*& current, Node*& previous, int value) { //Add function, manually add in a node
    if (head == NULL) {
        head = new Node();
        current = head;
        head->setData(value);
        head->setColor(0); //Head is always black
    }
    else {
        if (value < current->getData()) {
            previous = current;
            current = current->getLeft();
            if (current == NULL) {
                current = new Node();
                current->setData(value);
                previous->setLeft(current);
                current->setParent(previous);
                BALANCE(head, current);
                return;
            }
            else {
                ADD(head, current, previous, value);
            }
        }
        else {
            previous = current;
            current = current->getRight();
            if (current == NULL) {
                current = new Node();
                current->setData(value);
                previous->setRight(current);
                current->setParent(previous);
                BALANCE(head, current);
                return;
            }
            else {
                ADD(head, current, previous, value);
            }
        }
    }
}

void FADD(Node*& head) { //File Add function, take in input from a file
    char input[10000];
    char fileName[100];
    int modify[100];
    int modSize = 0;
    int count = 0;
    memset(input, 0, 10000);
    memset(fileName, 0, 100);
    memset(modify, 0, 100);

    cout << endl << "What is the name of the file? ";
    cin.get(fileName, 30);
    cin.clear();
    cin.ignore(100000, '\n');
    streampos size;
    ifstream file(fileName, ios::in | ios::binary | ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        file.seekg(0, ios::beg);
        file.read(input, size);
        file.close();
        PARSE(input, modify, count); //Parse input to modify array
        cout << "Input: ";
        for (int i = 0; i < 100; i++) {
            if (modify[i] != 0) {
                cout << modify[i] << " ";
                modSize++;
            }
        }
        cout << endl << "Size: " << modSize << endl;
        Node* current = NULL;
        Node* previous = NULL;
        for (int i = 0; i < modSize; i++) {
            if (modify[i] == 0) break;
            current = head;
            ADD(head, current, previous, modify[i]);
        }
    }
}

void PRINT(Node* root, Trunk* previous, bool isLeft) { //Print functions, prints out tree
    if (root == NULL) {
        return;
    }
    char* prevStr = (char*)("    ");
    Trunk* trunk = new Trunk(previous, prevStr);
    PRINT(root->getLeft(), trunk, true);
    //Formatting tree
    if (!previous) {
        trunk->str = (char*)("---");
    }
    else if (isLeft) {
        trunk->str = (char*)(".---");
        prevStr = (char*)("   |");
    }
    else {
        trunk->str = (char*)("'---");
        previous->str = prevStr;
    }
    showTrunks(trunk);
    if (root->getColor() == 0) { //if Black
        cout << BLUE << root->getData() << RESET << endl;
    }
    else { //Red
        cout << RED << root->getData() << RESET << endl;
    }
    if (previous) {
        previous->str = prevStr;
    }
    trunk->str = (char*)("   |");
    PRINT(root->getRight(), trunk, false);
}

void PARSE(char* in, int* modify, int& count) { //Parse function, takes in input
    int x = 0;  //keeps track of # of chars before space
    for (int i = 0; i < strlen(in); i++) {
        if (in[i] == ' ') {
            if (x == 1) {
                int temp = 0;
                temp = in[i - 1] - '0';
                modify[count] = temp;
                count++;
                x = 0;
            }
            else {
                int temp = 0;
                for (int a = 0; a < x; a++) {
                    temp = 10 * temp + (in[i - x + a] - '0');
                }
                modify[count] = temp;
                count++;
                x = 0;
            }
        }
        else {
            int temp = 0;
            x++;
            if (i == strlen(in) - 1) {  //last possible pair of chars
                for (int a = 0; a < x; a++) {
                    temp = 10 * temp + (in[i + a + 1 - x] - '0');
                }
                modify[count] = temp;
                count++;
            }
        }
    }
}

bool SEARCH(Node* current, int& data) { //Search function, used to find a specific value in a given tree
  while (current->getData() != data && current != NULL) {
    if (current != NULL) {
      if (current->getData() > data) {
        current = current->getLeft();
      }
      else {
	current = current->getRight();
      }
    }
    if (current == NULL) {
      break;
    }
  }
  if (current != NULL) {
    if (current->getData() == data) { //Number is in the list
      return 1;
      return true;
    }
  }
  else { //Number is not in the list
    return 0;
    return false;
  }
  return 0;
}

void BALANCE(Node*& head, Node*& current) { //Balance function, for Red-Black Tree properties
    Node* parent = NULL;
    Node* grandParent = NULL;
    while ((current != head) && (current->getColor() != 0) && ((current->getParent())->getColor() == 1)) {
        parent = current->getParent();
        grandParent = parent->getParent();
        //Case A: Parent = left child of grandparent
        if (parent == grandParent->getLeft()) {
            Node* uncle = grandParent->getRight();
            //Case 1A: Uncle = red, only recolor
            if (uncle != NULL && uncle->getColor() != 0) {
                grandParent->setColor(1); //Red
                parent->setColor(0); //Black
                uncle->setColor(0); //Black
                current = grandParent;
            }
            else {
                //Case 2A: Current = right child of parent, rotate left
                if (current == parent->getRight()) {
                    rotateLeft(head, parent);
                    current = parent;
                    parent = current->getParent();
                }
                //Case 3A: Current = left child of parent, rotate right
                rotateRight(head, grandParent);
                //swap colors of parent and grandparent
                int tempCol = parent->getColor();
                parent->setColor(grandParent->getColor());
                grandParent->setColor(tempCol);
                current = parent;
            }
        }
        //Case B: Parent = right child of grandparent
        else {
            Node* uncle = grandParent->getLeft();
            //Case 1B: Uncle = red, only recolor
            if (uncle != NULL && uncle->getColor() != 0) {
                grandParent->setColor(1); //Red
                parent->setColor(0); //Black
                uncle->setColor(0); //Black
                current = grandParent;
            }
            else {
                //Case 2B: Current = left child of parent, rotate right
                if (current == parent->getLeft()) {
                    rotateRight(head, parent);
                    current = parent;
                    parent = current->getParent();
                }
                //Case 3B: Current = right child of parent, rotate left
                rotateLeft(head, grandParent);
                //swap color of parent and grandparent
                int tempCol = parent->getColor();
                parent->setColor(grandParent->getColor());
                grandParent->setColor(tempCol);
                current = parent;
            }
        }
    }
    head->setColor(0); //head is black
}

void rotateLeft(Node*& head, Node*& current) { //Rotate Left
    Node* rightPointer = current->getRight();
    //begin rotation
    current->setRight(rightPointer->getLeft());
    if (current->getRight() != NULL) {
        (current->getRight())->setParent(current);
    }
    rightPointer->setParent(current->getParent());
    //if working with head
    if (current->getParent() == NULL) {
        head = rightPointer;
    }
    else if (current == (current->getParent())->getLeft()) {
        (current->getParent())->setLeft(rightPointer);
    }
    else {
        (current->getParent())->setRight(rightPointer);
    }
    rightPointer->setLeft(current);
    current->setParent(rightPointer);
}

void rotateRight(Node*& head, Node*& current) { //Rotate Right
    Node* leftPointer = current->getLeft();
    //being rotation
    current->setLeft(leftPointer->getRight());
    if (current->getLeft() != NULL) {
        (current->getLeft())->setParent(current);
    }
    leftPointer->setParent(current->getParent());
    //if working with head
    if (current->getParent() == NULL) {
        head = leftPointer;
    }
    else if (current == (current->getParent())->getLeft()) {
        (current->getParent())->setLeft(leftPointer);
    }
    else {
        (current->getParent())->setRight(leftPointer);
    }
    leftPointer->setRight(current);
    current->setParent(leftPointer);
}

void DELETE(Node* &head, Node* &x) {
  Node* u = replaceNode(x);
  Node* parent = x->getParent();
  //Bool to track if both are black
  bool bothBlack = ((u == NULL || u->getColor() == 0) && (x == NULL || x->getColor() == 0));

  //If x has no children
  if (u == NULL) {
    if (x == head) {
      head = NULL;
    }
    else {
      if (bothBlack) {
	//fixDoubleBlack();
      }
      else {
	//One is red -> make sibling red
	if (getSibling(x) != NULL) {
	  getSibling(x)->setColor(1);
	}
      }
      //Delete x from tree
      if (x == parent->getLeft()) {
	parent->setLeft(NULL);
      }
      else {
	parent->setRight(NULL);
      }
    }
    x->~Node();
    return;
  }

  //If x has 1 child
  if (x->getRight() == NULL || x->getLeft() == NULL) {
    if (x == head) {
      //Value of u goes to x
      x->setData(u->getData());
      x->setLeft(NULL);
      x->setRight(NULL);
      //Delete u
      u->~Node();
    }
    else {
      //Detach x from tree and move u up
      if (x == parent->getLeft()) {
	parent->setLeft(u);
      }
      else {
	parent->setRight(u);
      }
      //Delete x
      x->~Node();
      u->setParent(parent);
      if (bothBlack) {
	//fixDoubleBlack();
      }
      else {
	//If one is red, color u black
	u->setColor(0);
      }
    }
    return;
  }

  //if x has 2 children
  //swapNodeValues();
  DELETE(head, u);
}

Node* getSibling(Node* &x) {
  if (x->getParent() == NULL) {
    return NULL;
  }
  if (x == x->getParent()->getLeft()) {
    return x->getParent()->getRight();
  }
  else {
    return x->getParent()->getLeft();
  }
}

Node* successor(Node* &x) {
  //Get the left most value of right subtree
  Node* a = x;
  while (a->getLeft() != NULL) {
    a = a->getLeft();
  }
  return a;
}

Node* replaceNode(Node* &x) {
  //If node has 2 children
  if (x->getLeft() != NULL && x->getRight() != NULL) {
    Node* right = x->getRight();
    return successor(right);
  }
  //If node has no children
  else if (x->getLeft() == NULL && x->getRight() == NULL) {
    return NULL;
  }
  //If node has 1 child
  else {
    if (x->getLeft() != NULL) {
      return x->getLeft();
    }
    else {
      return x->getRight();
    }
  }
}
