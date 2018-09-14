#ifdef _MSC_VER
#define _ITERATOR_DEBUG_LEVEL (0)
#define _HAS_ITERATOR_DEBUGGING (0)
#endif

#include <iostream>
#include <map>
#include <vector>

#include "../inc/custom_allocator.h"
#include "../inc/custom_vector.h"
#include "../inc/factorial.h"

int main()
{
  using cust_vec_alloc_t = CustomAllocator<uint32_t, 100>;
  using cust_vec_t = CustomVector<uint32_t, cust_vec_alloc_t>;

  cust_vec_t vec1{1, 2, 3};
  cust_vec_t vec2{5, 6, 7};
  vec1.clear();


  for(uint32_t i = 0; i < 10; ++i)
    vec1.push_back(factorial(i));

    vec1.pop_back();

  vec1.swap(vec2);
  cust_vec_t vec3(std::forward<cust_vec_t>(vec1));

  for(auto it: vec1)
    std::cout << it << std::endl;

  for(auto it: vec2)
    std::cout << it << std::endl;

  for(auto it: vec3)
    std::cout << it << std::endl;

//  using cust_map_alloc_t = CustomAllocator<std::pair<const int, int>, 21>;
//  using cust_map_t = std::map<int, int, std::less<int>, cust_map_alloc_t>;

//  cust_map_t map1;

//  for(int i = 0; i < 20; ++i)
//    map1[i] = i;

//  for(auto it: map1)
//    std::cout << it.first << " " << it.second << std::endl;

//  cust_map_t map2 = map1;

//  for(auto it: map2)
//    std::cout << it.first << " " << it.second << std::endl;

  return 0;
}
