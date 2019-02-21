#include <cmath>
#include <iostream>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

//----------------------------------------------------------------------------
//CLASS BINTREE
//----------------------------------------------------------------------------

template <typename T, typename V>
class BinTree {
  struct Node {
    std::pair<const T, V> pair;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    Node* upper;

    Node(const std::pair<const T, V>& p, Node* l, Node* r,Node *u) :
      pair{p},
      left{l},
      right{r},
      upper{u} {
      std::cout << "Node ctor" << std::endl;
    }

    ~Node() noexcept = default;
  };

  std::unique_ptr<Node> root;

public:
  
  BinTree() = default;

  BinTree(const BinTree& bst);

  BinTree& operator=(const BinTree& bst);

  BinTree(BinTree&& bst) noexcept : root{std::move(bst.root)} {
    std::cout << "calling move ctor" << std::endl;
  }

  BinTree& operator=(BinTree&& bst) noexcept {
    std::cout << "calling move assignment" << std::endl;
    root = std::move(bst.root);
    return *this;
  }

  void insert(const std::pair<const T, V>& pair);

  void insert_more(const std::pair<const T, V>& pair, Node* comp);

  void clear() noexcept;

  class Iterator;

  class ConstIterator;

  Iterator begin();

  Iterator end() noexcept { return Iterator{nullptr}; };
  
  ConstIterator begin() const;

  ConstIterator end() const { return ConstIterator{nullptr}; };
  
  ConstIterator cbegin() const; 

  ConstIterator cend() const { return ConstIterator{nullptr}; };

  Iterator find(const T& key);

  ConstIterator find(const T& key) const;

  Iterator recfind(Node* subroot, const T& key);

  void balance();

  void balance(std::vector<std::pair<const T, V>>& aux, int left, int right);

  void deepcopy(Node*before, std::unique_ptr<Node>& after);

  template <typename ot, typename ov>
  friend std::ostream& operator<<(std::ostream&, const BinTree<ot, ov>&);
};


//--------------------------------------------------------------------------------
//CLASS ITERATOR
//---------------------------------------------------------------------------------

template <typename T, typename V>
class BinTree<T, V>::Iterator : public std::iterator<std::forward_iterator_tag,std::pair<const T, V>> {
  using Node = BinTree<T, V>::Node;                

  Node* current;

public:
  Iterator(Node* n) : current{n} {}

  std::pair<const T, V>& operator*() const { return current->pair; }

  Iterator &operator++() {
    if (current->right != nullptr) { 
      current = current->right.get();
      while (current->left != nullptr)
        current =
            current->left.get();
    } else {
      const T& key =
          current->pair.first;
      while (current != nullptr && key >= current->pair.first)
        current = current->upper;
    }
    return *this;
  }

  Iterator operator++(int) {
    Iterator it{*this};
    ++(*this);
    return it;
  }

  bool operator==(const Iterator& other) { return current == other.current; }
  bool operator!=(const Iterator& other) { return !(*this == other); }

};


//-----------------------------------------------------------------------------
//CLASS CONST_ITERATOR
//-----------------------------------------------------------------------------

template <typename T, typename V>
class BinTree<T, V>::ConstIterator : public BinTree<T, V>::Iterator {

public:
  using parent = BinTree<T, V>::Iterator;
  using parent::Iterator;

  const std::pair<const T, V>& operator*() const { return parent::operator*(); }
};


//--------------------------------------------------------------------------------
//INSERT
//--------------------------------------------------------------------------------

template <typename T, typename V>
void BinTree<T, V>::insert(const std::pair<const T, V>& p) {
  if (root == nullptr) {
    root.reset(new Node{p, nullptr, nullptr, nullptr});
  } else
    insert_more(p, root.get());
}

template <typename T, typename V>
void BinTree<T, V>::insert_more(const std::pair<const T, V>& p, Node* comp) {

  if (p.first < comp->pair.first) {
    if (comp->left == nullptr) {
      comp->left.reset(new Node{p, nullptr, nullptr, comp});
      return;                                   
    } else
      comp = comp->left.get();
  }

  else if (p.first == comp->pair.first) {
    comp->pair.second = p.second;
    return;
  }

  else if (p.first > comp->pair.first) {
    if (comp->right == nullptr) {
      comp->right.reset(new Node{p, nullptr, nullptr, comp});
      return;
    } else
      comp = comp->right.get();
  }
  insert_more(p, comp);
}

//--------------------------------------------------------------------------------------
//CLEAR
//---------------------------------------------------------------------------------------

template <typename T, typename V> void BinTree<T, V>::clear() noexcept {
  root.reset();
  std::cout << "inside clear" << std::endl;
}

//----------------------------------------------------------------------------------------
//FIND
//-----------------------------------------------------------------------------------------

template <typename T, typename V>
typename BinTree<T, V>::Iterator BinTree<T, V>::find(const T& key) {
  if (root == nullptr)
    return end();
  return recfind(root.get(), key);
}

template <typename T, typename V>
typename BinTree<T, V>::Iterator BinTree<T, V>::recfind(Node* subroot, const T& key) {

  if (key == subroot->pair.first) {
    std::cout << "key found" << std::endl;
    return Iterator{subroot};
  }

  if (key > subroot->pair.first) {
    if (subroot->right != nullptr)
      return recfind(subroot->right.get(), key);
    else {
      std::cout << "key not found R" << std::endl;
      return end();
    }
  }

  if (subroot->left != nullptr)
    return recfind(subroot->left.get(), key);
  else {
    std::cout << "key not found L" << std::endl;
    return end();
  }
}

