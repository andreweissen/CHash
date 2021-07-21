/**
 * @file chash.c
 * @author Andrew Eissen <andrew@andreweissen.com>
 * @date 7 July 2021
 * @brief Source file for CHash, a single-threaded hash table implementation
 */

#include <stdlib.h>
#include <string.h>
#include "chash.h"

/**
 * @brief Arguably the module's most important function, the <code>_hash</code>
 * function is used to hash a given string value passed as a formal parameter
 * to a unique <code>long int</code>. This particular implementation is a
 * variation of cryptographer Daniel J. Bernstein's famous string hashing
 * function "djb2." Though it makes use of the magic number 33, it does not use
 * the prime magic number 5381, instead starting from 0 and incrementing based
 * on the length of the passed string and the ASCII value of the current
 * <code>char</code> in the string.
 * <br />
 * <br />
 * While perhaps not the most efficient of hashing functions, <code>_hash</code>
 * ensures that for tables for which there exists an equal number of hash slots
 * and key/value pairs (i.e. four properties and four slots), each key/value
 * pair will have its own slot, thus ensuring the linked list fallback built in
 * to ensure multiple properties can exist at the same slot is unneeded in such
 * cases.
 *
 * @see http://www.cse.yorku.ca/~oz/hash.html
 * @param p_key const char* The string to be hashed
 * @return value unsigned long int The resultant hash <code>int</code> value
 */
static unsigned long int _hash(const char * p_key) {

  // Declarations
  unsigned int counter;
  unsigned short length;
  unsigned long int value;

  // Definitions
  length = strlen(p_key);
  value = 0L;
  counter = 0;

  // Update value based on previous value, string length, and ASCII value
  while (counter < length) {
    value = value * 33 + p_key[counter++];
  }

  return value;
}

/**
 * @brief The <code>_clear</code> helper function is a private function that is
 * used to deallocate all space previously reserved in heap memory for a
 * <code>t_property</code> <code>struct</code> and its associated string data
 * member <code>p_key</code>. This function is used by the public functions
 * <code>ch_clear</code> and <code>ch_destroy</code> to clean out an extant hash
 * table as needed.
 *
 * @param p_entry t_property* The specific <code>t_property</code> to be cleared
 * @return void
 */
static void _clear(t_property * p_entry) {

  // Free key space if extant
  if (p_entry->p_key != NULL) {
    free(p_entry->p_key);
    p_entry->p_key = NULL;
  }

  // Free entry itself if extant
  if (p_entry != NULL) {
    free(p_entry);
    p_entry = NULL;
  }
}

/**
 * @brief The <code>_construct</code> helper function is a private function that
 * is used to build a <code>t_property</code> <code>struct</code> from the key
 * and value passed as formal parameters. It allocates space in memory for the
 * new object and the string key, sets the various members, and returns the
 * <code>struct</code> for inclusion in the hash table. It is invoked primarily
 * by <code>ch_put</code> to assign new properties.
 *
 * @param p_key const char* A string representing the key of the key/value pair
 * @param p_value void* A void pointer to the address of the associated value
 * @return t_property*
 */
static t_property * _construct(const char * p_key, void * p_value) {

  // Declaration
  t_property * p_entry;

  // Allocation definitions
  p_entry = malloc(sizeof(t_property));
  p_entry->p_key = malloc(strlen(p_key) + 1);

  // Ensure space was successfully allocated for both
  if (!p_entry || !p_entry->p_key) {
    _clear(p_entry);
    return NULL;
  }

  // Copy string to data member from formal parameter
  strcpy(p_entry->p_key, p_key);

  // Set value as formal parameter void pointer
  p_entry->p_value = p_value;

  // No next by default
  p_entry->p_next = NULL;

  return p_entry;
}

