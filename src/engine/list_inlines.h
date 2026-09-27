/* Shared inline definitions. Select helpers before inclusion, then undefine their selectors.
 * BYTES(inline): repeated inclusion preserves each caller's dependency visibility and definition order.
 * Per-helper guards prevent redefinition when a later include selects more helpers. */

#if defined(SDW_INLINE_FREE_LIST_FIRST_LISTNODE) && !defined(SDW_INLINE_FREE_LIST_FIRST_LISTNODE_DEFINED)
#define SDW_INLINE_FREE_LIST_FIRST_LISTNODE_DEFINED
/* BYTES(inline): the list-head accessor preserves the Progress/Inventory
 * expansion temporaries. */
inline ListNode *List_First(ListNode **list)
{
    return *list;
}
#endif
