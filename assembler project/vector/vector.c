#include "vector.h"
#include <stdlib.h>

#define VECTOR_BEGIN_SIZE 12

struct vector {
    void ** items;
    size_t  pointer_count;
    size_t  item_count;
    void *  (*ctor)(const void *copy);
    void    (*dtor)(void *item);
};

/**
 * @brief defining a new vector, with his ctor and dtor functions.
 * 
 * @param ctor - the function that saving place for our vector at the beginning.
 * @param dtor - the function that destroying place for our vector at the end.
 * @return Vector - a ready vector with functions the copy it and destroy it.
 */
Vector new_vector(void * (*ctor)(const void *copy),void (*dtor)(void *item)) {
    Vector new = calloc(1,sizeof(struct vector));
    if(new == NULL)
        return NULL;
    new->pointer_count = VECTOR_BEGIN_SIZE;
    new->items = calloc(VECTOR_BEGIN_SIZE,sizeof(void*));
    if(new->items == NULL) {
        free(new);
        return NULL;
    }
    new->ctor = ctor;
    new->dtor = dtor;
    return new;
}

/**
 * @brief put something into the vector
 * 
 * @param v - the vector we want to save it in our vector.
 * @param copy_item - the item we would like to save in our vector.
 * @return void* - 'null' if there's no place for the item, otherwise- the last item in our vector list.
 */
void * vector_insert(Vector v,const void * copy_item) {
    size_t it;
    void ** temp;
    if(v->item_count == v->pointer_count) {
        v->pointer_count *=2;
        temp = realloc(v->items,v->pointer_count * sizeof(void *));
        if(temp == NULL) {
            v->pointer_count /= 2;
            return NULL;
        }
        v->items = temp;
        v->items[v->item_count] = v->ctor(copy_item);
        if(v->items[v->item_count] ==NULL) {
            return NULL;
        }
        v->item_count++;
        for(it = v->item_count; it < v->pointer_count; it++) {
            v->items[it] = NULL;
        }
    }else {
        for(it = 0;it<v->pointer_count;it++) {
            if(v->items[it] == NULL) {
                v->items[it] = v->ctor(copy_item);
                if(v->items[it] != NULL) {
                    v->item_count++;
                    break;    
                }
                return NULL;
            }
        }
    }
    return v->items[v->item_count-1];
}

/**
 * @brief pointing on the first address of the vector.
 * 
 * @param v - a vector that we want to get it's starting address
 * @return void* const* - the address of it's first item.
 */
void *const *vector_begin(const Vector v) {
    return v->items;
}

/**
 * @brief pointing on the last address of the vector
 * 
 * @param v - a vector that we want to get it's starting address
 * @return void* const* - the address of it's first item.
 */
void *const *vector_end(const Vector v) {
    return &v->items[v->pointer_count - 1];
}

/**
 * @brief returning to the user how many items are in the vector.
 * 
 * @param v - the vector we would like to check.
 * @return size_t - the amount of items in this vector.
 */
size_t vector_get_item_count(const Vector v) {
    return v->item_count; 
}

/**
 * @brief cleaning the vector from the memory.
 * 
 * @param v - the vector we would like to destroy and remove him from our memory
 */
void vector_destroy(Vector * v) {
    size_t it;
    if(*v != NULL) {
        if((*v)->dtor != NULL) {
            for(it = 0;it < (*v)->pointer_count; it++) {
                if((*v)->items[it] != NULL)
                    (*v)->dtor((*v)->items[it]);
            }
        }
        free((*v)->items);
        free(*v);
        *v = NULL;
    }
}