/**
 * @brief The <code>ch_put</code> function maps the specified prop key denoted
 * by the formal parameter <code>p_key</code> to the void pointer passed to the
 * function as the associated value, namely, <code>p_value</code>. If an extant
 * property is found at the specified key, the function assumes the intent is to
 * update the value at that key, and <code>p_value</code> is mapped to the key.
 * If two key/value pairs are mapped to the same hash slot, the function creates
 * a linked list at that hash slot rather than lose the new pair or rehash and
 * resize the entire structure.
 * <br />
 * <br />
 * The author acknowledges that a rehash could be done here if multiple pairs
 * are hashed to the same slot. However, if a table is too small, the same hash
 * slot could be regenerated again, so a linked list is the safest approach. In
 * large table sizes (or sizes as large as the total number of entries), a hash
 * of two pairs to the same slot is unlikely, so the linked list is simply a
 * fallback measure to gracefully handle edge cases without having to resize the
 * entire structure accordingly.
 *
 * @param p_table t_table* A pointer to the specific hash table
 * @param p_key const char* A string representing the key of the key/value pair
 * @param p_value void* A void pointer to the address of the associated value
 * @return p_value void* A void pointer representing the value of the pair
 */
void * ch_put(t_table * p_table, const char * p_key, void * p_value) {

  // Declarations
  unsigned int hash;
  t_property * p_entry, * p_previous;

  // Ensure hash lies between 0 and table's max size
  hash = _hash(p_key) % p_table->size;

  // Get first prospective key/value pair at this slot
  p_entry = p_table->p_entries[hash];

  // If there's nothing at this slot, add a new property struct here
  if (p_entry == NULL) {
    p_table->p_entries[hash] = _construct(p_key, p_value);
    return p_value;
  }

  // If there already are entries at this slot...
  while (p_entry != NULL) {

    // Update value of match found in linked list
    if (strcmp(p_entry->p_key, p_key) == 0) {
      p_entry->p_value = p_value;
      return p_value;
    }

    p_previous = p_entry;
    p_entry = p_previous->p_next;
  }

  // Add new property at tail of linked list
  p_previous->p_next = _construct(p_key, p_value);
  return p_value;
}

/**
 * @brief The <code>ch_get</code> function is used to retrieve the void pointer
 * constituting the value of the <code>t_property</code> type that is associated
 * with the formal parameter <code>p_key</code>. If multiple pairs exist at the
 * same hash slot, the function will iterate along the list until a match is
 * found or the end of the list encountered. The function does not remove the
 * key/value pair; that is the purpose of the <code>delete</code> function.
 *
 * @param p_table t_table* A pointer to the specific hash table
 * @param p_key const char* A string representing the key of the desired value
 * @return void* A void pointer representing the value of the key/value pair
 */
void * ch_get(t_table * p_table, const char * p_key) {

  // Declarations
  unsigned int hash;
  t_property * p_entry;

  // Ensure hash lies between 0 and table's size
  hash = _hash(p_key) % p_table->size;

  // Get prospective key/value pair
  p_entry = p_table->p_entries[hash];

  if (p_entry == NULL) {
    return NULL;
  }

  // Iterate through potential linked list found at hash slot
  while (p_entry != NULL) {

    // Return match found in linked list
    if (strcmp(p_entry->p_key, p_key) == 0) {
      return p_entry->p_value;
    }

    p_entry = p_entry->p_next;
  }

  return NULL;
}

/**
 * @brief The <code>ch_delete</code> function is used to remove the property
 * specified via the parameter constituting the key, namely <code>p_key</code>.
 * If multiple pairs exist at the same hash slot, the function will iterate
 * along the list until a match is found or the end of the list encountered.
 * Though the function returns the void pointer constituting the value, it
 * should not be used as the retrieval function; that is the purpose of the
 * <code>ch_get</code> function.
 *
 * @param p_table t_table* A pointer to the specific hash table
 * @param p_key const char* A string representing the key of the desired value
 * @return void* A void pointer representing the value of the key/value pair
 */
