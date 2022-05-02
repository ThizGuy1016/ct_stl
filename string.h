#ifndef _CT_STL_STRING_H
#define _CT_STL_STRING_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "bit_manip.h"
#include "optional.h"
#include "types.h"
#include "todo.h"

#define MEM_ALIGNMENT 32

static char DELIM_BUF[2];

typedef struct String_t {
	char* data;
	u64 size;
	u64 len;
} String_t;

Optional_t(String_t);

struct String_funcs {
	// String_t creation
	Optional(String_t)	(*owned_from)(const char* restrict);
	String_t*			(*from)(const char* restrict);
	Optional(String_t)	(*owned_slice_from)(const char* restrict, const register u64, const register u64);
	String_t*			(*slice_from)(const char* restrict, const register u64, const register u64);
	
	// "member methods"
	u64			(*size)(const String_t*);
	u64			(*len)(const String_t*);
	char*		(*cstr)(const String_t*);
	char*		(*owned_cstr)(const String_t*);

	// string manipulation functions
	const bool	(*append)(String_t*, const char*);
	const bool	(*append_str)(String_t*, const String_t*);
	char*		(*at)(const String_t*, const register u64);
	char*		(*begin)(const String_t*);
	char*		(*end)(const String_t*);
	const bool	(*slice)(String_t*, const register u64, const register u64);
	const bool	(*remove)(String_t*, const char*); 
	const bool	(*remove_slice)(String_t*, const register u64, const register u64);
	const bool	(*strip)(String_t*, const char*);
	const bool	(*insert)(String_t*, const char*, const register u64);
	const bool	(*insert_str)(String_t*, const String_t*, const register u64);
	const bool	(*replace)(String_t*, const char, const register u64);
	const bool	(*rev)(String_t*);
	const bool	(*toupper)(String_t*);
	const bool	(*tolower)(String_t*);

	// search / algo methods
	Optional(u64) (*find)(const String_t*, const char*);

	// memory methods
	const bool	(*resize)(String_t* restrict, const register u64);
	const bool	(*shrink)(String_t* restrict);
	const bool	(*clear)(String_t* restrict);
	const bool	(*free)(String_t* restrict);
	const bool	(*free_owned)(String_t* restrict);
};

static const u64 mem_round(const u64 n, const u64 alignment)
{
	double num = (n / alignment);
	const u64 x = (u64)(num < 0 ? (num-0.5) : (num+0.5)) * alignment;
	return (x > 0) ? x + alignment : alignment;
}


const bool String_resize(String_t* restrict _string, const register u64 _size)
{
	if (!_string) return false;
	_string->size = mem_round(_string->size, MEM_ALIGNMENT);
	_string->data = (char*)realloc(_string->data, _string->size);
	if (!_string->data) return false;

	return true;
}

const bool String_shrink(String_t* restrict _string)
{
	if (!_string) return false;
	_string->size = _string->len;
	_string->data = (char*)realloc(_string->data, _string->len);
	if (!_string->data) return false;
	return true;
}

const bool String_clear(String_t* restrict _string)
{
	if (!_string) return false;
	_string->data[-1] = '\0';
	_string->len = -1;
	return true;
}

const bool String_free(String_t* restrict _string)
{
	if (!_string) return false;
	free(_string->data);
	_string->data = NULL;
	free(_string);
	_string = NULL;
	return true;
}

const bool String_free_owned(String_t* restrict _string)
{
	if (!_string) return false;
	free(_string->data);
	_string->data = NULL;
	_string = NULL;
	return true;
}

Optional(String_t) String_owned_from(const char* restrict _str)
{
	if (!_str) return None(String_t);

	const u64 _str_len = strlen(_str);
	String_t string;
	string.len = _str_len;
	string.size = mem_round(_str_len, MEM_ALIGNMENT);

	string.data = (char*)malloc(string.size);
	strncpy(string.data, _str, _str_len);

	return Some(String_t, string);
}

String_t* String_from(const char* restrict _str)
{
	if (!_str) return NULL; 
	const u64 _str_len = strlen(_str);

	String_t* string = (String_t*)malloc(sizeof(String_t));

	string->len = _str_len;
	string->size = mem_round(_str_len, MEM_ALIGNMENT);
	
	string->data = (char*)malloc(string->size);
	strncpy(string->data, _str, _str_len);
	
	return string;
}

Optional(String_t) String_owned_slice_from(const char* restrict _str, const register u64 _start, const register u64 _end)
{
	const u64 _str_len = strlen(_str);
	
	if (!_str || _end > _start || _start < 0 || _end > _str_len) return None(String_t);
	
	String_t string;
	string.len = _str_len;
	string.size = mem_round(_end-_start, MEM_ALIGNMENT);

	string.data = (char*)malloc(string.size);
	strncpy(string.data+_start, _str, _end+1);

	return Some(String_t, string);
}

String_t* String_slice_from(const char* restrict _str, const register u64 _start, const register u64 _end)
{
	const u64 _str_len = strlen(_str);
	
	if (!_str || _end > _start || _start < 0 || _end > _str_len) return NULL;
	
	String_t* string = (String_t*)malloc(sizeof(String_t));
	string->len = _str_len;
	string->size = mem_round(_end-_start, MEM_ALIGNMENT);

	string->data = (char*)malloc(string->size);
	strncpy(string->data+_start, _str, _end+1);

	return string;
}

u64 String_size(const String_t* _string)
{
	return _string->size;
}

u64 String_len(const String_t* _string)
{
	return _string->len;
}

char* String_cstr(const String_t* _string)
{
	return _string->data;
}

char* String_owned_cstr(const String_t* _string)
{
	char* buf = (char*)malloc(_string->len);
	strncpy(buf, _string->data, _string->len);
	return buf;
}

