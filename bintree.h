/**
 * @file bintree.h
 * @brief Hlavickovy soubor struktury binarniho vyhledavaciho stromu
 *
 * @authors Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */
#ifndef _BINTREE_H_
#define _BINTREE_H_

struct bintree_t;
struct tree_item_t;

typedef struct tree_item_t * ItemPtr;

ItemPtr init_item(char *);
void destroy_item(ItemPtr *);

int init_tree(struct bintree_t *);
ItemPtr insert_tree(struct bintree_t *, char *);
ItemPtr search_tree(struct bintree_t *, char *);
int delete_item(struct bintree_t *, char *);
void destroy_tree(struct bintree_t *);

#endif // _BINTREE_H_
