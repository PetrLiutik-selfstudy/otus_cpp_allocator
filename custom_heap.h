#pragma once

///< Размер кучи, байт.
static constexpr size_t HEAP_SIZE = 65536;

/**
 * @brief customMalloc - функция выделения памяти в куче.
 * @param size - размер выделяемого блока памяти.
 * @return указатель на выделенный блок памяти.
 */
void* customMalloc(size_t size);

/**
 * @brief customFree - функция освобождения памяти в куче.
 * @param ptr - указатель на удаляемый блок памяти.
 * @return
 */
void customFree(void* ptr);

/**
 * @brief getFreeHeapSize - выдать размер свободной памяти в куче.
 * @return
 */
size_t getFreeHeapSize();
