#include "custom_heap.h"

#include <stdlib.h>
#include <stdint.h>



// Описатель блока памяти в цепочке свободных блоков кучи.
struct mcb_t {
  mcb_t* nextFreeMcb; // Указатель на следующий свободный блок.
  size_t size;				// Размер свободного блока.
};

///< Минимальный размер остатка при разбиении блока.
#define MIN_BLOCK_SIZE	((size_t)(sizeof(mcb_t) * 2))

/**
 * @brief Вставка блока в цепочку свободных блоков, а так-же слияние свободных блоков.
 * @param block - блок, который необходимо добавить в цепочку.
 */
static void insertMcbIntoFreeChunk(mcb_t* block);

/**
 * @brief Инициализация кучи
 */
static void customHeapInit();


///< Память для кучи.
static uint8_t customHeap[HEAP_SIZE];

///< Начальный блок в цепочке свободных.
static mcb_t beginMcb;

///< Указатель на конечный блок в цепочке свободных.
static mcb_t* endMcb = NULL;

///< Размер свободного места в куче.
static size_t freeBytes = HEAP_SIZE;

void* customMalloc(size_t size) {
  void* ptr = NULL;

  // Инициализация кучи.
  if(endMcb == NULL)
    customHeapInit();

  if(size > 0)
    size += sizeof(mcb_t);

  if((size > 0) && (size <= freeBytes))
  {
    // Итерирование по списку свободных блоков, до нахождения первого с большим либо равным размером.
    mcb_t* prevMcb = &beginMcb;
    mcb_t* curMcb  = beginMcb.nextFreeMcb;
    while((curMcb->size < size) && (curMcb->nextFreeMcb != NULL)) {
      prevMcb = curMcb;
      curMcb  = curMcb->nextFreeMcb;
    }

    // Блок нужного размера найден.
    if(curMcb != endMcb) {
      // Указатель на выделенный блок памяти.
      ptr = (void*)(((uint8_t*)prevMcb->nextFreeMcb) + sizeof(mcb_t));

      // Необходимо исключить этот блок из списка свободных.
      prevMcb->nextFreeMcb = curMcb->nextFreeMcb;

      // Если блок больше требуемого размера производится разбиения на два блока.
      if((curMcb->size - size) > MIN_BLOCK_SIZE)
      {
        // Создание нового блока содержащего остаток памяти от разбиения.
        mcb_t* newMcb = (mcb_t*)(((uint8_t*)curMcb) + size);
        newMcb->size = curMcb->size - size;
        curMcb->size = size;

        // Помещение нового блока в список свободных.
        insertMcbIntoFreeChunk(newMcb);
      }

      freeBytes -= curMcb->size;
      curMcb->nextFreeMcb = NULL;
    }
  }
  return ptr;
}

void customFree(void* ptr) {
  if(ptr != NULL) {
    // Вычисление указателя на mcb.
    uint8_t* bytePtr = (uint8_t*)ptr;
    bytePtr -= sizeof(mcb_t);
    mcb_t* mcb = (mcb_t*)bytePtr;

    // Возврат куска памяти в цепочку свободных.
    if(mcb->nextFreeMcb == NULL)
      insertMcbIntoFreeChunk(mcb);
  }
}

size_t getFreeHeapSize() {
  return freeBytes;
}

static void customHeapInit() {
  // Инициализация начального блока списка свободных.
  beginMcb.nextFreeMcb = (mcb_t*)customHeap;
  beginMcb.size = 0;

  // Блок обозначающий конец кучи endMcb располагается в конце массива.
  uint8_t* heapEnd = customHeap + HEAP_SIZE - sizeof(mcb_t);
  endMcb = (mcb_t*)heapEnd;
  endMcb->size = 0;
  endMcb->nextFreeMcb = NULL;

  // Первый свободный блок, содержащий всю память выделенную под кучу минус endMcb.
  mcb_t* fisrtFreeMcb = (mcb_t*)customHeap;
  fisrtFreeMcb->size = HEAP_SIZE - sizeof(mcb_t);
  fisrtFreeMcb->nextFreeMcb = endMcb;

  // Куча уже содержит endMcb.
  freeBytes -= sizeof(mcb_t);
}

static void insertMcbIntoFreeChunk(mcb_t* mcb) {
  // Итерирование по цепочке блоков до нахождения блока с большим адресом.
  mcb_t* it;
  for(it = &beginMcb; it->nextFreeMcb < mcb; it = it->nextFreeMcb)
    ;

  // Если конец предыдущего блока совпадает с началом текущего, блоки объединяются.
  uint8_t* bytePtr;
  bytePtr = (uint8_t*)it;
  if((bytePtr + it->size) == (uint8_t*)mcb) {
    it->size += mcb->size;
    mcb = it;
  }

  // Если конец текущего блока совпадает с началом следующего, блоки объединяются.
  bytePtr = (uint8_t*)mcb;
  if((bytePtr + mcb->size) == (uint8_t*)it->nextFreeMcb) {
    if(it->nextFreeMcb != endMcb)
    {
      mcb->size += it->nextFreeMcb->size;
      mcb->nextFreeMcb = it->nextFreeMcb->nextFreeMcb;
    }
    else
      mcb->nextFreeMcb = endMcb;
  }
  else
    mcb->nextFreeMcb = it->nextFreeMcb;

  // Если не было слияния предыдущего и текущего блоков.
  if(it != mcb)
    it->nextFreeMcb = mcb;
}
