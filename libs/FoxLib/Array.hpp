/*Convenient array types.*/
//Simple array with no memory management
template<typename elemType>
struct array {
	elemType* b;
	fuint len;

	array<elemType>& initEmpty() {
		this->len = 0;
		return *this;
	}

	elemType& at(fuint indx) const {
		assert(indx < this->len);
		return this->b[indx];
	}

	elemType& atUnbounded(fuint indx) const {
		return this->b[indx];
	}

	elemType& operator[](fuint indx) const {
		return this->at(indx);
	}

	//Absolute memory comparison
	bool memEquals(const array<elemType>& other) const {
		if (this->len == other.len) {
			return std::memcmp(this->b, other.b, this->len*sizeof(elemType)) == 0;
		}
		return false;
	}

	//Comparison with != operator
	bool equals(const array<elemType>& other) const {
		if (this->len == other.len) {
			fox_for (indxElem, this->len) {
				if (this->at(indxElem) != other[indxElem]) {
					return false;
				}
			}
			return true;
		}
		return false;
	}

	bool isEmpty() const {
		return this->len == 0;
	}
	
	array<elemType> subArray(fuint indxStart, fuint indxEndExcl) const {
		array<elemType> sub;
		sub.b = this->b + indxStart;
		sub.len = indxEndExcl - indxStart;
		return sub;
	}
		
	array<elemType> subArray(fuint indxEndExcl) const {
		return this->subArray(0, indxEndExcl);
	}

	//---Built in operators
	bool operator==(const array<elemType>& other) const {
		return this->equals(other);
	}
	bool operator!=(const array<elemType>& other) const {
		return !this->equals(other);
	}
};
