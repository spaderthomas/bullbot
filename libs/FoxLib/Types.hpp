/*Numeric fixed size types.*/
typedef std::int8_t int8;
typedef std::int16_t int16;
typedef std::int32_t int32;
typedef std::int64_t int64;

typedef std::uint8_t uint8;
typedef std::uint16_t uint16;
typedef std::uint32_t uint32;
typedef std::uint64_t uint64;

typedef float float32;
typedef double float64;

union float32Parts {
	float32 all;
	
	struct {
		uint32 mantisa : 23;
		uint32 exponent : 8;
		uint32 sign : 1;
	} parts;
};

union float64Parts {
	float64 all;
	
	struct {
		uint64 mantisa : 52;
		uint64 exponent : 11;
		uint64 sign : 1;
	} parts;
};

//Fox (u)int
#ifdef foxLib64
	typedef int64 fint;
	typedef uint64 fuint;
#else
	typedef int32 fint;
	typedef uint32 fuint;
#endif
