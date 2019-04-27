#include <chrono>
#include <iostream>

class Timer {
 private:
  // per using sutrumpiname ilgus chrono bibliotekos tipus
  using hrClock = std::chrono::high_resolution_clock;
  using durationDouble = std::chrono::duration<double>;
  std::chrono::time_point<hrClock> start;

 public:
  Timer() : start{hrClock::now()} {}
  void reset() { start = hrClock::now(); }
  double elapsed() const {
    return durationDouble(hrClock::now() - start).count();
  }
};

class Vector {
 private:
  int sz;
  double* elem;

 public:
  // default konstruktorius
  Vector() : sz(0), elem(new double[sz]) {
    std::cout << "default c-tor" << std::endl;
  }
  // konstruktorius su 1 parametru
  Vector(int s) : sz{s}, elem{new double[sz]} {
    std::cout << "c-tor su 1 par." << std::endl;
  }
  Vector(int s, double val) : sz(s), elem(new double[sz]) {
    std::fill_n(elem, s, val);
    std::cout << "c-tor su 2 par." << std::endl;
  }
  // std::initializer list konstruktorius
  Vector(std::initializer_list<double> il)
      : sz{static_cast<int>(il.size())},  // kam reikalingas static_cast?
        elem{new double[il.size()]} {
    std::copy(il.begin(), il.end(), elem);  // nukopijuoti iš il į elem
  }
  // copy konstruktorius
  Vector(const Vector& v)
      : sz{v.sz},               // inicializuojame sz
        elem{new double[v.sz]}  // išskiriame atmintį elem
  {
    for (int i = 0; i != sz; ++i)  // nukopijuojame elementus
      elem[i] = v.elem[i];
    std::cout << "copy c-tor" << std::endl;
  }

  // move konstruktorius
  Vector(Vector&& v)
      : sz{v.sz},
        elem{v.elem}  // "pavok elementus" iš v
  {
    v.elem = nullptr;  // v neturi jokių elementų
    v.sz = 0;
    std::cout << "move c-tor" << std::endl;
  }

  // Priskyrimo operatorius (Copy semantika)
  Vector& operator=(const Vector& v) {
    std::cout << "copy operator=" << std::endl;
    // Savęs priskyrimo aptikimas
    if (&v == this)
      return *this;
    double* p = new double[v.sz];
    for (int i = 0; i != v.sz; ++i)  // nukopijuojame v elementus
      p[i] = v.elem[i];
    delete[] elem;  // atlaisviname seną atmintį!
    elem = p;       // elem point'ina į naują atmintį
    sz = v.sz;      // atnaujiname size
    return *this;   // grąžiname objektą
  }

  // Priskyrimo operatorius (Move semantika)
  Vector& operator=(Vector&& v) {
    std::cout << "move operator=" << std::endl;
    // Savęs priskyrimo aptikimas
    if (&v == this)
      return *this;
    delete[] elem;     // atlaisviname seną atmintį!
    elem = v.elem;     // elem point'ina į v.elems atmintį
    sz = v.sz;         // atnaujiname size
    v.elem = nullptr;  // v neturi jokių elementų
    v.sz = 0;
    return *this;  // grąžiname objektą
  }

  ~Vector() { delete[] elem; }
  int size() const { return sz; }
  double& operator[](int i) { return elem[i]; }
  const double& operator[](int i) const { return elem[i]; }
};

Vector operator+(const Vector& a, const Vector& b) {
  if (a.size() != b.size())
    throw std::runtime_error("Vektorių dydžio neatitikimas!");
  auto size = a.size();
  Vector c(size);
  for (auto i = 0; i != a.size(); ++i) c[i] = a[i] + b[i];
  return c;
}

template<class T>
void swap(T& x, T& y) {
  T temp { std::move(x) }; // iškviečia move-ctor
  x = std::move(y);        // iškviečia move-assignment
  y = std::move(temp);     // iškviečia move-assignment
}

int main() {
  int size = 10000000;
  Vector a(size,1);
  Vector b(size,2);
  Timer t;  // Paleisti
  Vector c;
  c = a + b;
  std::cout << size << " elementų sudėti užtruko: " << t.elapsed() << " s\n";
  std::cout << "c[100000] = " << c[100000] << std::endl; // Patikriname ar tikrai sudėjo
  return 0;
}