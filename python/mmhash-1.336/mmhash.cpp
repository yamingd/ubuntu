/*


MurmurHash2 http://murmurhash.googlepages.com/
MurmurHash2, 64-bit versions, by Austin Appleby

Modified by changsheng@douban.com

zsp windows,linux python lib

*/

#include <Python.h>
#include <sys/types.h>

#if defined(_MSC_VER)
typedef signed __int8 int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef signed __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif 


#define FNV_32_PRIME 0x01000193ULL

uint32_t mmhash_32( const void * key, int len){
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const uint32_t m = 0x5bd1e995;
	const uint32_t r = 24;

	// Initialize the hash to a 'random' value

	uint32_t h = FNV_32_PRIME ^ len;

	// Mix 4 bytes at a time into the hash

	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
	{
		uint32_t k = *(uint32_t *)data;

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;

		data += 4;
		len -= 4;
	}
	
	// Handle the last few bytes of the input array

	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

uint64_t mmhash_64(const void *key, size_t len) {
	const uint64_t m = 0xc6a4a7935bd1e995ULL;
	const uint64_t r = 47;
	uint64_t h = FNV_32_PRIME ^ (len * m);
	const uint64_t * data = (const uint64_t *)key;
	const uint64_t * end = data + (len/8);

	uint64_t k ;
	while (data != end) {
		k = *data++;
		k *= m;
		k ^= k >> r;
		k *= m;
		h ^= k;
		h *= m;
	};

	const unsigned char * data2 = (const unsigned char*)data;
	switch (len & 7) {
	case 7: h ^= (uint64_t)(data2[6]) << 48;
	case 6: h ^= (uint64_t)(data2[5]) << 40;
	case 5: h ^= (uint64_t)(data2[4]) << 32;
	case 4: h ^= (uint64_t)(data2[3]) << 24;
	case 3: h ^= (uint64_t)(data2[2]) << 16;
	case 2: h ^= (uint64_t)(data2[1]) << 8;
	case 1: h ^= (uint64_t)(data2[0]);
		h *= m;
	}
	h ^= h >> r;
	h *= m;
	h ^= h >> r;
	return h;
}


static PyObject * get_unsigned_hash64(PyObject *self,PyObject *args) {
	char * guid;
	unsigned int len;
	if(!PyArg_ParseTuple(args,"z#",&guid,&len)) {
		return NULL;
	}
	uint64_t h =  mmhash_64(guid,len);
	return PyLong_FromUnsignedLongLong(h);
}

static PyObject * get_hash32(PyObject *self,PyObject *args) {
	char * guid;
	unsigned int len;
	if(!PyArg_ParseTuple(args,"z#",&guid,&len)) {
		return NULL;
	}
	uint32_t h =  mmhash_32(guid,len);
	return PyInt_FromLong(h);
}

static PyObject * get_hash64(PyObject *self,PyObject *args) {
	char * guid;
    unsigned int len;
	if(!PyArg_ParseTuple(args,"z#",&guid,&len)) {
		return NULL;
	}
	int64_t h =  mmhash_64(guid,len);
	/* return Py_BuildValue("l",h); */
#if defined(__x86_64__) || defined(_M_X64)
	return PyInt_FromLong(h);
#else
	return PyLong_FromLongLong(h);
#endif
}


static PyMethodDef methods[] = {
	{"hash64",(PyCFunction)get_hash64,METH_VARARGS,NULL},
	{"uhash64",(PyCFunction)get_unsigned_hash64,METH_VARARGS,NULL},
    {"hash32",(PyCFunction)get_hash32,METH_VARARGS,NULL},
	{NULL,NULL,0,NULL}
};

PyMODINIT_FUNC initmmhash() {
	Py_InitModule3("mmhash", methods, "MurmurHash2 hash algorithm extension module.");
}
