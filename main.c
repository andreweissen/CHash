/**
 * @file main.c
 * @author Andrew Eissen <andrew@andreweissen.com>
 * @date 7 July 2021
 * @brief Source file used solely to test the various functions and associated
 * functionality of the CHash hash table data structure created by the author
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "chash.h"

/**
 * @brief Given that the author is under the impression that the hash table
 * module itself should not be printing data, this function was included in
 * <code>main</code> as a means of visualizing the table's appearance in terms
 * of its hash slots, properties, and any linked lists that occur on account of
 * key/value pairs sharing hash slots.
 *
 * @see https://stackoverflow.com/a/9053835
 * @param p_ht t_table* A pointer to the specific hash table
 * @return void
 */
static void _print_hash_table(t_table * p_ht) {

  // Declarations
  int i;
  t_property * p_entry;

  for (i = 0; i < p_ht->size; i++) {
    if (!(p_entry = p_ht->p_entries[i])) {
      continue;
    }

    printf("[%5d]: ", i);

    while (p_entry != NULL) {
      printf(
        (p_entry->p_next == NULL)
          ? "\"%s\": 0x%" PRIXPTR " "
          : "\"%s\": 0x%" PRIXPTR ", ",
        p_entry->p_key, (uintptr_t) p_entry->p_value
      );

      p_entry = p_entry->p_next;
    }

    printf("\n");
  }
}

/**
 * @brief The <code>main</code> function, a required C function, serves as the
 * driver of the program. It contains a number of test cases that measure the
 * hash table data structure's ability to handle the manipulation of data in
 * the form of key/value pairs.
 *
 * @param argc int Number of command line arguments
 * @param argv char** Actual command line arguments passed on invocation
 * @return int Default of 0
 */
int main(int argc, char ** argv) {

  // Declarations
  t_table * p_ht;
  int size, value1, value2, value3, new_value3;
  char new_value1;
  float value4;

  // Definitions
  size = 4;
  value1 = 7;
  value2 = 1370;
  value3 = 193;
  value4 = 199.22;
  new_value1 = 'a';

  printf("-----Case 1: Create hash table of size %d-----\n\n", size);
  p_ht = ch_create(size);

  printf("Put value 1 : %d\n", *(int *) ch_put(p_ht, "value 1", &value1));
  printf("Put value 2 : %d\n", *(int *) ch_put(p_ht, "value 2", &value2));
  printf("Put value 3 : %d\n", *(int *) ch_put(p_ht, "value 3", &value3));
  printf("Put value 4 : %0.2f\n", *(float *) ch_put(p_ht, "value 4", &value4));
  printf("\n");
  printf("Get value 1 : %d\n", *(int *) ch_get(p_ht, "value 1"));
  printf("Get value 2 : %d\n", *(int *) ch_get(p_ht, "value 2"));
  printf("Get value 3 : %d\n", *(int *) ch_get(p_ht, "value 3"));
  printf("Get value 4 : %0.2f\n", *(float *) ch_get(p_ht, "value 4"));

  printf("\nPrint current hash table\n");
  _print_hash_table(p_ht);

  printf("\nDelete value 2: 0x%" PRIXPTR "\n",
    (uintptr_t) ch_delete(p_ht, "value 2"));
  _print_hash_table(p_ht);

  printf("\nUpdate value 1: %d --> '%c' (0x%" PRIXPTR ")\n", value1,
    *(char *) ch_put(p_ht, "value 1", &new_value1), (uintptr_t) &new_value1);
  _print_hash_table(p_ht);

  // Deallocate all space
  ch_destroy(p_ht);

  // Resize to one
  size = 1;
  value1 = 7;
  value2 = 1370;
  value3 = 193;
  value4 = 199.22;
  new_value3 = 7711;

  printf("\n-----Case 2: Create hash table of size %d-----\n\n", size);
  p_ht = ch_create(size);

  ch_put(p_ht, "value 1", &value1);
  ch_put(p_ht, "value 2", &value2);
  ch_put(p_ht, "value 3", &value3);
  ch_put(p_ht, "value 4", &value4);

  printf("Print current hash table\n");
  _print_hash_table(p_ht);

  printf("\nUpdate value 3: %d --> %d (0x%" PRIXPTR ")\n", value3,
    *(int *) ch_put(p_ht, "value 3", &new_value3), (uintptr_t) &new_value3);
  _print_hash_table(p_ht);

  printf("\nDelete value 1: 0x%" PRIXPTR "\n",
    (uintptr_t) ch_delete(p_ht, "value 1"));
  _print_hash_table(p_ht);

  // Deallocate all space
  ch_destroy(p_ht);

  size = 10000;
  value1 = 7;
  value2 = 1370;
  value3 = 193;
  value4 = 199.22;

  printf("\n-----Case 3: Create hash table of size %d-----\n\n", size);
  p_ht = ch_create(size);

  ch_put(p_ht, "asimplekey 1", &value1);
  ch_put(p_ht, "slightlymorecomplicatedkey 2", &value2);
  ch_put(p_ht, "shortkey 3", &value3);
  ch_put(p_ht, "aparticularlylongstringtoserveaskey 4", &value4);

  printf("Print current hash table\n");
  _print_hash_table(p_ht);

  // Deallocate all space
  ch_destroy(p_ht);

  return 0;
}