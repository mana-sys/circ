//
// Created by mana on 8/12/19.
//

#ifndef INTLIST_H
#define INTLIST_H

struct intlist_node_s {

};

struct intlist_s {
    struct intlist_node_s *head;
};

struct intlist_s *intlist_new();
int intlist_append();

#endif //CIRC_INTLIST_H