//-------------------------------------------------------------------------------
//<< OPERATOR
//--------------------------------------------------------------------------------

template <typename T, typename V>
std::ostream& operator<<(std::ostream& os, BinTree<T, V>& bst) {
  for (const auto &x : bst)             
    os << x.first << ": " << x.second << std::endl;
  return os;
}

//--------------------------------------------------------------------------------------
//BEGIN
//--------------------------------------------------------------------------------------

template <typename T, typename V>
typename BinTree<T, V>::Iterator BinTree<T, V>::begin() {
  auto tmp = root.get();
  while (tmp->left != nullptr) {
    tmp = tmp->left.get();
  }
  return Iterator{tmp};
}

//-------------------------------------------------------------------------------------
//CBEGIN
//-------------------------------------------------------------------------------------

template <typename T, typename V>
typename BinTree<T, V>::ConstIterator BinTree<T, V>::cbegin() const {
  if (root == nullptr)
    return ConstIterator{nullptr};
  auto tmp = root.get();
  while (tmp->left != nullptr) {
    tmp = tmp->left.get();
  }
  return ConstIterator{tmp};
}

//---------------------------------------------------------------------------------------
//BALANCE
//---------------------------------------------------------------------------------------

template <typename T, typename V>
void BinTree<T, V>::balance() {

  if (!root) {
    std::cout << "You're trying to balance an empty tree" << std::endl;
    return;
  }
  std::vector<std::pair<const T, V>> aux;
  Iterator iter{begin()};
  for (; iter != nullptr; ++iter)
    aux.push_back(*iter);
  clear();
  balance(aux, 0, aux.size() - 1); // chiamo la funzione (privata) balance
}

template <typename T, typename V>
void BinTree<T, V>::balance(std::vector<std::pair<const T, V>>& aux, int left, int right) {

  int median;

  if (left > right)
    return;
  median =
      floor((left + right) / 2);
  insert(aux[median]);
  std::cout << "in func balance\t" << aux[median].first
            << std::endl;
  balance(aux, left, median - 1);
  balance(aux, median + 1, right);
}

//----------------------------------------------------------------------------------
//COPY CTOR
//-----------------------------------------------------------------------------------

template <typename T, typename V> BinTree<T, V>::BinTree(const BinTree& bst) {
  std::cout << "calling copy ctor" << std::endl;
  if (bst.root != nullptr) {
    root.reset(new Node{bst.root->pair, nullptr, nullptr, nullptr});
    deepcopy(bst.root.get(), root);
  }
}

template <typename T, typename V>
void BinTree<T, V>::deepcopy(Node* before, std::unique_ptr<Node>& after) {
  if (before->left != nullptr) {
    after->left.reset(new Node{before->left->pair, nullptr, nullptr, after.get()});
    deepcopy(before->left.get(), after->left);
  }
  if (before->right != nullptr) {
    after->right.reset(new Node{before->right->pair, nullptr, nullptr, after.get()});
    deepcopy(before->right.get(), after->right);
  }
}

//-----------------------------------------------------------------------------
//COPY ASSIGNMENT
//------------------------------------------------------------------------------

template <typename T, typename V>
BinTree<T, V> &BinTree<T, V>::operator=(const BinTree& bst) {
  std::cout << "calling copy assignment" << std::endl;
  root.reset();
  auto tmp = bst;
  (*this) = std::move(tmp);
  return *this;
}


//----------------------------------------------------------------------------
//MAIN
//----------------------------------------------------------------------------

int main() {
  BinTree<int, int> bst{};
  BinTree<int, int> b3{};
  
  std::pair<const int, int> pair1{8, 45};
  std::pair<const int, int> pair2{3, 45};
  std::pair<const int, int> pair3{10, 45};
  std::pair<const int, int> pair4{1, 45};
  std::pair<const int, int> pair5{6, 45};
  std::pair<const int, int> pair6{3, 45};
  std::pair<const int, int> pair7{7, 45};
  std::pair<const int, int> pair8{14, 45};
  std::pair<const int, int> pair9{13, 45};
  std::pair<const int, int> pair10{4, 45};

  bst.insert(pair1);
  bst.insert(pair2);
  bst.insert(pair3);
  bst.insert(pair4);
  bst.insert(pair5);
  bst.insert(pair6);
  bst.insert(pair7);
  bst.insert(pair8);
  bst.insert(pair9);
  bst.insert(pair10);
  bst.insert({42, 42});

  std::cout << bst;

  //for (const auto &x : bst) {
  //  std::cout << "(" << x.first << ":" << x.second << ") ";
  //  std::cout << std::endl;
  // }

  //auto it = bst.begin();
  //auto stop = bst.end();

  //for (; it != stop; ++it) {
  //  const auto &x = *it;
    ////   // std::cout << x.first ... ;
  //}

  bst.find(7);
  bst.find(16);
  bst.find(9);

  // bst.clear();

  bst.balance();

  // BinTree<int,int> moved{std::move(bst)};

  //std::cout <<moved << "moved" << std::endl;

  // BinTree<int, int> copied{bst};

  //std::cout << copied << "copied" << std::endl;

  //BinTree<int, int> b3;

  // b3 = bst;

  //std::cout <<b3 <<"b3" << std::endl;

  return 0;
}


