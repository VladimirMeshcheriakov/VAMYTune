#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#ifndef NODE_H
#define NODE_H



typedef struct sig_info
{
  int type;
  int id;
  float form;
  float amp;
  float freq;
  int mute;
}sig_info;


typedef struct node
{
    int stop_thread;
    struct sig_info *value;
    struct node *next;
} node;

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
void node_lower_id(node* head, int after_id);
#endif