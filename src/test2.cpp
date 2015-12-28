#include <list>
#include <iostream>

using std::cout;
using std::endl;

class Element
{
  public:
    Element(int i): _i(i) { cout << "Constructor Element " << i << endl; };
    ~Element() { cout << "Destructor Element " << _i << endl; };
    void print() { cout << "Print Element " << _i << endl; };
    int _i;
};


class Referrer
{
  public:
    Referrer (Element & element) : _element(element) {};
    Element _element;
    void update(Element & element) { _element = element; };
    void print() { cout << "Referrer --- "; _element.print(); };
};

std::list<Element> l;

bool sortfunc(Element &e1, Element &e2) { return(e1._i < e2._i); };

int main(int, char *[]) {
  cout << "Emplacing" << endl;
/*  l.emplace_back(1);
  l.emplace_back(2);
  l.emplace_back(5);
  l.emplace_back(4);
  l.emplace_back(0); */
  l.sort(sortfunc);
  cout << "Creating referrer" << endl;
  Element e = Element(99);
  Element e2 = Element(999);
  Referrer r = Referrer(e);
  e.print();
  r.print();
  r.update(e2);
  r.print();
  cout << "Finished" << endl;
};
