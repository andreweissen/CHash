/**
 * @file chash.h
 * @author Andrew Eissen <andrew@andreweissen.com>
 * @date 7 July 2021
 * @brief Header file for CHash, a single-threaded hash table implementation
 */

#ifndef __CHASH_H_
#define __CHASH_H_

/**
 * @brief The <code>s_property</code> <code>struct</code> contains three data
 * members for each hash table property. These are <code>p_key</code>, a string
 * that serves as the key for the key/value pair constituting the property;
 * <code>p_value</code>, a void pointer to the address of the associated value;
 * and <code>p_next</code>, a pointer to the next node in the linked list that
 * forms if a hash slot has more than one key/value pair associated with itself.
 */
typedef struct s_property {
  char * p_key;                 /**< String representing the key of the pair */
  void * p_value;               /**< Void pointer representing the value */
  struct s_property * p_next;   /**< Next node if linked list exists at slot */
} t_property;

/**
 * @brief The <code>t_table</code> <code>struct</code> has a pair of data
 * members, namely <code>size</code>, which denotes the desired number of hash
 * slots in the table, and <code>p_entries</code>, a double pointer/array of
 * <code>t_property</code>s constituting the properties of the hash table. The
 * author has seen macros used in place of the <code>size</code> member, but
 * elected to use a run-time value rather than a compile-time value.
 */
typedef struct {
  unsigned long int size;       /**< Total number of hash slots/properties */
  t_property ** p_entries;      /**< Array of properties existing in table */
} t_table;

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
void * ch_put(t_table * p_table, const char * p_key, void * p_value);

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
void * ch_get(t_table * p_table, const char * p_key);

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
void * ch_delete(t_table * p_table, const char * p_key);

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
void ch_clear(t_table * p_table);

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
t_table * ch_create(unsigned long int table_size);

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
void ch_destroy(t_table * p_table);

#endif

