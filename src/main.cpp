#include <iostream>
#include <map>

#include "../inc/custom_allocator.h"
#include "../inc/custom_vector.h"
#include "../inc/factorial.h"
#include "../inc/ver.h"

constexpr int MAX_ELEMS = 10;

int main()
{
  // Стандартный мап со стандартным аллокатором
  using map_t = std::map<int, int>;
  map_t map1;

  // Стандартный мап с кастомным аллокатором (MSVC использует 1 дополнительный элемент в мапе).
  using map_cust_alloc_t = custom::allocator<std::pair<const int, int>, MAX_ELEMS + 1>;
  using map_cust_t = std::map<int, int, std::less<int>, map_cust_alloc_t>;
  map_cust_t map2;

  // Кастомный вектор со стандартным аллокатором.
  using cust_vec_t = custom::vector<int>;
  cust_vec_t vec1;

  // Кастомный вектор с кастомным аллокатором.
  using cust_vec_cust_alloc_t = custom::allocator<int, MAX_ELEMS>;
  using cust_vec_cust_t = custom::vector<int, cust_vec_cust_alloc_t>;
  cust_vec_cust_t vec2;
  vec2.reserve(MAX_ELEMS);

  // Кастомный вектор с кастомным аллокатором с кастомной кучей.
  using cust_vec_cust1_alloc_t = custom::allocator<int, 0>;
  using cust_vec_cust1_t = custom::vector<int, cust_vec_cust1_alloc_t>;
  cust_vec_cust1_t vec3;


  for(int i = 0; i < MAX_ELEMS; ++i) {
    int value = static_cast<int>(factorial(static_cast<uint32_t>(i)));
    map1[i] = value;
    map2[i] = value;
    vec1.push_back(value);
    vec2.push_back(value);
    vec3.push_back(value);
  }

  std::cout << "Alloc version: "
            << ver_major() << "."
            << ver_minor() << "."
            << ver_patch() << "."
            << std::endl;

  std::cout << "STL map with STL allocator" << std::endl;
  for(const auto& it: map1)
    std::cout << it.first << " " << it.second << std::endl;

  std::cout << "STL map with custom allocator" << std::endl;
  for(const auto& it: map2)
    std::cout << it.first << " " << it.second << std::endl;

  std::cout << "Custom vector with STL allocator" << std::endl;
  for(const auto& it: vec1)
    std::cout << it << std::endl;

  std::cout << "Custom vector with custom allocator" << std::endl;
  for(const auto& it: vec2)
    std::cout << it << std::endl;

  cust_vec_cust1_t vec4{1, 2, 3};
  std::swap(vec3, vec4);

  cust_vec_cust1_t vec5(std::forward<cust_vec_cust1_t>(vec3));


  return 0;
}
