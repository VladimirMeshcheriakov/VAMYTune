#include "node.h"

// Prints a string of characters.
static inline void print(const char *f, ...)
{
    va_list args;
    va_start(args, f);
    vprintf(f, args);
    va_end(args);
}

// Prints a string of characters followed by a new line.
static inline void println(const char *f, ...)
{
    va_list args;
    va_start(args, f);
    vprintf(f, args);
    va_end(args);
    printf("\n");
}

static inline void *mem_alloc(size_t size)
{
    void *p = malloc(size);

    if (p == NULL)
    {
        err(EXIT_FAILURE, "Problem with malloc().");
    }
    return p;
}

sig_info *init_null_struct()
{
    sig_info *sine_data = malloc(sizeof(sig_info));
    sine_data->amp = 0;
    sine_data->freq = 0;
    sine_data->form = 0;
    sine_data->phase = 0;
    sine_data->id = -1;
    sine_data->mute = 0;
    sine_data->inverse = 0;
    sine_data->signal = calloc(1024,sizeof(float));
    return sine_data;
}



void print_sine_info(sig_info *sine_data)
{
    switch (sine_data->type)
    {
    case 0:
        printf("{ Sine id %d , amp: %f, freq: %f, phase %f\n", sine_data->id, sine_data->amp, sine_data->freq, sine_data->phase);
        break;
    case 1:
        printf("{ Triangle id %d ,  amp: %f, freq: %f\n", sine_data->id, sine_data->amp, sine_data->freq);
        break;
    case 2:
        printf("{ Saw id %d ,  amp: %f, freq: %f\n", sine_data->id, sine_data->amp, sine_data->freq);
        break;
    case 3:
        printf("{ Saw_composite id %d , components: %f , amp: %f, freq: %f\n", sine_data->id, sine_data->form, sine_data->amp, sine_data->freq);
        break;
    case 4:
        printf("{ Square id %d , form: %f , amp: %f, freq: %f\n", sine_data->id, sine_data->form, sine_data->amp, sine_data->freq);
        break;

    default:
        break;
    }
}

// Verify if node is empty
int node_is_empty(node *head)
{
    return head == NULL;
}

// Verify if node is not empty
int node_is_not_empty(node *head)
{
    return head != NULL;
}

// Builds the sentinell of the node structure

node *node_build_sentinel()
{
    // Creates the sentinel.
    node *head = mem_alloc(sizeof(node));
    head->value = init_null_struct();
    head->next = NULL;
    // Returns the head of the node which is the sentinell.
    return head;
}

// Prints the contents of a node node* node_build_sentinel()
void node_print(node *head)
{
    print("{");
    while (head->next)
    {
        head = head->next;
        print_sine_info(head->value);
    }
    println(" }");
}

// Frees the allocated node
void node_free(node *head)
{
    node *previous;

    while (head)
    {
        previous = head;
        head = head->next;
        //free(previous->value->signal);
        free(previous);
    }
}

// Inserts a value right after the head
/*

    HEAD -> 1 -> 2 -> ..... -> 8
    node_insert_beg(node* HEAD, int 42);
    HEAD -> 42 -> 1 -> 2 -> ..... -> 8

*/
void node_insert_beg(node *head, sig_info *value)
{
    node *tmp = mem_alloc(sizeof(node));
    tmp->value = value;
    tmp->next = head->next;
    head->next = tmp;
}

// Inserts a value right after the head
/*

    HEAD -> 1 -> 2 -> ..... -> 8
    node_insert_end(node* HEAD, int 42);
    HEAD -> 1 -> 2 -> ..... -> 8 -> 42

*/
void node_insert_end(node *head, sig_info *value)
{
    node *tmp = mem_alloc(sizeof(node));
    for (; node_is_not_empty(head->next); head = head->next)
    {
        // This loop runs to the last node and quits with head being that last node
        continue;
    }
    tmp->value = value;
    tmp->next = head->next;
    head->next = tmp;
}

// Inserts odd values to the end and even values to the beginning
void node_insert_num_odd_and_even(node *head, sig_info *value)
{
    // odd
    if (value->id % 2)
    {
        node_insert_end(head, value);
    }
    // even
    else
    {
        node_insert_beg(head, value);
    }
}

// Extracts the minimum value of the node (in other words deletes it from the node)
void node_extract_min(node *list, node *sup)
{
    node *before_min = list;
    while (list->next != NULL)
    {
        if (list->next->value->id < before_min->next->value->id)
        {
            before_min = list;
        }
        list = list->next;
    }
    node *output = before_min->next;
    before_min->next = before_min->next->next;
    output->next = NULL;
    while (sup->next != NULL)
    {
        sup = sup->next;
    }
    sup->next = output;
}

// Extracts the maximum value of the node (in other words deletes it from the node)
void node_extract_max(node *list, node *sup)
{
    node *before_max = list;
    while (list->next != NULL)
    {
        if (list->next->value->id > before_max->next->value->id)
        {
            before_max = list;
        }
        list = list->next;
    }
    node *output = before_max->next;
    before_max->next = before_max->next->next;
    output->next = NULL;
    while (sup->next != NULL)
    {
        sup = sup->next;
    }
    sup->next = output;
}

