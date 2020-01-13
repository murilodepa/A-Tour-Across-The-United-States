//
//  Stackld.h
//  Exercícios pilha
//===============================================================
// esta biblioteca foi construida especificamente para o teste dos
//  exercicios.
//===============================================================
// a representacao utilizada para esta biblioteca foi a
// representacao ligada dinamica.
//================================================================
//  Created by Angela Engelbrecht on 03/04/19.
//  Copyright © 2019 Angela Engelbrecht. All rights reserved.
//

#ifndef Stackld_h
#define Stackld_h
#include <iostream>
#include <stdlib.h>

using namespace std;


struct no {
    stack_element info;
    no * link;
};

struct Stack {
    no * top;
};

void initStack     (Stack&);
void push          (Stack& , stack_element);
stack_element pop  (Stack&);
stack_element peek (Stack);
int isEmpty        (Stack);
void error          (int);

void initStack(Stack &p){ p.top= NULL;}

no* novo_no(){
    no* novo;
    novo = new no;
    if (!novo) error(1);
    return novo;
}

void push(Stack &p, stack_element a){
    no * novo = novo_no();
    novo->info = a;
    novo->link = p.top;
    p.top = novo;
}

stack_element  pop (Stack& p){
    if(p.top == NULL) error (2);
    stack_element v = p.top ->info;
    no * x = p.top;
    p.top = (p.top)->link;
    delete x;
    return v;
}
stack_element  peek (Stack p){
    if(p.top == NULL) error(2);
    return  (p.top) ->info;
}

int isEmpty(Stack p){
    if (p.top == NULL)  return 1; return 0;
}

void error(int i){
    switch(i){
        case 1:cerr<< " OVERFLOW "; exit(1);
        case 2:cerr<< " UNDERFLOW ";exit(1);
    }
}


#endif /* Stackld_h */