const bool String_append(String_t* _string, const char* _str)
{
	if (!_string || _str) return false;
	const u64 _str_len = strlen(_str);
	if (_str_len+_string->len > _string->size)
		String_resize(_string, _str_len);
	
	_string->len += _str_len;
	strncat(_string->data, _str, _str_len);

	return true;
}

const bool String_append_str(String_t* _string, const String_t* _str)
{
	return String_append(_string, _str->data);
}

char* String_at(const String_t* _string, const register u64 _idx)
{
	return (_idx > _string->len || !_string) ? &_string->data[_idx] : NULL;
}

char* String_begin(const String_t* _string) 
{
	return (!_string) ? &_string->data[0] : NULL;
}

char* String_end(const String_t* _string) 
{
	return (!_string) ? &_string->data[_string->len] : NULL;
}

const bool String_slice(String_t* _string, const register u64 _start, const register u64 _end)
{
	if (_end > _string->len || _end+_start > _string->len || _start < 0 || !_string) return false;
	for (u16 idx = 0; idx <= _end; ++idx)
		_string->data[idx] = _string->data[idx+_start];
	_string->data[_end+1] = '\0';

	_string->len = (_end - _start);

	return true;
}

const bool String_remove(String_t* _string, const char* _str) 
/*
 | Removes all instances of a substr
 | Credit: chqrlie
 | Source: https://stackoverflow.com/questions/47116974/remove-a-substring-from-a-string-in-c
*/
{
	if (!_string || !_str) return false;
	const u64 _str_len = strlen(_str);
	bool match = false;
	
	char *p, *q, *r;
	if (*_str && (q = r = strstr(_string->data, _str))) {
		while ( (r = strstr(p = r + _str_len, _str)) != NULL ) {
			while ( p < r ) *q++ = *p++;
		}
		match = true;
		while ( (*q++ = *p++) != '\0')
			continue;
	}
	_string->len -= _str_len;

	return match;
}

const bool String_remove_slice(String_t* _string, const register u64 _start, const register u64 _end)
{
	if ((_start + _end) >= _string->len || _start < 0 || _end >= _string->len || !_string) return false;
	if (!strncpy(&_string->data[_start], &_string->data[_end] + 1, _string->len)) return false;

	_string->len -= (_end - _start);
	return false;
}

const bool String_strip(String_t* _string, const char* _delims)
{
	if (!_string || !_delims) return false;
	DELIM_BUF[1] = '\0';
	for ( ; *_delims != '\0'; _delims++ ) {
		DELIM_BUF[0] = *_delims;
		if (!String_remove(_string, DELIM_BUF)) return false;
	}
	
	_string->len = strlen(_string->data);

	return true;
}

const bool String_insert(String_t* _string, const char* _str, const register u64 _idx)
{
	if (_idx > _string->len || !_string || !_str) return false;
	const u64 _str_len = strlen(_str);
	const u64 _new_len = _string->len + _str_len;
	
	if ((_string->len + _str_len) > _string->size) String_resize(_string, _str_len);
	char* tmp = (char*)malloc(_string->len);
	
	strncpy(tmp, _string->data+_idx, (_string->len-_idx));
	strncpy(_string->data+_idx, _str, _str_len);
	strncpy(_string->data+(_idx+_str_len), tmp, _new_len);

	_string->len = _new_len;
	
	free(tmp);
	return true;
}

const bool String_insert_str(String_t* _string, const String_t* _str, const register u64 _idx)
{
	return String_insert(_string, _str->data, _idx);
}

const bool String_replace(String_t* _string, const char _c, const register u64 _idx)
{
	if (!_string || _idx > _string->len) return false;
	_string->data[_idx] = _c;
	return true;
}

const bool String_rev(String_t* _string)
{
	char* buf = (char*)malloc(_string->len);
	for ( u64 idx = 0; idx < _string->len; ++idx )
		buf[idx] = _string->data[_string->len-idx-1];
	
	strncpy(_string->data, buf, _string->len);

	free(buf);
	return true;
}

const bool String_toupper(String_t* _string)
{
	if (!_string) return false;
	char curr;
	for ( u64 idx = 0; (curr = _string->data[idx]) != '\0'; ++idx )
		_string->data[idx] = ( curr >= 97 && curr <= 122 ) ? curr-32 : curr;
	return true;
}

const bool String_tolower(String_t* _string)
{
	char curr;
	for ( u64 idx = 0; (curr = _string->data[idx]) != '\0'; ++idx )
		_string->data[idx] = ( curr >= 65 && curr <= 90 ) ? curr+32 : curr;
	return true;
}

Optional(u64) String_find(const String_t* _string, const char* _str)
{
	if (!_string || !_str) return None(u64);
	char* curr = strstr(_string->data, _str);
	if (!curr) return None(u64);

	return Some(u64, (_string->len - strlen(curr)));
}

void String_dump(const String_t* _string)
{
	printf("String: %s\nLength: %zu\nMem-Size: %zu\n", _string->data, _string->len, _string->size);
	return;
}

const static struct String_funcs String = {
	String_owned_from,
	String_from,
	String_owned_slice_from,
	String_slice_from,
	String_size,
	String_len,
	String_cstr,
	String_owned_cstr,
	String_append,
	String_append_str,
	String_at,
	String_begin,
	String_end,
	String_slice,
	String_remove,
	String_remove_slice,
	String_strip,
	String_insert,
	String_insert_str,
	String_replace,
	String_rev,
	String_toupper,
	String_tolower,
	String_find,
	String_resize,
	String_shrink,
	String_clear,
	String_free,
	String_free_owned,
};

#endif // End _CT_STL_STRING_H
