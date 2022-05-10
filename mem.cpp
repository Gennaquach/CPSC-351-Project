#include <sys/types.h> 
#include <bits/stdc++.h>

using namespace std;  

class IMemoryManager 
  {
  public:
    virtual void* allocate(size_t) = 0;
    virtual void  free(void*) = 0;
  };

class MemoryManager: public IMemoryManager 
  { 
  struct FreeStore
    {
     FreeStore *next;
    }; 
  void expandPoolSize ();
  void cleanUp ();
  FreeStore* freeStoreHead;
  public:
    MemoryManager () { 
      freeStoreHead = 0;
      expandPoolSize ();
      }
    virtual ~MemoryManager () { 
      cleanUp ();
      }
    virtual void* allocate(size_t);
    virtual void   free(void*);
  };

MemoryManager gMemoryManager;

class Complex 
  {
  public:
    Complex (double a, double b): r (a), c (b) {}
    inline void* operator new(size_t);
    inline void   operator delete(void*);
  private:
    double r; // Real Part
    double c; // Complex Part
  };

inline void* MemoryManager::allocate(size_t size)
  {
  if (0 == freeStoreHead)
    expandPoolSize ();

  FreeStore* head = freeStoreHead;
  freeStoreHead = head->next;
  return head;
  }

inline void MemoryManager::free(void* deleted)
  {
  FreeStore* head = static_cast <FreeStore*> (deleted);
  head->next = freeStoreHead;
  freeStoreHead = head;
  }

void* Complex::operator new (size_t size) 
  {
  return gMemoryManager.allocate(size);
  }

void Complex::operator delete (void* pointerToDelete)
  {
  gMemoryManager.free(pointerToDelete);
  }


  #define POOLSIZE 32

void MemoryManager::expandPoolSize ()
  {
  size_t size = (sizeof(Complex) > sizeof(FreeStore*)) ?
    sizeof(Complex) : sizeof(FreeStore*);
  FreeStore* head = reinterpret_cast <FreeStore*> (new char[size]);
  freeStoreHead = head;

  for (int i = 0; i < POOLSIZE; i++) {
    head->next = reinterpret_cast <FreeStore*> (new char [size]);
    head = head->next;
    }

  head->next = 0;
  }

void MemoryManager::cleanUp()
  {
  FreeStore* nextPtr = freeStoreHead;
  for (; nextPtr; nextPtr = freeStoreHead) {
    freeStoreHead = freeStoreHead->next;
    delete [] nextPtr; // remember this was a char array
    }
  }

int main(int argc, char* argv[]) 
  {
  Complex* array[1000];
  for (int i = 0;i  <  5000; i++) {
    for (int j = 0; j  <  1000; j++) {
      array[j] = new Complex (i, j);
      }
    for (int j = 0; j  <  1000; j++) {
      delete array[j];
      }
    }
  return 0;
  }
