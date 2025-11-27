/*-----------------------------------------------------------------*/
/*
 Licence Informatique - Structures de données
 Mathias Paulin (Mathias.Paulin@irit.fr)
 
 Implantation du TAD List vu en cours.
 */
/*-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

typedef struct s_LinkedElement {
	int value;
	struct s_LinkedElement* previous;
	struct s_LinkedElement* next;
} LinkedElement;

/* Use of a sentinel for implementing the list :
 The sentinel is a LinkedElement* whose next pointer refer always to the head of the list and previous pointer to the tail of the list
 */
struct s_List {
	LinkedElement* sentinel;
	int size;
};

// Structure SubList  (dans list.c : 2 raison 1 Ont a dit qu'on toucher pas a list.h 2 c'est interne pcq ont l'utilise que dedans et que c'est pas utile pour les autre fonction)
typedef struct s_SubList {
	struct s_LinkedElement* tete;
	struct s_LinkedElement* queue;
} SubList;


/*-----------------------------------------------------------------*/

List* list_create(void) {
	List* l;
	l = malloc(sizeof(struct s_List) + sizeof(LinkedElement));
	l->sentinel = (LinkedElement*)(l+1);
	l->sentinel->previous = l->sentinel;
	l->sentinel->next = l->sentinel;
	l->size = 0;
	return l;
}

/*-----------------------------------------------------------------*/

List* list_push_back(List* l, int v) {
	LinkedElement* e = malloc(sizeof(LinkedElement));
	e->value = v;
	e->next = l->sentinel;
	e->previous = e->next->previous;
	e->previous->next = e;
	e->next->previous = e;
	l->size++;
	return l;
}

/*-----------------------------------------------------------------*/

void list_delete(ptrList* l) {
	LinkedElement* actuel = (*l)->sentinel->next;
	while (actuel!=(*l)->sentinel) {
		LinkedElement* suivant=actuel->next;
		free(actuel);
		actuel=suivant;
	}
	free(*l);
	*l=NULL;
}

/*-----------------------------------------------------------------*/

List* list_push_front(List* l, int v) {
	LinkedElement* e = malloc(sizeof(LinkedElement));
	e->value = v;
	e->previous = l->sentinel;
	e->next = e->previous->next;
	e->next->previous = e;
	e->previous->next = e;
	l->size++;
	return l;
}

/*-----------------------------------------------------------------*/

int list_front(const List* l) {
	assert(!list_is_empty(l));
	return l->sentinel->next->value;
}

/*-----------------------------------------------------------------*/

int list_back(const List* l) {
	assert(!list_is_empty(l));
	return l->sentinel->previous->value;
}

/*-----------------------------------------------------------------*/

List* list_pop_front(List* l) {
	assert(!list_is_empty(l));
	LinkedElement* e = l->sentinel->next;
	l->sentinel->next = e->next;
	l->sentinel->next->previous = l->sentinel;
	l->size--;
	free(e);
	return l;
}

/*-----------------------------------------------------------------*/

List* list_pop_back(List* l){
	assert(!list_is_empty(l));
	LinkedElement* e = l->sentinel->previous;
	l->sentinel->previous = e->previous;
	l->sentinel->previous->next = l->sentinel;
	l->size--;
	free(e);
	return l;
}

/*-----------------------------------------------------------------*/

List* list_insert_at(List* l, int p, int v) {
	LinkedElement* curseur = l->sentinel->next;
	while (p!=0) {
		p--;
		curseur=curseur->next;
	}
	LinkedElement* e = malloc(sizeof(LinkedElement));
	e->value = v;
	e->next = curseur;
	e->previous = curseur->previous;
	e->previous->next = e;
	e->next->previous = e;
	l->size++;
	return l;
}

/*-----------------------------------------------------------------*/

List* list_remove_at(List* l, int p) {
	LinkedElement* curseur = l->sentinel->next;
	while (p!=0) {
		p--;
		curseur=curseur->next;
	}
	curseur->previous->next = curseur->next;
	curseur->next->previous = curseur->previous;
	free(curseur);
	l->size--;
	return l;
}

/*-----------------------------------------------------------------*/

int list_at(const List* l, int p) {
	LinkedElement* curseur = l->sentinel->next;
	while (p!=0) {
		p--;
		curseur=curseur->next;
	}
	return curseur->value;
}

/*-----------------------------------------------------------------*/

