#pragma once

///< Размер кучи, байт.
static constexpr size_t HEAP_SIZE = 65536;

/**
 * @brief Выделение памяти в куче.
 * @param size - размер выделяемого блока памяти.
 * @return указатель на выделенный блок памяти.
 */
void* customMalloc(size_t size);

/**
 * @brief Освобождение памяти в куче.
 * @param ptr - указатель на удаляемый блок памяти.
 */
void customFree(void* ptr);

/**
 * @brief Выдать размер свободной памяти в куче, без учета фрагментации.
 * @return размер свободной памяти в куче.
 */
size_t getFreeHeapSize();
