//Abstract data types which use foxlib allocators
template<typename elemType>
struct stackArray : public array<elemType> {
	fuint allocSize;
	stackAllocator* memSrc;
	
	stackAllocator::tempAllocSave initEmpty() {
		assert(memSrc);
		this->b = (elemType*)memSrc->point;
		this->len = 0;
		this->allocSize = 0;
		return memSrc->startTempAlloc();
	}

	void expand(fuint count) {
		this->len += count;
		auto additionalSize = sizeof(elemType) * count;
		this->allocSize += additionalSize;
		if (memSrc->canAllocOnCurBlock(additionalSize)) {
			//Only allocate or one new one
			//Note we are relying on noone making allocations in the middle of this array!
			memSrc->allocOnCurBlock(additionalSize);
		} else {
			//TODO: 
			assert(memSrc->blockSize > this->allocSize);

			//We must move array
			auto* newB = (elemType*)memSrc->allocOnNewBlock(this->allocSize);
			memcpy(newB, this->b, this->allocSize);
			this->b = newB;
			//Waste old memory
		}
	}
	
	elemType& pushBack(elemType e) {
		this->expand(1);
		return (this->b[this->len - 1] = e);
	}

	void pushArr(array<elemType> toAdd) {
		auto* addPoint = &this->b[this->len];
		this->expand(toAdd.len);
		fox_for (indxElem, toAdd.len) {
			addPoint[indxElem] = toAdd[indxElem];
		}
	}
		
	array<elemType> toStatic() const {
		return array<elemType> { this->b, this->len };
	}
};

struct stackStr : public stackArray<char> {
	void writeChar(char c) {
		this->pushBack(c);
	}
	void writeStr(constStr s) {
		auto oldLen = this->len;
		this->expand(s.len);
		char* bWritePoint = this->b + oldLen; //We have to calc this after resize because buffer may move
		memcpy(bWritePoint, s.b, s.len);
	}
	void writeIndent(fuint countTabChars) {
		for (fuint i = 0; i < countTabChars; i++) {
			this->writeStr("  ");
		}
	}
	void writeInt(int64 n) {
		char nBuf[foxlib::maxInt64StrLen];
		this->writeStr(str().fromInt(n, nBuf).constify());
	}
	//TODO: rename this to be consistent plsplspls
	void writeUInt(uint64 n) {
		char nBuf[foxlib::maxUint64StrLen];
		this->writeStr(str().fromUint(n, nBuf).constify());
	}
	void writeFloat(float32 n) {
		char nBuf[foxlib::maxFloat32StrLen];
		this->writeStr(str().fromFloat(n, nBuf).constify());
	}

	str toStr() const {
		str r;
		r.b = this->b;
		r.len = this->len;
		return r;
	}
	
	constStr toConstStr() const {
		return this->toStr().constify();
	}
};

template<typename elemType>
struct linkedArray {
	allocator* memSrc;
	struct arrayLink {
		arrayLink* next;
		fuint len;
		elemType elems; //&elems is the buffer for the actual elements
	} *headLink, *tailLink;

	fuint linkCapacity;
	fuint linkSize;
	fuint len; //Total length

	void init() {
		assert(this->linkCapacity);
		assert(this->memSrc);
		this->linkSize = this->linkCapacity * sizeof(elemType)
			           + sizeof(arrayLink) - sizeof(elemType);
		headLink = tailLink = this->makeLink();
		this->len = 0;
	}

	arrayLink* makeLink() {
		auto block = memSrc->allocBlock(this->linkSize);
		assert(block.len >= this->linkSize);
		auto& newLink = *(arrayLink*)block.b;
		newLink.next = nullptr;
		newLink.len = 0;
		return &newLink;
	}
		
	void expand(fuint count) {
		assert(this->headLink);
		assert(this->tailLink);
		this->len += count;

		auto remaining = this->linkCapacity - tailLink->len;

		//Allocate new links if we don't have enough space
		while (count > remaining) {
			tailLink->len += remaining;
			count -= remaining;
			auto* oldTailLink = this->tailLink;
			this->tailLink = this->makeLink();
			oldTailLink->next = this->tailLink;
			remaining = this->linkCapacity;
		}
		
		tailLink->len += count;
	}

	elemType& at(fuint indx) const {
		assert(indx < this->len);
		assert(this->headLink);
		
		auto* curLink = this->headLink;
		while (indx >= curLink->len) {
			indx -= curLink->len;
			curLink = curLink->next;
			assert(curLink);
		}
		return (&curLink->elems)[indx];
	}
	
	elemType& operator[](fuint indx) const {
		return this->at(indx);
	}

	elemType& pushBack(elemType newElem) {
		auto newIndx = this->len;
		this->expand(1);
		return this->at(newIndx) = newElem;
	}
		
	array<elemType> toStatic() const {
		auto contigBlock = memSrc->allocBlock(this->len * sizeof(elemType));
		array<elemType> asStatic;
		asStatic.b = (elemType*)contigBlock.b;
		asStatic.len = this->len;
		fox_for (indx, this->len) {
			asStatic[indx] = this->at(indx);
		}
		return asStatic;
	}
};

void testArrays() {
	//Test string from number
	char iBuf[foxlib::maxInt64StrLen];
	char fBuf[foxlib::maxFloat32StrLen];
	str numStr;
	numStr.fromInt(-1000000000000000000L, iBuf);
	assert(numStr == str().fromCStr("-1000000000000000000"));
	numStr.fromFloat(-2.5f, fBuf);
	assert(numStr == str().fromCStr("-2.5"));

	mallocAllocator mallocator;
	mallocator.init();
	linkedArray<fuint> testla; //The new model testla is all the rage
	testla.memSrc = &mallocator;
	testla.linkCapacity = 2;
	testla.init();
	testla.pushBack(0xdeadbeef);
	testla.pushBack(123456);
	testla.pushBack(0xceedbaf);
	testla.pushBack(8);
	testla[3] = 7;
	assert(testla[0] == 0xdeadbeef);
	assert(testla[1] == 123456);
	assert(testla[2] == 0xceedbaf);
	assert(testla[3] == 7);
}