// Inserts a node into a SORTED linked list
void node_insert_sort(node *head, sig_info *value)
{
    node *tmp = mem_alloc(sizeof(node));
    tmp->value = value;
    // Run through the nodes until it is not empty and the value of the next node is smaller than value parameter
    for (; node_is_not_empty(head->next) && (head->next->value->id >= value->id); head = head->next)
    {
        continue;
    }
    tmp->next = head->next;
    head->next = tmp;
}

// Sorts any linked list from largest values to the smallest
void node_sort_linked_list(node *head)
{
    // Linear sort, find max and swap it with the first val

    // If there is only a sentinel
    if (node_is_empty(head->next))
    {
        // Do nothing since there is only a sentinel
    }
    else
    {
        node *tmp;
        tmp = node_build_sentinel();
        while (node_is_not_empty(head->next))
        {
            node_extract_max(head, tmp);
        }
        head->next = tmp->next;
        free(tmp);
    }
}

// Deletes the first occurence of value in node
int node_delete_first_occurence(node *head, node *sup, int id)
{
    int seen = 0;
    node *tmp = head;
    while (head->next != NULL)
    {
        if (head->next->value->id == id)
        {
            tmp = head;
            seen += 1;
            break;
        }
        head = head->next;
    }
    if (seen == 0)
    {
        return seen;
    }
    node *output = head->next;
    tmp->next = tmp->next->next;
    output->next = NULL;
    while (sup->next != NULL)
    {
        sup = sup->next;
    }
    sup->next = output;
    return seen;
}

// Deletes all occurences of value in node
int node_delete_all_occurences(node *head, node *sup, int id)
{
    int seen = 0;
    node *tmp = head;
    while (head->next != NULL)
    {
        if (head->next->value->id == id)
        {
            seen += 1;
            tmp = head;
            node *output = head->next;
            tmp->next = tmp->next->next;
            output->next = NULL;
            while (sup->next != NULL)
            {
                sup = sup->next;
            }
            sup->next = output;
            continue;
        }
        head = head->next;
    }
    return seen;
}

// Get a node at index if index invalid return NULL
// DOES NOT DELETE THE NODE
node *node_get_at(node *node, int index)
{
    while (index > 0)
    {
        node = node->next;

        index--;
    }
    if (node != NULL)
    {
        node = node->next;
    }
    return node;
}

// lover each id after the id in the struct by 1
void node_lower_id(node *head, int after_id, last_events_stack *last_events)
{
    int seen = 0;
    node *tmp = head;
    while (head->next != NULL)
    {
        if (head->next->value->id >= after_id)
        {
            if (seen == 0)
            {
                tmp = head;
                node *output = head->next;
                tmp->next = tmp->next->next;
                output->next = NULL;
                // Free the deleted node
                // Or put it to the delete stack
                last_events_stack_push(last_events, output->value);
                last_events_stack_print(last_events);
                // free(output->value);
                free(output);
                seen = 1;
                continue;
            }
            head->next->value->id -= 1;
        }
        head = head->next;
    }
}

int node_val_count(node *head)
{
    int cpt = 0;
    while (head->next != NULL)
    {
        cpt += 1;
        head = head->next;
    }
    return cpt;
}

// Init a last_event_stacl struct
last_events_stack *last_events_stack_build_sentinel()
{
    // Creates the sentinel.
    last_events_stack *head = mem_alloc(sizeof(last_events_stack));
    head->value = init_null_struct();
    head->next = NULL;
    head->id = -1;
    // Returns the head of the node which is the sentinell.
    return head;
}

// Frees the allocated last_events_stack
void last_events_stack_free(last_events_stack *head)
{
    last_events_stack *previous;

    while (head)
    {
        previous = head;
        head = head->next;
        free(previous);
    }
}

// Prints the contents of a last_enets_stack
void last_events_stack_print(last_events_stack *head)
{
    print("{");
    while (head->next)
    {
        head = head->next;
        head->value->id = head->id;
        print_sine_info(head->value);
    }
    println(" }");
}

// Push a newly deleted signal to the stack
// And clear the last element of the stack if there are too many elements
void last_events_stack_push(last_events_stack *head, sig_info *value)
{
    last_events_stack *tmp = mem_alloc(sizeof(last_events_stack));
    if (head->next !=NULL)
    {
        tmp->value = value;
        tmp->id = head->next->id + 1;
        tmp->next = head->next;
        head->next = tmp; 
        if(head->next->id > 4)
        {
            // Too many elements, pop the last one!
            // lower all the ids and insert
            //skip the sentinell
            head = head->next;
            while (head->id > 1)
            {
                head->id -= 1;
                head = head->next;
            }
            // We are on the previous to last element
            last_events_stack *tmp = head->next;
            head->next = NULL;
            //Get the sig_info for the popped value here!
            free(tmp->value);
            free(tmp);
            head->id = 0;
        }
    }
    else
    {
        tmp->value = value;
        tmp->id = head->id + 1;
        tmp->next = head->next;
        head->next = tmp;
    }
}

// Pop the first element of the stack to restore it
sig_info *stack_pop(last_events_stack *head)
{
    if (head->next == NULL)
    {
        fprintf(stderr, "Stack is empty!\n");
        return init_null_struct();
    }
    else
    {
        sig_info *pop_data = malloc(sizeof(sig_info));
        last_events_stack *tmp;
        tmp = head->next;
        pop_data = tmp->value;
        if (head->next->next != NULL)
        {
            head->next = head->next->next;
            tmp->next = NULL;
        }
        else
        {
            head->next = NULL;
        }
        free(tmp);
        return pop_data;
    }
}