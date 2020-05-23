#include <list>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

template<class T>
class UF_Set;

// 링크드리스트 아이템 노드 클래스
template<class T>
class UF_Item {
public:
  typedef UF_Item<T>* pointer_type;
  typedef UF_Set<T>* parent_type;

  T value;
  int no;
  char* name;
  pointer_type next;
  parent_type parent;
};

// 아이템들의 Head와 Tail정보를 가지는 Set 클래스
template<class T>
class UF_Set {
public:
  typedef UF_Item<T> item_type;
  typedef UF_Set<T> this_type;
  typedef UF_Set<T>* pointer_type;

  // 새로운 Set 만들기
  static pointer_type MakeSet()
  {
    this_type* tt = new this_type;
    tt->cardinarity = 1;
    tt->weight = 0;
    return tt;
  }

  // 새로운 Set을 만들고 Item 노드 만들기
  static item_type* MakeItem(int no, T value, char* name)
  {
    item_type* item = new item_type;
    item->next = nullptr;
    item->no = no;
    item->value = value;
    item->name = name;
    item->parent = MakeSet();
    item->parent->head = item->parent->tail = item;
    return item;
  }

  // Set 크기
  int cardinarity;
  // Set Weight
  long weight;
  item_type* head;
  item_type* tail;

  // Set1과 Set2를 합칩니다.
  static void Union(pointer_type uf1, pointer_type uf2, long weight) {
    if (uf1 == uf2) return;

    if (uf1->cardinarity < uf2->cardinarity ||
      (uf1->cardinarity == uf2->cardinarity
        && uf1->head->no > uf2->head->no)) {
      pointer_type tmp = uf1;
      uf1 = uf2;
      uf2 = tmp;
    }

    // uf2를 uf1으로 합치기
    uf1->tail->next = uf2->head;
    uf1->tail = uf2->tail;
    uf1->cardinarity += uf2->cardinarity;
    item_type* iter = uf2->head;
    while (iter)
    {
      iter->parent = uf1;
      iter = iter->next;
    }

    uf1->weight += uf2->weight + weight;

    delete uf2;
  }
};

// UnionFind 클래스
template<class T, int L>
class UF {
public:
  UF_Item<T> *items[L];

  // 아이템 초기화
  UF_Item<T>* Push(int no, int count, char *name){
    return items[no] = UF_Set<T>::MakeItem(no, count, name);
  }

  // Union
  void Union(int a, int b, long w) {
    UF_Set<T>::Union(items[a]->parent, items[b]->parent, w);
  }

  // Find
  int Find(int a) {
    return items[a]->parent->head->no;
  }
};

// 크루스칼 알고리즘을 실행하기 위한 노드 클래스입니다.
class KruskalNode {
public:
  typedef UF_Item<int>* item_type;
  
  int weight;
  item_type i1;
  item_type i2;
};

// 크루스칼 알고리즘의 아이템을 선택할 수 있는 클래스입니다.
class KruskalSelector {
public:

  typedef UF_Item<int>* item_type;
  typedef KruskalNode node_type;

  std::vector<node_type*> wait;

  // 아이템 초기화
  void Push(item_type i1, item_type i2, long w) {
    node_type* node = new node_type;
    node->i1 = i1;
    node->i2 = i2;
    node->weight = w;
    wait.push_back(node);
  }

  // 아이템을 역순으로 정렬
  void Sort() {
    sort(wait.rbegin(), wait.rend(), [](node_type* i1, node_type* i2) {
      if (i1->weight != i2->weight)
        return i1->weight < i2->weight;
      if ((i1->i1->value + i1->i2->value) != (i2->i1->value + i2->i2->value))
        return (i1->i1->value + i1->i2->value) >= (i2->i1->value + i2->i2->value);
      int mn1 = i1->i1->no < i1->i2->no ? i1->i1->no : i1->i2->no;
      int mn2 = i2->i1->no < i2->i2->no ? i2->i1->no : i2->i2->no;
      return mn1 < mn2;
      });
  }

  // 크루스칼 아이템을 하나씩 가져오기
  node_type* Next() {
    node_type* last = wait[wait.size()-1];
    wait.pop_back();
    return last;
  }
};

// string 클래스를 사용하지 못하므로 직접 문자열 처리

int strlen(char* src) {
	const char *tar = src;
	while (*src)
		src++;
	return src - tar;
}

char* strdup(char* src) {
  int len = strlen(src);
  char* buf = new char[len + 1];
  char* ptr = buf;
  while (len--)
    *ptr++ = *src++;
  *ptr = 0;
  return buf;
}

static UF<int, 1000010> uf;
static KruskalSelector ks;

int main()
{
  int N, M, q; cin >> N >> M >> q;
  int n = N;
  int a = 0;

  char buf[50];

  // 도시 입력
  while (N--) {
    int no, value;
    // 도시이름에 공백이 포함되어있지 않다고 가정
    cin >> no >> buf >> value;
    char* name = strdup(buf);
    uf.Push(no, value, name);
    a = no;
  }

  // 거리 입력
  while (M--) {
    int i1, i2;
    long w;
    cin >> i1 >> i2 >> w;
    ks.Push(uf.items[i1], uf.items[i2], w);
  }

  ks.Sort();

  while (q--) {
  
    char ch; cin >> ch;

    switch (ch) {
    case 'N':
      {
        int v, k;
        cin >> v >> k;
        if (uf.items[v] == nullptr || uf.items[v]->parent->cardinarity <= k)
        {
          cout << "no exist\n";
        }
        else
        {
          UF_Item<int>* iter = uf.items[v]->parent->head;
          while (--k >= 0)
            iter = iter->next;
          cout << iter->no << ' ' << iter->name << '\n';
        }
      }
      break;
    case 'L':
      {
        int v; cin >> v;
        cout << uf.items[v]->parent->cardinarity << '\n';
      }
      break;
    case 'I':
      {
        KruskalSelector::node_type* next = ks.Next();
        if (uf.Find(next->i1->no) != uf.Find(next->i2->no))
        {
          uf.Union(next->i1->no, next->i2->no, next->weight);
          if (next->i1->parent->cardinarity != n)
          {
            // ID S
            cout << next->i1->parent->head->no << ' ' << next->i1->parent->cardinarity << '\n';
          }
          else
          {
            // ID S v D
            cout << next->i1->parent->head->no << ' ' << next->i1->parent->cardinarity << ' ' 
              << next->i1->parent->head->no << ' ' << next->i1->parent->weight << '\n';
            return 0;
          }
        }
        else
          cout << "not union\n";
      }
      break;
    case 'F':
      {
        int v1, v2;
        cin >> v1 >> v2;
        if (uf.Find(v1) == uf.Find(v2)) 
        {
          cout << "True " << uf.items[v1]->parent->head->no << '\n';
        }
        else
        {
          cout << "False " << uf.items[v1]->parent->head->no << ' ' << uf.items[v2]->parent->head->no << '\n';
        }
      }
      break;
    case 'W':
      {
        int v;
        cin >> v;
        cout << uf.items[v]->parent->weight << '\n';
      }
      break;
    case 'Q':
      {
        while (ks.wait.size() != 0) {
          KruskalSelector::node_type* next = ks.Next();
          if (uf.Find(next->i1->no) != uf.Find(next->i2->no))
            uf.Union(next->i1->no, next->i2->no, next->weight);
        }
        cout << uf.items[a]->parent->head->no << ' ' << uf.items[a]->parent->weight << '\n';
        return 0;
      }
      break;
    }
  }
}
