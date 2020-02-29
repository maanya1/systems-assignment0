#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Node {
  char* info;
  struct Node* next;
};
typedef struct Node Node;

Node* makeNode(char* c) {
  Node* byt = malloc(sizeof(Node));
  byt->info = c;
  byt->next = NULL;
  return byt;
}

void printList(Node* list) {
  Node* curr = list;

  while (curr != NULL) {
    printf("'%s' ", curr->info);
    curr = curr->next;
  }

  printf("\n");
}

int isWhitespace(char* c) { return (*c == ' ' || *c == '\n' || *c == '\t'); }

int lenList(Node* head) {
  Node* curr = head;
  int len = 0;
  while (curr != NULL) {
    len++;
    curr = curr->next;
  }
  return len;
}

char* listToString(Node* head) {
  int len = lenList(head);
  char* string = malloc((sizeof(char) * len) + 1);
  Node* curr = head;
  int ind = 0;

  while (curr != NULL) {
    string[ind] = *(curr->info);
    curr = curr->next;
    ind++;
  }

  string[ind] = '\0';
  return string;
}

struct Node* fileRead(int fd) {
  int byteCount = 0;
  Node* head = NULL;
  Node* curr = NULL;
  Node* miniHead = NULL;
  Node* miniCurr = NULL;
  do {
    char* byt = malloc(sizeof(char));
    byteCount = read(fd, byt, 1);
    if (isalpha(*byt) || isdigit(*byt) || *byt == '-') {
      Node* charNode = makeNode(byt);
      if (miniHead == NULL) {
        miniHead = charNode;
        miniCurr = charNode;
      } else {
        miniCurr->next = charNode;
        miniCurr = miniCurr->next;
      }
    }
    if (*byt == ',') {
      char* string = listToString(miniHead);
      Node* charNode = makeNode(string);
      if (head == NULL) {
        head = charNode;
        curr = charNode;
      } else {
        curr->next = charNode;
        curr = curr->next;
      }
      miniHead = NULL;
      miniCurr = NULL;
    }
  } while (byteCount > 0);

  if (miniHead != NULL) {  // Don't add empty token if it's at the end.
    char* string = listToString(miniHead);
    Node* charNode = makeNode(string);
    if (head == NULL) {
      head = charNode;
      curr = charNode;
    } else {
      curr->next = charNode;
      curr = curr->next;
    }
  }

  return head;
}

int underTheHood(char* str1, char* str2) {
  if (str1 == NULL && str2 == NULL) {
    return 0;
  }
  if (str1 == NULL) {
    return *str2;
  }
  if (str2 == NULL) {
    return *str1;
  }

  while (*str1 != '\0' && *str2 != '\0') {
    int diff = *str1 - *str2;
    if (diff != 0) {
      return diff;
    }
    str1++;
    str2++;
  }

  if (*str1 == '\0' && *str2 == '\0') {
    return 0;
  }
  if (*str1 == '\0') {
    return -(*str2);
  }
  if (*str2 == '\0') {
    return *str1;
  }

  return 0;
}

int stringCompare(void* str1, void* str2) {
  Node* s1 = str1;
  Node* s2 = str2;

  return underTheHood(s1->info, s2->info);
}

Node* appendToList(Node* head, Node* new) {
  if (head == NULL && new == NULL) {
    return 0;
  }
  if (head == NULL) {
    return new;
  } else if (new == NULL) {
    return head;
  } else {
    Node* curr = head;
    Node* prev = NULL;
    while (curr != NULL) {
      prev = curr;
      curr = curr->next;
    }
    prev->next = new;
  }
  return head;
}

int insertionSort(void* toSort, int (*comparator)(void*, void*)) {
  Node* dummyHead = toSort;
  Node* curr = dummyHead->next;
  Node* prev = NULL;

  while (curr != NULL) {
    Node* ptr = dummyHead->next;
    Node* ptrPrev = NULL;

    while (ptr != curr) {
      if (comparator(curr, ptr) < 0) {
        if (ptrPrev == NULL) {
          prev->next = curr->next;
          curr->next = dummyHead->next;
          dummyHead->next = curr;
        } else {
          prev->next = curr->next;
          curr->next = ptr;
          ptrPrev->next = curr;
        }

        break;
      }

      ptrPrev = ptr;
      ptr = ptr->next;
    }

    prev = curr;
    curr = curr->next;
  }

  return 0;
}

int quickSort(void* toSort, int (*comparator)(void*, void*)) {
  Node* head = toSort;

  Node* pivot = head->next;

  Node* less = NULL;
  Node* greater = NULL;
  Node* prev = NULL;

  if (pivot == NULL || pivot->next == NULL) {
    return 0;
  }

  Node* curr = pivot->next;
  pivot->next = NULL;

  while (curr != NULL) {
    Node* next = curr->next;
    curr->next = NULL;

    if (comparator(pivot, curr) > 0) {
      less = appendToList(less, curr);
    } else {
      greater = appendToList(greater, curr);
    }

    curr = next;
  }

  Node* dummyNode1 = malloc(sizeof(Node));
  dummyNode1->next = less;

  Node* dummyNode2 = malloc(sizeof(Node));
  dummyNode2->next = greater;

  quickSort(dummyNode1, comparator);
  quickSort(dummyNode2, comparator);
  head->next =
      appendToList(appendToList(dummyNode1->next, pivot), dummyNode2->next);
  return 0;
}

int int_comparator(void* a, void* b) {
  Node* a2 = a;
  Node* b2 = b;
  return atoi(a2->info) - atoi(b2->info);
}

int listIsString(Node* head) {
  if (head == NULL) {
    return 1;
  }
  Node* curr = head;
  if (*(curr->info) == '\0') {
    return listIsString(curr->next);
  }
  return atoi(curr->info) == 0 &&
         *(curr->info) != '0';  // return 1 if string, 0 if not string
}

int main(int argc, char** argv) {
  int fd = open(argv[2], O_RDONLY | O_CREAT);

  Node* list = fileRead(fd);
  int isString = listIsString(list);

  Node* dummyNode = malloc(sizeof(Node));
  dummyNode->next = list;

  int (*cmpr)(void*, void*) = isString ? stringCompare : int_comparator;

  if (underTheHood(argv[1], "-i") == 0) {
    insertionSort(dummyNode, cmpr);
  }

  if (underTheHood(argv[1], "-q") == 0) {
    quickSort(dummyNode, cmpr);
  }

  printList(dummyNode->next);
  return 0;
}
