#include <stdio.h>
#include "list.h"

List list_arr[LIST_MAX_NUM_HEADS];
Node node_arr[LIST_MAX_NUM_NODES];
bool list_used[LIST_MAX_NUM_HEADS] = {false};
int list_arr_index = 0;
int node_arr_index = 0;

List* List_create(){
    if(list_arr_index >= LIST_MAX_NUM_HEADS){
        for(int i = 0; i < LIST_MAX_NUM_HEADS; i++){
            if(list_used[i] == false){
                list_arr_index = i;
                list_used[i] = true;
                break;
            }
        }
        return NULL;
    }
    List* newList = &list_arr[list_arr_index++];
    newList->head = -1;
    newList->current = LIST_OOB_START;
    newList->end = -1;
    newList->count = 0;
    newList->track_used = list_arr_index - 1;
    newList->bound = false;
    return newList;
}

int List_count(List* pList){
    if(pList == NULL){
        return LIST_FAIL;
    }
    return pList->count;
}

void* List_first(List* pList){
    if(pList == NULL){
        return NULL;
    }
    if(pList->head == -1){
        pList->current = -1;
        return NULL;
    }
    pList->current = pList->head;
    return node_arr[pList->current].data;
}

void* List_last(List* pList){
    if(pList == NULL){
        return NULL;
    }
    if(pList->end == -1){
        pList->current = -1;
        return NULL;
    }
    pList->current = pList->end;
    return node_arr[pList->current].data;
}

void* List_next(List* pList){
    if(pList == NULL || pList->current == -1){
        return NULL;
    }
    if(pList->current == pList->end){
        pList->current = LIST_OOB_END;
        pList->bound = true;
        return NULL;
    }
    pList->current = node_arr[pList->current].next;
    return node_arr[pList->current].data;
}

void* List_prev(List* pList){
    if(pList == NULL || pList->current == -1){
        return NULL;
    }
    if(pList->current == pList->head){
        pList->current = LIST_OOB_START;
        pList->bound = true;
        return NULL;
    }
    pList->current = node_arr[pList->current].prev;
    return node_arr[pList->current].data;
}

void* List_curr(List* pList){
    if(pList == NULL || pList->current == -1){
        return NULL;
    }
    return node_arr[pList->current].data;
}

int List_insert_after(List* pList, void* pItem){
    if(pList == NULL || node_arr_index >= LIST_MAX_NUM_NODES){
        return LIST_FAIL;
    }
    int new_node_index = node_arr_index++;
    Node* new_node = &node_arr[new_node_index];
    new_node->data = pItem;
    if(pList->current == LIST_OOB_START && (pList->count == 0 || pList->bound == true)){
        pList->bound = false;
        new_node->prev = -1;
        if(pList->head == -1){
            new_node->next = -1;
            pList->head = new_node_index;
            pList->end = new_node_index;
        } 
        else{
            new_node->next = pList->head;
            pList->head = new_node_index;
        }
    }
    else if(pList->current == LIST_OOB_END && (pList->count == 0 || pList->bound == true)){
        pList->bound = false;
        new_node->next = -1;
        if(pList->end == -1){
            pList->end = new_node_index;
        }
        else{
            new_node->prev = pList->end;
            pList->end = new_node_index;
        }
    }
    else {
        int next_node = node_arr[pList->current].next;
        new_node->next = next_node;
        new_node->prev = pList->current;
        node_arr[pList->current].next = new_node_index;
        if(next_node != -1){
            node_arr[next_node].prev = new_node_index;
        } 
        else{
            pList->end = new_node_index;
        }
    }
    pList->count++;
    pList->current = new_node_index;
    return LIST_SUCCESS;
}

int List_insert_before(List* pList, void* pItem){
    if (pList == NULL || node_arr_index >= LIST_MAX_NUM_NODES) {
        return LIST_FAIL;
    }
    int new_node_index = node_arr_index++;
    Node* new_node = &node_arr[new_node_index];
    new_node->data = pItem;
     if((pList->current == LIST_OOB_START) && (pList->count == 0 || pList->bound == true)){
        pList->bound = false;
        new_node->prev = -1;
        if(pList->head == -1){
            new_node->next = -1;
            pList->head = new_node_index;
            pList->end = new_node_index;
        } 
        else{
            new_node->next = pList->head;
            pList->head = new_node_index;
        }
    }
    else if((pList->current == LIST_OOB_END) && (pList->count == 0 || pList->bound == true)){
        pList->bound = false;
        new_node->next = -1;
        if(pList->end == -1){
            pList->end = new_node_index;
        }
        else{
            new_node->prev = pList->end;
            pList->end = new_node_index;
        }
    }
    else{
        int prev_node = node_arr[pList->current].prev;
        new_node->prev = prev_node;
        new_node->next = pList->current;
        node_arr[pList->current].prev = new_node_index;
        if(prev_node != -1){
            node_arr[prev_node].next = new_node_index;
        } 
        else{
            pList->head = new_node_index;
        }
    }
    pList->count++;
    pList->current = new_node_index;
    return LIST_SUCCESS;
}

