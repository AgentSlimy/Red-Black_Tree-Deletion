#ifndef NODE_H
#define NODE_H

using namespace std;

class Node {
 public:
  Node(); //Constructor
  ~Node(); //Destructor
  //Getters
  Node* getLeft();
  Node* getRight();
  Node* getParent();
  int getData();
  int getColor();
  //Setters
  void setLeft(Node*);
  void setRight(Node*);
  void setParent(Node*);
  void setData(int);
  void setColor(int);
 private:
  Node* left;
  Node* right;
  Node* parent;
  int data;
  int color; // 0 = black, 1 = red
};
#endif
