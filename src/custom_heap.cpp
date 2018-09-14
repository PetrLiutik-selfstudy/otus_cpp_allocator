#include "../inc/custom_heap.h"

#include <stdlib.h>
#include <stdint.h>


///< Описатель блока памяти блока памяти кучи.
struct mcb_t {
  mcb_t* nextMcb; // Указатель на следующий блок.
  size_t size;    // Размер блока.
};

///< Минимальный размер остатка при разбиении блока.
static constexpr size_t MIN_BLOCK_SIZE = sizeof(mcb_t) * 2;

/**
 * @brief Вставка блока в цепочку свободных блоков, а так-же слияние свободных блоков.
 * @param block - блок, который необходимо добавить в цепочку.
 */
static void insertMcbIntoFreeChunk(mcb_t* block);

/**
 * @brief Инициализация кучи.
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
    mcb_t* curMcb  = beginMcb.nextMcb;
    while((curMcb->size < size) && (curMcb->nextMcb != NULL)) {
      prevMcb = curMcb;
      curMcb  = curMcb->nextMcb;
    }

    // Блок нужного размера найден.
    if(curMcb != endMcb) {
      // Указатель на выделенный блок памяти.
      ptr = static_cast<void*>((reinterpret_cast<uint8_t*>(prevMcb->nextMcb)) + sizeof(mcb_t));

      // Необходимо исключить этот блок из списка свободных.
      prevMcb->nextMcb = curMcb->nextMcb;

      // Если блок больше требуемого размера производится разбиения на два блока.
      if((curMcb->size - size) > MIN_BLOCK_SIZE)
      {
        // Создание нового блока содержащего остаток памяти от разбиения.
        mcb_t* newMcb = reinterpret_cast<mcb_t*>((reinterpret_cast<uint8_t*>(curMcb) + size));
        newMcb->size = curMcb->size - size;
        curMcb->size = size;

        // Помещение нового блока в список свободных.
        insertMcbIntoFreeChunk(newMcb);
      }

      freeBytes -= curMcb->size;
      curMcb->nextMcb = NULL;
    }
  }
  return ptr;
}

void customFree(void* ptr) {
  if(ptr != NULL) {
    // Вычисление указателя на mcb.
    uint8_t* bytePtr = reinterpret_cast<uint8_t*>(ptr);
    bytePtr -= sizeof(mcb_t);
    mcb_t* mcb = reinterpret_cast<mcb_t*>(bytePtr);

    freeBytes += mcb->size;

    // Возврат куска памяти в цепочку свободных.
    if(mcb->nextMcb == NULL)
      insertMcbIntoFreeChunk(mcb);
  }
}

size_t getFreeHeapSize() {
  return freeBytes;
}

static void customHeapInit() {
  // Инициализация начального блока списка свободных.
  beginMcb.nextMcb = reinterpret_cast<mcb_t*>(customHeap);
  beginMcb.size = 0;

  // Блок обозначающий конец кучи endMcb располагается в конце массива.
  uint8_t* heapEnd = customHeap + HEAP_SIZE - sizeof(mcb_t);
  endMcb = reinterpret_cast<mcb_t*>(heapEnd);
  endMcb->size = 0;
  endMcb->nextMcb = NULL;

  // Первый свободный блок, содержащий всю память выделенную под кучу минус endMcb.
  mcb_t* fisrtFreeMcb = reinterpret_cast<mcb_t*>(customHeap);
  fisrtFreeMcb->size = HEAP_SIZE - sizeof(mcb_t);
  fisrtFreeMcb->nextMcb = endMcb;

  // Куча уже содержит endMcb.
  freeBytes -= sizeof(mcb_t);
}

static void insertMcbIntoFreeChunk(mcb_t* mcb) {
  // Итерирование по цепочке блоков до нахождения блока с большим адресом.
  mcb_t* it;
  for(it = &beginMcb; it->nextMcb < mcb; it = it->nextMcb)
    ;

  // Если конец предыдущего блока совпадает с началом текущего, блоки объединяются.
  uint8_t* bytePtr = reinterpret_cast<uint8_t*>(it);
  if((bytePtr + it->size) == reinterpret_cast<uint8_t*>(mcb)) {
    it->size += mcb->size;
    mcb = it;
  }

  // Если конец текущего блока совпадает с началом следующего, блоки объединяются.
  bytePtr = reinterpret_cast<uint8_t*>(mcb);
  if((bytePtr + mcb->size) == reinterpret_cast<uint8_t*>(it->nextMcb)) {
    if(it->nextMcb != endMcb)
    {
      mcb->size += it->nextMcb->size;
      mcb->nextMcb = it->nextMcb->nextMcb;
    }
    else
      mcb->nextMcb = endMcb;
  }
  else
    mcb->nextMcb = it->nextMcb;

  // Если не было слияния предыдущего и текущего блоков.
  if(it != mcb)
    it->nextMcb = mcb;
}
