#pragma once
#include <cstdlib>

class MemoryPool {
public:
	void* firstBlock;
	void* nowBlock;
	void* nextItem;
	void* deadItemStack;
	void* pathBlock;
	void* pathItem;
	int alignBytes;
	int itemBytes, itemWords;
	int itemsPerBlock;
	long items, maxItems;
	int unallocatedItems;
	int pathItemsLeft;

	MemoryPool() {
		firstBlock = nowBlock = nullptr;
		nextItem = nullptr;
		deadItemStack = nullptr;
		pathBlock = nullptr;
		pathItem = nullptr;
		alignBytes = 0;
		itemBytes = itemWords = 0;
		itemsPerBlock = 0;
		items = maxItems = 0l;
		unallocatedItems = 0;
		pathItemsLeft = 0;
	}

	MemoryPool(int byteCount, int itemCount, int wordSize, int alignment) {
		initializePool(byteCount, itemCount, wordSize, alignment);
	}

	~MemoryPool() {
		while (*(void**)firstBlock != nullptr) {
			nowBlock = *(void**)firstBlock;
			free(firstBlock);
			firstBlock = nowBlock;
		}
		free(firstBlock);
	}

	void initializePool(int byteCount, int itemCount, int wordSize, int alignment) {


		if (alignment > wordSize) {
			alignBytes = alignment;
		}
		else {
			alignBytes = wordSize;
		}

		if (sizeof(void*) > alignBytes) {
			alignBytes = sizeof(void*);
		}

		itemWords = ((byteCount + alignBytes - 1) / alignBytes) * (alignBytes / wordSize);
		itemBytes = itemWords * wordSize;
		itemsPerBlock = itemCount;

		firstBlock = malloc(itemsPerBlock * itemBytes + sizeof(void*) + alignBytes);
		if (firstBlock == nullptr) {
			return; // Out of memory
		}

		*(void**)firstBlock = nullptr;
		restart();
	}

	void restart() {
		uintptr_t alignPtr;

		items = 0;
		maxItems = 0;

		nowBlock = firstBlock;
		alignPtr = (uintptr_t)((void**)nowBlock + 1);
		nextItem = (void*)(alignPtr + (uintptr_t)alignBytes - (alignPtr % (uintptr_t)alignBytes));
		unallocatedItems = itemsPerBlock;
		deadItemStack = nullptr;
	}

	void* allocate() {
		//============================================================================//
		//                                                                            //
		// poolinit()    Initialize a pool of memory for allocation of items.         //
		//                                                                            //
		// A `pool' is created whose records have size at least `bytecount'.  Items   //
		// will be allocated in `itemcount'-item blocks.  Each item is assumed to be  //
		// a collection of words, and either pointers or floating-point values are    //
		// assumed to be the "primary" word type.  (The "primary" word type is used   //
		// to determine alignment of items.)  If `alignment' isn't zero, all items    //
		// will be `alignment'-byte aligned in memory.  `alignment' must be either a  //
		// multiple or a factor of the primary word size;  powers of two are safe.    //
		// `alignment' is normally used to create a few unused bits at the bottom of  //
		// each item's pointer, in which information may be stored.                   //
		//                                                                            //
		//============================================================================//

		// Find the proper alignment, which must be at least as large as:
		//   - The parameter `alignment'.
		//   - The primary word type, to avoid unaligned accesses.
		//   - sizeof(void *), so the stack of dead items can be maintained
		//       without unaligned accesses.

		void* newItem;
		void* newBlock;
		uintptr_t alignPtr;

		if (deadItemStack != nullptr) {
			newItem = deadItemStack;
			deadItemStack = *(void**)deadItemStack;
		}
		else {
			if (unallocatedItems == 0) {
				if (*(void**)nowBlock == nullptr) {
					newBlock = malloc(itemsPerBlock * itemBytes + sizeof(void*) + alignBytes);
					if (newBlock == nullptr) {
						return nullptr; // Out of memory
					}
					*(void**)nowBlock = newBlock;
					*(void**)newBlock = nullptr;
				}
				nowBlock = *(void**)nowBlock;
				alignPtr = (uintptr_t)((void**)nowBlock + 1);
				nextItem = (void*)(alignPtr + (uintptr_t)alignBytes - (alignPtr % (uintptr_t)alignBytes));
				unallocatedItems = itemsPerBlock;
			}
			newItem = nextItem;
			nextItem = (void*)((uintptr_t)nextItem + itemBytes);
			unallocatedItems--;
			maxItems++;
		}
		items++;
		return newItem;
	}

	void deallocate(void* dyingItem) {
		*((void**)dyingItem) = deadItemStack;
		deadItemStack = dyingItem;
		items--;
	}

	void traversalInit() {
		//============================================================================//
		//                                                                            //
		// traversalinit()   Prepare to traverse the entire list of items.            //
		//                                                                            //
		// This routine is used in conjunction with traverse().                       //
		//                                                                            //
		//============================================================================//
		uintptr_t alignPtr;
		pathBlock = firstBlock;
		alignPtr = (uintptr_t)((void**)pathBlock + 1);
		pathItem = (void*)(alignPtr + (uintptr_t)alignBytes - (alignPtr % (uintptr_t)alignBytes));
		pathItemsLeft = itemsPerBlock;
	}

	void* traverse() {
		//============================================================================//
		//                                                                            //
		// traverse()   Find the next item in the list.                               //
		//                                                                            //
		// This routine is used in conjunction with traversalinit().  Be forewarned   //
		// that this routine successively returns all items in the list, including    //
		// deallocated ones on the deaditemqueue. It's up to you to figure out which  //
		// ones are actually dead.  It can usually be done more space-efficiently by  //
		// a routine that knows something about the structure of the item.            //
		//                                                                            //
		//============================================================================//

		void* newItem;
		uintptr_t alignPtr;

		if (pathItem == nextItem) {
			return nullptr;
		}

		if (pathItemsLeft == 0) {
			pathBlock = *(void**)pathBlock;
			alignPtr = (uintptr_t)((void**)pathBlock + 1);
			pathItem = (void*)(alignPtr + (uintptr_t)alignBytes - (alignPtr % (uintptr_t)alignBytes));
			pathItemsLeft = itemsPerBlock;
		}
		newItem = pathItem;
		pathItem = (void*)((uintptr_t)pathItem + itemBytes);
		pathItemsLeft--;
		return newItem;
	}
};