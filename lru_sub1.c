#include "lru.h"
#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

void lru_init_queue(Set *set) {
  LRUNode *s = NULL;
  LRUNode **pp = &s;  // place to chain in the next node
  for (int i = 0; i < set->line_count; i++) {
    Line *line = &set->lines[i];
    LRUNode *node = (LRUNode *)(malloc(sizeof(LRUNode)));
    node->line = line;
    node->next = NULL;
    (*pp) = node;
    pp = &((*pp)->next);
  }
  set->lru_queue = s;
}

void lru_init(Cache *cache) {
  Set *sets = cache->sets;
  for (int i = 0; i < cache->set_count; i++) {
    lru_init_queue(&sets[i]);
  }
}

void lru_destroy(Cache *cache) {
  Set *sets = cache->sets;
  for (int i = 0; i < cache->set_count; i++) {
    LRUNode *p = sets[i].lru_queue;
    LRUNode *n = p;
    while (p != NULL) {
      p = p->next;
      free(n);
      n = p;
    }
    sets[i].lru_queue = NULL;
  }
}

void lru_fetch(Set *set, unsigned int tag, LRUResult *result) {
  // TODO:
  // Implement the LRU algorithm to determine which line in
  // the cache should be accessed.
  //
  LRUNode *prev = NULL;
  LRUNode *lru = NULL;
  LRUNode *curr = set->lru_queue;
  
  //Iterate over the linked list:
  while (curr != NULL){
    //1. if the line is valid and the tag matches, we have a hit and set the result access to HIT
    if(curr->line->valid && curr->line->tag == tag){
      if (prev != NULL) {
	prev->next = curr->next;
          } else {
              result->line = curr->line;
              result->access = HIT;
              return;
            }
    }
    if (!curr->line->valid){
      lru = curr;
    }
    prev = curr;
    curr = curr->next;
	      //2. if we did not find a matching tag and the set is not full,
	     //use a line with valid bit 0 to represent the tag and set the result access to COLD_MISS
    if (lru != NULL){
      lru->line->valid = 1;
      lru->line->tag = tag;
      result->line = lru->line;
      result->access = COLD_MISS;
    }
	     //3. if we did not find a matching tag and the set is full,
	     //replace the last line and set the result access to CONFLICT_MISS
    else{
      LRUNode *new_lru = NULL;
      curr = set->lru_queue;
      while (curr->next != NULL) {
          new_lru = curr;
          curr = curr->next;
        }
      if(new_lru != NULL){
	new_lru->next = NULL;
      }
      if (curr != set->lru_queue){
	curr->next = set->lru_queue;
	set->lru_queue = curr;
      }
	
             //Set the result line to the line you use to represent the Tag.                                                                                                                                         
             //Update the list so the line will be the most recent one.     
        curr->line->tag = tag;
        result->line = curr->line;
        result->access = CONFLICT_MISS;
    }
}
}
