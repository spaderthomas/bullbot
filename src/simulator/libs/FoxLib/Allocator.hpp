//Portable memory allocators that can be stacked with each other
struct allocator {
	virtual array<uint8> allocBlock(fuint minSize) { array<uint8> dummy; fox_unimplemented(dummy); }
	virtual void freeBlock(array<uint8> block) fox_unimplemented();
};

struct mallocAllocator : public allocator {
	void init() {
		new(this) mallocAllocator;
	}
	
	array<uint8> allocBlock(fuint minSize) override {
		assert(minSize && "allocation must be positive");
		
		array<uint8> block = {(uint8*)malloc(minSize), minSize};
		assert(block.b);
		memset(block.b, 0, (size_t)minSize);
		return block;
	}

	void freeBlock(array<uint8> block) override {
		free(block.b);
	}
};

#ifdef _WIN32
//Allocates via windows VirtualAlloc
struct virtualAllocator : public allocator {
    void init() {
        new(this) virtualAllocator;
    }

    array<uint8> allocBlock(fuint minSize) override {
		assert(minSize && "allocation must be positive");	    
	    
        auto* buf = VirtualAlloc(nullptr, (SIZE_T)minSize, MEM_COMMIT, PAGE_READWRITE);
        assert(buf);
        array<uint8> block = {(uint8*)buf, minSize};
        return block;
    }

    void freeBlock(array<uint8> block) override {
        auto success = VirtualFree(block.b, (SIZE_T)block.len, MEM_RELEASE);
        assert(success);
    }
};
#endif

struct stackAllocator : public allocator {
	fuint blockSize;
	struct stackBlock {
		stackBlock* next;
		fuint size;     //Size of memory including this header
		uint8 memStart; //&memStart is where allocatable memory starts
	} *head, *tail;
	allocator* memSrc;

	struct tempAllocSave {
		stackBlock* oldTail;
		uint8* oldPoint;
	};
	
	uint8* point; //Next free spot
	
	void init() {
		assert(blockSize);
		assert(blockSize >= sizeof(stackBlock));

		new(this) stackAllocator;
		this->head = this->tail = this->allocStackBlock();
		this->point = &this->tail->memStart;
	}

	//TODO: destroy, reset
	
	//Allocation functions
	//Allocation from parent allocator
	stackBlock* allocStackBlock() {
		auto newBlock = memSrc->allocBlock(this->blockSize);
		auto* newStackBlock = (stackBlock*)newBlock.b;
		newStackBlock->next = nullptr;
		newStackBlock->size = newBlock.len;
		return newStackBlock;
	}

	array<uint8> allocBlock(fuint minSize) override {
		assert(minSize && "allocation must be positive");
		assert(minSize <= this->blockSize
		       && "allocation too large");
		array<uint8> memBlock;
		memBlock.len = minSize;
		if (this->canAllocOnCurBlock(minSize)) {
			memBlock.b = this->allocOnCurBlock(minSize);
		} else {
			memBlock.b = this->allocOnNewBlock(minSize);
		}

		memset(memBlock.b, 0, minSize);
		return memBlock;
	}

	bool canAllocOnCurBlock(fuint size) {
		uint8* end = ((uint8*)this->tail) + this->tail->size;
		return (fuint)(end - this->point) >= size;
	}

	uint8* allocOnCurBlock(fuint size) {
		assert(this->canAllocOnCurBlock(size)
		       && size > 0
		       && "invalid allocation size");
		auto* allocedMem = this->point;
		this->point += size;
		return allocedMem;
	}

	uint8* allocOnNewBlock(fuint size) {
		//See if we already have a next block
		if (!this->tail->next) {
			this->tail->next = this->allocStackBlock();
		}
		
		this->tail = this->tail->next;
		this->point = &this->tail->memStart;
		return this->allocOnCurBlock(size);
	}

	//Temporary allocation
	//TODO: less misleading names, this only creates a restore point
	tempAllocSave startTempAlloc() {
		return tempAllocSave { this->tail, this->point };
	}

	void endTempAlloc(tempAllocSave s) {
		this->tail = s.oldTail;
		this->point = s.oldPoint;
	}
	
	//TODO: implement a free for large blocks which adds those blocks to the end of the stack
	
	//Convenience functions
	template<typename t>
	t* alloc(fuint count = 1) {
		auto block = this->allocBlock(sizeof(t) * count);
		return (t*)block.b;
	}

	template<typename t>
	array<t> allocArray(fuint count) {
		array<t> ret;
		ret.len = count;
		ret.b = count ? this->alloc<t>(ret.len) : nullptr;
		return ret;
	}

	str allocStr(fuint count) {
		str ret;
		ret.len = count;
		ret.b = count ? this->alloc<char>(ret.len) : nullptr;
		return ret;
	}
};

struct poolAllocator : public allocator {
	fuint allocSize;
	fuint blockSize;
	allocator* memSrc;
	struct poolBlock {
		fuint countBlocks; //Number of blocks this stretch of free memory contains
		poolBlock* nextFree;
	} *firstFree;

