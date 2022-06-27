#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#ifndef NODE_H
#define NODE_H


/*


The sig info now contains the EWt
*/
typedef struct sig_info
{
  int type;
  int id;
  float form;
  float amp;
  float freq;
  float phase;
  float inverse;
  int mute;

  unsigned short * current_index;
  /*Only Used for drawing the signal*/
  float * signal;
  /* Elementary Wave Table*/
  float * ewt;


}sig_info;

//An event could be anything the user has done
//We should distinguish between events, some are to add\ delete new signals
//others are just changing the controll value
/*
Here is therefore the protocole for this implementation
 * Fist approach, no management for controll undo
    - when a node is deleted its info is pushed to a last-event stack
    - when there are more than 5 events on the stack it will clean the last event automatically
*/
typedef struct last_events_stack
{ 
  struct sig_info *value;
  struct last_events_stack *next;
  int id;
}last_events_stack;


typedef struct node
{
    struct sig_info *value;
    struct node *next;
} node;

int node_val_count(node* head);
void node_extract_min(node* list,node *sup);
void print_sine_info(sig_info * sine_data);
void node_insert_beg(node* head, sig_info * value);
void node_insert_end(node* head, sig_info * value);
sig_info * init_null_struct();
node* node_build_sentinel();
void node_insert_sort(node * head, sig_info * value);
node * node_get_at(node* node, int index);
void node_free(node* head);
int node_delete_first_occurence(node* head, node* sup, int id);
void node_print(node* head);
void node_lower_id(node *head, int after_id, last_events_stack * last_events);
last_events_stack *last_events_stack_build_sentinel();
void last_events_stack_free(last_events_stack *head);
void last_events_stack_print(last_events_stack *head);
void last_events_stack_push(last_events_stack *head, sig_info *value);
sig_info *stack_pop(last_events_stack *head);

#endif