void * ch_delete(t_table * p_table, const char * p_key) {

  // Declarations
  unsigned int hash;
  t_property * p_current, * p_previous;
  void * p_value_storage;

  // Ensure hash lies between 0 and table's max size
  hash = _hash(p_key) % p_table->size;

  // Get first prospective key/value pair at slot
  p_current = p_table->p_entries[hash];
  p_previous = NULL;

  // Iterate through potential linked list comparing key strings
  while (p_current != NULL && strcmp(p_current->p_key, p_key) != 0) {
    p_previous = p_current;
    p_current = p_current->p_next;
  }

  // No such key/value pair found matching target
  if (p_current == NULL) {
    return NULL;
  }

  // Store value void pointer for return from function
  p_value_storage = p_current->p_value;

  /*
   * If there is no previous entry prior to target entry, redefine "head node"
   * sitting at this hash slot. Otherwise, redefine the previous node's p_next
   * data member. If the current node has no next property, implying it sits at
   * the end of the linked list, apply NULL as the redefined value. Otherwise,
   * set the next node as the redefined value.
   */
  *((!p_previous) ? &p_table->p_entries[hash] : &p_previous->p_next) =
    (!p_current->p_next) ? NULL : p_current->p_next;

  // Deallocate space reserved for this property
  _clear(p_current);

  // Return cached value void pointer
  return p_value_storage;
}

/**
 * @brief The <code>ch_clear</code> function is used as the means by which the
 * given hash table specified via the formal parameter <code>p_table</code>
 * is cleared of entries stored in heap memory. The function iterates through
 * the table and any linked lists and deallocates space reserved in heap for
 * each <code>t_property</code> and <code>p_key</code>. The private helper
 * function <code>_clear</code> is employed for this purpose.
 *
 * @param p_table t_table* A pointer to the specific hash table
 * @return void
 */
void ch_clear(t_table * p_table) {

  // Declarations
  t_property * p_entry;
  int counter;

  // Define counter int
  counter = 0;

  // Iterate through table entries, deallocating all space associated with each
  while (counter < p_table->size) {
    p_entry = p_table->p_entries[counter++];

    if (p_entry != NULL) {
      _clear(p_entry);
    }
  }
}

/**
 * @brief The <code>ch_create</code> function is used to construct a new hash
 * table. It accepts as a sole formal parameter a <code>long int</code>
 * representing the desired number of table hash slots and the associated size
 * of the table's <code>p_entries</code> data member, an array of
 * <code>t_property<code> <code>struct<code>s. The function allocates space in
 * heap memory for the table and the array.
 *
 * @param table_size unsigned longint Desired number of table slots
 * @return t_table* A pointer to the specific hash table
 */
t_table * ch_create(unsigned long int table_size) {

  // Declarations
  t_table * p_table;
  int counter;

  // Definitions
  p_table = NULL;
  counter = 0;

  // Allocate space for table and its array of key/value properties
  p_table = malloc(sizeof(t_table));
  p_table->p_entries = malloc(sizeof(t_property *) * table_size);

  // Set size of table for properties/hash slots
  p_table->size = table_size;

  // Ensure space was successfully allocated for both
  if (p_table == NULL || p_table->p_entries == NULL) {
    ch_destroy(p_table);
    return NULL;
  }

  // Set default value of NULL for all hash slots
  while (counter < table_size) {
    p_table->p_entries[counter++] = NULL;
  }

  return p_table;
}

/**
 * @brief The <code>ch_destroy</code> function is used simply to deallocate all
 * space reserved for the hash table specified via the formal parameter
 * <code>p_table</code> and its associated <code>p_entries</code> data member
 * array in heap memory. If the array has extant members, it will likewise make
 * use of <code>clear</code> to deallocate all space reserved for those
 * properties and their <code>p_key</code> data members.
 *
 * @param p_table t_table* A pointer to the specific hash table
 * @return void
 */
void ch_destroy(t_table * p_table) {

  // Run through table entries array if populated
  if ((sizeof(p_table->p_entries) / sizeof(t_property)) > 0) {
    ch_clear(p_table);
  }

  // Free table entries if extant
  if (p_table->p_entries != NULL) {
    free(p_table->p_entries);
    p_table->p_entries = NULL;
  }

  // Free table if extant
  if (p_table != NULL) {
    free(p_table);
    p_table = NULL;
  }
}