	void init() {
		assert(allocSize);
		assert(allocSize >= blockSize);
		assert(allocSize % blockSize == 0);
		assert(blockSize >= sizeof(poolBlock));
		
		this->allocPoolBlock();
	}

	void allocPoolBlock() {
		auto block = this->memSrc->allocBlock(this->allocSize);
		this->firstFree = (poolBlock*)block.b;
		this->firstFree->countBlocks = block.len / this->blockSize;
		this->firstFree->nextFree = nullptr;
	}

	array<uint8> allocBlock(fuint minSize) override {
		if (this->blockSize >= minSize) {
			if (!this->firstFree) {
				this->allocPoolBlock();
			}
			
			//Save block data
			auto countBlocks = this->firstFree->countBlocks;
			auto* nextFree = this->firstFree->nextFree;

			//Get the block as usable memory
			array<uint8> block;
			block.len = this->blockSize;
			block.b = (uint8*)this->firstFree;
			assert(countBlocks);
			countBlocks--;
			if (countBlocks) {
				this->firstFree = (poolBlock*)(block.b + blockSize);
				this->firstFree->countBlocks = countBlocks;
				this->firstFree->nextFree = nextFree;
			} else {
				this->firstFree = nextFree;
			}

			memset(block.b, 0, block.len);
			return block;
		} else {
			//Fallback to parent allocator
			return this->memSrc->allocBlock(minSize);
		}
	}
	
	void freeBlock(array<uint8> block) override {
		assert(block.b);
		if (block.len <= this->blockSize) {
			auto* oldFirstFree = this->firstFree;
			this->firstFree = (poolBlock*)block.b;
			this->firstFree->countBlocks = 1;
			this->firstFree->nextFree = oldFirstFree;
		} else {
			this->memSrc->freeBlock(block);
		}
	}
};

poolAllocator& createPoolAllocatorStack(allocator& memSrc, fuint headAllocerBlockSize, array<fuint> countBlocks) {
	assert(countBlocks.len >= 2);

	//Allocate our block of memory for both our allocators and the memory they use
	auto allocerMemSize = sizeof(poolAllocator) * countBlocks.len;
	auto headAllocerAllocSize = headAllocerBlockSize * countBlocks[0];
	auto memSize = headAllocerAllocSize + allocerMemSize;
	auto memBlock = memSrc.allocBlock(memSize);
	assert(memBlock.len >= memSize);

	//Split the memory into allocators and buffers
	poolAllocator* allocers = (poolAllocator*)memBlock.b;
	uint8* memPtr = memBlock.b + allocerMemSize;

	//Create parentmost allocator
	poolAllocator* parent = &allocers[0];
	new(parent) poolAllocator;
	parent->blockSize = headAllocerBlockSize;
	parent->allocSize = headAllocerAllocSize;
	parent->memSrc = &memSrc;
	parent->firstFree = (poolAllocator::poolBlock*)memPtr;
	parent->firstFree->countBlocks = countBlocks[0];
	parent->firstFree->nextFree = nullptr;

	//Create child allocators
	for (fuint indxAllocer = 1; indxAllocer < countBlocks.len; indxAllocer++) {
		poolAllocator* child = &allocers[indxAllocer];
		new(child) poolAllocator;
		child->allocSize = parent->blockSize;
		child->blockSize = parent->blockSize / countBlocks[indxAllocer];
		child->memSrc = parent;
		child->init();
		parent = child;
	}
	
	return *parent; //Return childmost allocator ... I call it ironic programming
}

void testAllocators() {
	mallocAllocator testTLA;
	fuint countBlocks[] = {3, 4, 2};
	array<fuint> realCountBlocks;
	realCountBlocks.b = countBlocks;
	realCountBlocks.len = 3;
	auto& testBlockStack = createPoolAllocatorStack(testTLA, 128, realCountBlocks);
	auto& smallAllocer = testBlockStack;
	auto& mediumAllocer = *(poolAllocator*)smallAllocer.memSrc;
	auto& largeAllocer = *(poolAllocator*)mediumAllocer.memSrc;
	
	auto largeBlock = testBlockStack.allocBlock(128);
	auto mediumBlock = testBlockStack.allocBlock(17);
	auto smallBlock = testBlockStack.allocBlock(1);

	assert(mediumAllocer.blockSize == 32);
	assert(smallAllocer.blockSize == 16);
	assert(largeBlock.len == 128);
	assert(mediumBlock.len == 32);
	assert(smallBlock.len == 16);
	assert(largeAllocer.firstFree->countBlocks == 1);
	assert(mediumAllocer.firstFree->countBlocks == 2);
	assert(smallAllocer.firstFree->countBlocks == 1);
	
	testBlockStack.freeBlock(largeBlock);
	testBlockStack.freeBlock(mediumBlock);
	testBlockStack.freeBlock(smallBlock);
	assert(largeAllocer.firstFree->countBlocks == 1);
	assert(mediumAllocer.firstFree->countBlocks == 1);
	assert(smallAllocer.firstFree->countBlocks == 1);
	fox_for (_, 3) {
		largeBlock = testBlockStack.allocBlock(128);
	}
	assert(largeAllocer.firstFree->countBlocks == 2);
}
