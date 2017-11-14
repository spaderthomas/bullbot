/*String classes and operations*/	
//Constants indicating maximum len of a string representing the value of a specific numeric type
const fuint maxInt64StrLen = 20; //-9,223,372,036,854,775,808
const fuint maxUint64StrLen = 20; //18,446,744,073,709,551,615
const fuint maxFloat32StrLen = 41; //-1e-38

/*Converts 64 bit unsigned number to a string
  outBuffer must be at least maxUint64StrLen in len
  number will be written into outBuffer from the back to the front
  writing stops when number is parsed so front is garbage
  pointer to front of number string returned*/
char* strFromUint(uint64 n, char* outBuffer) {
	if (n) {
		fuint indxDigit = maxUint64StrLen;
		while (n) {
			fuint digit = n % 10;
			--indxDigit;
			assert(indxDigit >= 0);
			outBuffer[indxDigit] = (char)(digit + '0');
			n /= 10;
		}
		return &outBuffer[indxDigit];
	}
	outBuffer[maxUint64StrLen - 1] = '0';
	return &outBuffer[maxUint64StrLen - 1];
}

/*Does the above but prints '-' as necessary
  outBuffer must be at least maxInt64StrLen in len*/
char* strFromInt(int64 n, char* outBuffer) {
	if (n < 0) {
		n = -n;
		char* bufferFront = strFromUint((uint64)n, outBuffer);
		bufferFront--;
		assert(bufferFront >= outBuffer);
		bufferFront[0] = '-';
		return bufferFront;
	}
	return strFromUint((uint64)n, outBuffer);
}

/*Converts double precision float in [0,1) to string
  outBuffer must be big enough to contain the converted string
  the leading 0. will not be written
  the buffer is written back to front with the lenght of whats written returned.*/
fuint strFromFrac(float32 n, char* outBuffer) {
	assert(0.0f <= n && n < 1.0f);
	if (n > 0.0f) {
		fuint indxDigit = 0;
		while (n > 0.0f) {
			assert(indxDigit < maxFloat32StrLen);
			n *= 10.0f;                          //Promote first fractional digit to integer part
			float32 digit = std::fmod(n, 10.0f); //digit = n % 10
			n -= digit;                          //Remove integer part to get back to fraction
			outBuffer[indxDigit++] = (char)((uint32)digit + '0');
		}
		return indxDigit;
	}
	outBuffer[0] = '0';
	return 1;
}

//TODO: write one for definitely positive floats that this calls.
/*Converts single precision float to string
  outBuffer must be big enough to contain the converted string
  a leading 0. is printed for floating points less than 1
  a float of 0 will be converted as 0.0
  the length of the converted string is returned*/
fuint strFromFloat(float32 n, char* outBuffer) {
	fuint signLen = 0;
	//Split into integer and fractional [0,1) parts
	if (n < 0.0f) {
		n = -n;
		outBuffer[0] = '-';
		outBuffer++;
		signLen = 1;
	}

	auto intPart = (uint64)n;
	auto fracPart = n - intPart;

	//Convert each part into string
	char* numStart = strFromUint(intPart, outBuffer);
	fuint intLen = &outBuffer[maxUint64StrLen] - numStart;
	//Move number to beginning of buffer
	std::memcpy(outBuffer, numStart, intLen);
	
	auto* fracBuffer = outBuffer + intLen;
	fracBuffer[0] = '.';
	fracBuffer++;
	const fuint pointLen = 1;
	
	fuint fracLen = strFromFrac(fracPart, fracBuffer);
	
	return signLen + intLen + pointLen + fracLen;
}


//Convert base base number to returned fuint (only for unsigned and base <= 10
fuint numFromStr(const char* inBuffer, fuint len, float32 base) {
	assert(base <= 10);
	fuint r = 0;
	auto lastIndx = len - 1;
	for (fuint i = lastIndx; i >= 0; i--) {
		r += (inBuffer[i] - '0') * (fuint)std::pow(base, (float32)(lastIndx - i));
	}
	return r;
}

//String classes
struct constStr : public array<const char> {
	constStr() {}
	constStr(const char* c) {
		this->fromCStr(c);
	}
	
    constStr& initEmpty() {
        this->len = 0;
        return *this;
    }
	
	constStr& fromCStr(const char* c) {
		assert(c);
		this->b = c;
		this->len = strlen(c);
		return *this;
	}

	constStr& fromArray(array<const char> a) {
		this->b = a.b;
		this->len = a.len;
		return *this;
	}
};

struct str : public array<char> {
	str() {}
	str(char* c) {
		this->fromCStr(c);
	}

	str& fromCStr(char* c) {
		assert(c);
		this->b = c;
		this->len = strlen(c);
		return *this;
	}

	str& fromArray(array<char> a) {
		this->b = a.b;
		this->len = a.len;
		return *this;
	}
	
	str& fromInt(int64 n, char* initBuffer) {
        assert(initBuffer);
        this->b = foxlib::strFromInt(n, initBuffer);
        this->len = &initBuffer[foxlib::maxInt64StrLen] - this->b;
        return *this;
    }

    str& fromUint(uint64 n, char* initBuffer) {
        assert(initBuffer);
        this->b = foxlib::strFromUint(n, initBuffer);
        this->len = &initBuffer[foxlib::maxUint64StrLen] - this->b;
        return *this;
    }

    str& fromFloat(float32 n, char* initBuffer) {
        assert(initBuffer);
        this->b = initBuffer;
        this->len = foxlib::strFromFloat(n, initBuffer);
        return *this;
    }
    
    constStr constify() const {
        constStr s;
        s.b = this->b;
        s.len = this->len;
        return s;
    }
};