int List_append(List* pList, void* pItem){
    if(pList == NULL || node_arr_index >= LIST_MAX_NUM_NODES){
        return LIST_FAIL;
    }
    int new_node_index = node_arr_index++;
    Node* new_node = &node_arr[new_node_index];
    new_node->data = pItem;
    new_node->next = -1;
    if(pList->head == -1){
        pList->head = new_node_index;
        pList->end = new_node_index;
        new_node->prev = -1;
    }
    else{
        new_node->prev = pList->end;
        node_arr[pList->end].next = new_node_index;
        pList->end = new_node_index;
    }
    pList->count++;
    pList->current = new_node_index;
    return LIST_SUCCESS;
}

int List_prepend(List* pList, void* pItem){
    if(pList == NULL || node_arr_index >= LIST_MAX_NUM_NODES){
        return LIST_FAIL;
    }
    int new_node_index = node_arr_index++;
    Node* new_node = &node_arr[new_node_index];
    new_node->data = pItem;
    new_node->prev = -1;
    if(pList->head == -1){
        pList->head = new_node_index;
        pList->end = new_node_index;
        new_node->next = -1;
    }
    else{
        new_node->next = pList->head;
        node_arr[pList->head].prev = new_node_index;
        pList->head = new_node_index;
    }
    pList->count++;
    pList->current = new_node_index;
    return LIST_SUCCESS;
}

void* List_remove(List* pList){
    if (pList == NULL || pList->current == -1){
        return NULL;
    }
    int curr_node = pList->current;
    int prev_node = node_arr[curr_node].prev;
    int next_node = node_arr[curr_node].next;
    if(prev_node == -1 || next_node == -1){
        if(prev_node == -1){
            pList->head = next_node;
            pList->current = LIST_OOB_START;
            pList->bound = true;
        }
        else{
            node_arr[prev_node].next = next_node;
        }
        if(next_node == -1) {
            pList->end = prev_node;
            pList->current = LIST_OOB_END;
            pList->bound = true;
        } else {
            node_arr[next_node].prev = prev_node;
        }
    }
    else{
        node_arr[prev_node].next = next_node;
        node_arr[next_node].prev = prev_node;
        pList->current = next_node;
    }
    pList->count--;
    return node_arr[curr_node].data;
}

void* List_trim(List* pList){
    if(pList == NULL || pList->end == -1){
        return NULL;
    }
    int last_node = pList->end;
    int prev_node = node_arr[last_node].prev;
    if(prev_node == -1){
        pList->head = -1;
        pList->end = -1;
        pList->current = LIST_OOB_END;
        pList->bound = true;
    }
    else{
        node_arr[prev_node].next = -1;
        pList->end = prev_node;
        pList->current = prev_node;
    }
    pList->count--;
    return node_arr[last_node].data;
}

void List_concat(List* pList1, List* pList2){
    if(pList1 == NULL || pList2 == NULL){
        return;
    }
    if(pList1->head == -1){
        pList1->head = pList2->head;
        pList1->current = pList2->current;
        pList1->end = pList2->end;
    }
    else{
        node_arr[pList1->end].next = pList2->head;
        pList1->end = pList2->end;
    }
    pList1->count += pList2->count;
    list_used[pList2->track_used] = false;
    pList2->head = -1;
    pList2->current = -1;
    pList2->end = -1;
    pList2->count = 0;
}

void List_free(List* pList, FREE_FN pItemFreeFn){
    if (pList == NULL) {
        return;
    }
    int current_node = pList->head;
    while(current_node != -1){
        if(pItemFreeFn != NULL && node_arr[current_node].data != NULL){
            (*pItemFreeFn)(node_arr[current_node].data);
        }
        current_node = node_arr[current_node].next;
    }
    list_used[pList->track_used] = false;
    pList->head = -1;
    pList->current = -1;
    pList->end = -1;
    pList->count = 0;
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
    if(pList == NULL || pComparator == NULL || pComparisonArg == NULL){
        return NULL;
    }
    int current_node = pList->current;
    while(current_node != -1){
        if((*pComparator)(node_arr[current_node].data, pComparisonArg)){
            pList->current = current_node;
            return node_arr[current_node].data;
        }
        current_node = node_arr[current_node].next;
    }
    pList->current = LIST_OOB_END;
    pList->bound = true;
    return NULL;
}