bool list_is_empty(const List* l) {
	return l->size==0;
}

/*-----------------------------------------------------------------*/

int list_size(const List* l) {
	return l->size;
}

/*-----------------------------------------------------------------*/

List* list_map(List* l, ListFunctor f, void* environment) {
	for (LinkedElement* e = l->sentinel->next; e!= l->sentinel; e = e->next) e->value=f(e->value,environment);
	return l;
}

/*-----------------------------------------------------------------*/

// DECOUPE L EN DEUX SOUS LISTE (Taille = a 1 elem près).
// RENVOIE SUBLIST :
// Tete => LE DERNIER ELEMENT DE LA SOUS LISTE GAUCHE
// Queue => LE PREMIER ELEMENT DE LA SOUS LISTE DROITE.
SubList list_split(SubList l) {
	SubList output;

	LinkedElement* curdebut = l.tete;
	LinkedElement* curfin = l.queue;

	while (curdebut != curfin && curdebut->next != curfin) {
		curdebut = curdebut->next;
		curfin = curfin->previous;
	}
	output.tete=curdebut; // DERNIER ELEMENT Liste GAUCHE
	output.queue=curdebut->next; // PREMIER ELEMENT Liste DROITE
	output.tete->next=NULL; // COUPURE
	output.queue->previous=NULL;
	return output;
}

SubList list_merge(SubList leftlist, SubList rightlist, OrderFunctor f) {
	SubList output;

	// TÊTE = PREMIER ELEMENT
	LinkedElement* curL = leftlist.tete;
	LinkedElement* curR = rightlist.tete;

	// SI l'UNE LISTE EST VIDE
	if (curL==NULL) return rightlist;
	if (curR==NULL) return leftlist;

	// PREMIER ELEM DEF EN FONCTION DE F
	if(f(curL->value,curR->value)) {
		output.tete = curL;
		curL = curL->next;
	} else {
		output.tete=curR;
		curR = curR->next;
	}

	LinkedElement* curOut = output.tete;

	while (curL!=NULL && curR!=NULL) {
		if(f(curL->value,curR->value)) {
			curOut->next = curL;
			curL->previous = curOut;
			curOut = curOut->next;
			curL = curL->next;
		} else {
			curOut->next = curR;
			curR->previous = curOut;
			curOut = curOut->next;
			curR = curR->next;
		}
	}

	// COMPLETE FIN LISTE MANQUANTE

	while (curL!=NULL) {
		curOut->next = curL;
		curL->previous = curOut;
		curOut = curOut->next;
		curL = curL->next;
	}

	while (curR!=NULL) {
		curOut->next = curR;
		curR->previous = curOut;
		curOut = curOut->next;
		curR = curR->next;
	}

	// COUPUR
	output.tete->previous= NULL;
	output.queue = curOut;
	output.queue->next=NULL;

	return output;
}

SubList list_mergesort(SubList l, OrderFunctor f) {
	// CAS LISTE VIDE ET 1 ELEM
	if (l.tete == NULL || l.tete == l.queue) {
		return l;
	}

	//1ER TEMPS -> DECOUPE l EN 2 (L et R)
	SubList midcur = list_split(l);

	SubList leftList;
	leftList.tete = l.tete;
	leftList.queue = midcur.tete;

	SubList rightList;
	rightList.tete = midcur.queue;
	rightList.queue = l.queue;

	//2IEME TEMPS -> RECURSION LIST_MERGSORT
	leftList = list_mergesort(leftList,f);
	rightList = list_mergesort(rightList,f);

	//3IEME TEMPS -> FUSION DES 2 LISTE
	SubList fusion = list_merge(leftList,rightList,f);

	return fusion;

}

List* list_sort(List* l, OrderFunctor f) {
	// List sentinel -> SubList
	SubList noSentinel;
	noSentinel.tete = l->sentinel->next;
	noSentinel.queue = l->sentinel->previous;

	// ONT COUPE CIRCULAIRE
	noSentinel.tete->previous = NULL;
	noSentinel.queue->next = NULL;

	// list_mergesort(sublist)
	noSentinel = list_mergesort(noSentinel,f);

	// SubList -> List sentinel
	l->sentinel->next = noSentinel.tete;
	l->sentinel->previous = noSentinel.queue;
	l->sentinel->next->previous = l->sentinel;
	l->sentinel->previous->next = l->sentinel;
	return l;
}