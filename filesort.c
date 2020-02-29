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

void warning(char* warn_str) { printf("[Warning] %s\n", warn_str); }

void error(char* err_str) { printf("[Error] %s\n", err_str); }

void fatalError(char* err_str) {
  printf("[Fatal Error] %s\n", err_str);
  exit(1);
}

void freeList(Node* list) {
  if (list == NULL) {
    return;
  }

  Node* next = list->next;

  free(list->info);
  free(list);

  freeList(next);
}

void* mallocWithErrors(size_t size) {
  void* ptr = malloc(size);
  if (ptr == NULL) {
    error("Malloc returned null.");
  }
  return ptr;
}

Node* makeNode(char* c) {
  Node* byt = mallocWithErrors(sizeof(Node));
  byt->info = c;
  byt->next = NULL;
  return byt;
}

void printList(Node* list) {
  Node* curr = list;

  while (curr != NULL) {
    printf("%s\n", curr->info);
    curr = curr->next;
  }
}

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
  char* string = mallocWithErrors((sizeof(char) * len) + 1);
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
    char* byt = mallocWithErrors(sizeof(char));
    byteCount = read(fd, byt, 1);

    if (byteCount < 0) {
      error("Could not read bytes.");
    }

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
      // Don't add lone dashes.
      int is_lone_dash = 0;

      if (miniHead != NULL) {
        // If it's one char long and that char is a dash.
        is_lone_dash = miniHead->next == NULL && *(miniHead->info) == '-';
      }

      if (!is_lone_dash) {
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

      freeList(miniHead);

      miniHead = NULL;
      miniCurr = NULL;
    }
  } while (byteCount > 0);

  // Don't add lone dashes.
  int is_lone_dash = 0;

  if (miniHead != NULL) {
    // If it's one char long and that char is a dash.
    is_lone_dash = miniHead->next == NULL && *(miniHead->info) == '-';
  }

  // Don't add empty tokens or lone dashes at the end.
  if (miniHead != NULL && !is_lone_dash) {
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

  freeList(miniHead);

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

  return *str1 - *str2;
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

  Node* dummyNode1 = mallocWithErrors(sizeof(Node));
  dummyNode1->next = less;

  Node* dummyNode2 = mallocWithErrors(sizeof(Node));
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
  if (argc < 2) {
    fatalError("Arguments expected.");
  }

  if (underTheHood(argv[1], "-i") != 0 && underTheHood(argv[1], "-q") != 0) {
    fatalError("No sorting algorithm specified.");
  }

  int fd = open(argv[2], O_RDONLY);

  if (fd < 0) {
    fatalError("Invalid file.");
  }

  Node* list = fileRead(fd);
  int isString = listIsString(list);

  Node* dummyNode = mallocWithErrors(sizeof(Node));
  dummyNode->next = list;

  int (*cmpr)(void*, void*) = isString ? stringCompare : int_comparator;

  if (underTheHood(argv[1], "-i") == 0) {
    insertionSort(dummyNode, cmpr);
  }

  if (underTheHood(argv[1], "-q") == 0) {
    quickSort(dummyNode, cmpr);
  }

  if (dummyNode->next == NULL) {
    warning("File is empty.");
  }

  printList(dummyNode->next);
  freeList(dummyNode);

  close(fd);
  return 0;
}
