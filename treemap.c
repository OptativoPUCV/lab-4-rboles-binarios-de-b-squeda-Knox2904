#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treemap.h"

typedef struct TreeNode TreeNode;


struct TreeNode {
    Pair* pair;
    TreeNode * left;
    TreeNode * right;
    TreeNode * parent;
};

struct TreeMap {
    TreeNode * root;
    TreeNode * current;
    int (*lower_than) (void* key1, void* key2);
};

int is_equal(TreeMap* tree, void* key1, void* key2){
    if(tree->lower_than(key1,key2)==0 &&  
        tree->lower_than(key2,key1)==0) return 1;
    else return 0;
}

TreeNode * createTreeNode(void* key, void * value) {
    TreeNode * new = (TreeNode *)malloc(sizeof(TreeNode));
    if (new == NULL) return NULL;
    new->pair = (Pair *)malloc(sizeof(Pair));
    new->pair->key = key;
    new->pair->value = value;
    new->parent = new->left = new->right = NULL;
    return new;
}

TreeMap * createTreeMap(int (*lower_than) (void* key1, void* key2)) {

    TreeMap *MapAux = malloc(sizeof(TreeMap)) ; 
    if(MapAux==NULL)exit(EXIT_FAILURE) ; 
    
    MapAux->current = NULL ;   
    MapAux->root = NULL ; 
    MapAux->lower_than = lower_than;

    return MapAux;
}

void insertTreeMap(TreeMap * tree, void* key, void * value) {

    if(searchTreeMap(tree , key) != NULL) return ;
    Pair* par = malloc(sizeof(Pair)) ; 
    if(par== NULL)exit(EXIT_FAILURE) ; 
    par->key = key ; 
    par->value  = value ;
    //--------------------------
    TreeNode* NodoTemp = malloc(sizeof(TreeNode)) ; // me acabo de dar  cuenta que hay una funcion para crear el nodo :D
    NodoTemp->pair = par ; 
    NodoTemp->left = NULL ; 
    NodoTemp->right= NULL ; 
    NodoTemp->parent = NULL ; 
    //--------------------------
    TreeNode* currentAux = NULL ; // no va a ser confuso para nada que se llamen igual
    TreeNode* currentReal = tree->root ; 
    //--------------------------
    while(currentReal !=  NULL){
        currentAux = currentReal ; //lo actualizo para saber donde estoy , aux es aparte del current real
        if(tree->lower_than(key , currentReal->pair->key)){
            currentReal = currentReal->left ; 
        }
        else if(tree->lower_than(currentReal->pair->key , key)){
            currentReal = currentReal->right ; 
        }
    }
    NodoTemp->parent = currentAux ;  // le actualizo la posicion a la correcta 
    //--------------------------
    if(currentAux == NULL){ //no ciclos , arbol vacio
        tree->root = NodoTemp ; 
    }
    else if(tree->lower_than(key , currentAux->pair->key)){
        currentAux->left = NodoTemp ; 
    }
    else {
        currentAux->right = NodoTemp ; 
    }
    //--------------------------
    tree->current = NodoTemp ; // se actualiza el current al nuevo nodo creado si todo esta bien 
}

TreeNode * minimum(TreeNode * x){
    if(x== NULL) return NULL ; 

    while(x->left != NULL){
        x = x->left ; 
    }

    return x;
}

void removeNode(TreeMap * tree, TreeNode* node) {

    if (node == NULL) return ; //llamemoslo caso 0 , nodo sin nada o inexistente por asi decirlo   

    //---------------------------------------------------------
    //caso1 nodo sin hijos
    if(node->left == NULL && node->right == NULL){

        if(node->parent== NULL){ // raiz == nodo o alrevez
            tree->root = NULL ; 
        } 
        else {

            if(node->parent->left == node){
                node->parent->left = NULL ; 
            }
            else (node->parent->right = NULL) ; 

        }
        free(node->pair) ; 
        free(node) ; 
        return;
    }
    //---------------------------------------------------------
    //caso2 nodo con hijo unico 
    if(node->left == NULL || node->right == NULL){
        TreeNode* hijo = (node->left != NULL) ? node->left : node->right ; // operadores ternarios :D 
        
        if(node->parent == NULL){
            tree->root = hijo ; //basicamente lo mismo de arriba pero ahora con el hijo en vez de null
        }
        else{

            if(node->parent->left == node){
                node->parent->left = hijo ; 
            }
            else (node->parent->right = hijo) ; 
        }
    
        hijo->parent = node->parent ; // se mueve el nodo por asi decirlo para terminar
        free(node->pair) ; 
        free(node) ; 
        return;
    }
    //---------------------------------------------------------
    //caso 3 nodo con dos hijos
    //no se podrian hacer cada una como funcion acaso? , para llamarla solo cuando sea necesario?
    TreeNode* elMasApto = minimum(node->right) ; 

    Pair*aux = node->pair ; // guardemos el par
    node->pair = elMasApto->pair ; 

    removeNode(tree , elMasApto) ; // confiamos en el proceso de la recurisvidad
    //en realidad simplemente es que ahora se trata como cualquiera de los otros dos casos de arriba
    //me acabo de dar cuenta de el consejo del profe en la tarea, tendria que haberla leido antes
    free(aux) ; 

}

void eraseTreeMap(TreeMap * tree, void* key){
    if (tree == NULL || tree->root == NULL) return;

    if (searchTreeMap(tree, key) == NULL) return;
    TreeNode* node = tree->current;
    removeNode(tree, node);

}


Pair * searchTreeMap(TreeMap * tree, void* key) {

    TreeNode *current = tree->root ; 
    while(current != NULL){
        if(tree->lower_than(key , current->pair->key)){
            current = current->left ; 
        }
        else if(tree->lower_than(current->pair->key , key)){
            current = current->right ; 
        }
        else {
            tree->current = current ; 
            return current->pair ; 
        }
    }

    return NULL;
}

Pair * upperBound(TreeMap * tree, void* key) {
    return NULL;
}

Pair * firstTreeMap(TreeMap * tree) {
    if(tree== NULL || tree->root == NULL) return NULL ; //basicamente lo mismo que minimum
    TreeNode* minNodo = minimum(tree->root) ; // reutilizamos funciones
    tree->current= minNodo ; // cambio el current
    return (minNodo != NULL) ? minNodo->pair : NULL ; // operadores ternarios 2 
}

Pair * nextTreeMap(TreeMap * tree) {
    if(tree== NULL || tree->root == NULL) return NULL ; //basicamente lo mismo que minimum y que arriba

    TreeNode* current = tree->current ; // para que sea mas facil , lo dejo en una variable 

    if (current->right!= NULL){
        TreeNode* elMasApto = minimum(current->right) ; 
        tree->current = elMasApto ; //actualizamos el current
        return elMasApto->pair ; 
    }

    TreeNode* padre = current->parent ; 
    while(padre != NULL && current == padre->left){
        current = padre ; 
        padre = padre->parent ;  // subiendo en el arbol 
    }

    tree->current = padre ; 
    return (padre != NULL) ? padre->pair : NULL; // ternarios 3 
}
