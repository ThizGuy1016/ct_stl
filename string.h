#ifndef _CT_STL_STRING_H
#define _CT_STL_STRING_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h> 

#include "optional.h"
#include "types.h"
#include "todo.h"

#define MEM_ALIGNMENT 32

typedef struct {
	char*	data;
	u64 mem_size;
} String_t;

Optional_t(u64);

typedef struct {
	String_t  (*owned_from)(const char*);
	String_t* (*from)(const char*);

	u64		(*size)(const String_t*);
	u64		(*len)(const String_t*);
	void	(*realloc)(String_t*, const u64);
	void	(*clear)(String_t*);
	char*	(*cstr)(const String_t*);
	char*	(*owned_cstr)(const String_t*);
	void	(*append)(String_t*, const char*);
	void	(*append_str)(String_t*, const String_t*);
	char*	(*at)(const String_t*, const u64);
	char*	(*begin)(const String_t*);
	char*	(*end)(const String_t*);
	bool	(*remove_idx)(String_t*, const u64, const u64);
	bool	(*insert)(String_t*, const char*, const u64);
	bool	(*insert_str)(String_t*, const String_t*, const u64);
	bool	(*replace)(String_t*, const u64, const char);
	
	Optional(u64) (*find)(const String_t*, const char*, const u64, const u64);
	void	(*rev)(String_t*);

	void	 (*free)(String_t* _string);	
} String_funcs;

u64 mem_round(u64 n, u64 alignment)
{
	double num = (n / alignment);
	u64 x = (u64)(num < 0 ? (num - 0.5) : (num + 0.5)) * alignment;
	return (x > 0) ? x : alignment;
}

String_t String_owned_from(const char* _str) 
{
	String_t string;
	
	u64 _str_len = strlen(_str) + 1;
	u64 _mem_size = mem_round(_str_len, MEM_ALIGNMENT);
	
	string.data = (char*)malloc(_mem_size);

	strncpy(string.data, _str, _str_len);

	string.mem_size = _mem_size;
	string.data[_str_len] = '\0';
	string.data[string.mem_size] = (string.mem_size - _str_len);

	return string;
}

String_t* String_from(const char* _str)
{
	/*
	 | Code duplication due to String_from_owned being an lvalue
	*/
	String_t* string;

	u64 _str_len = strlen(_str) + 1;
	u64 _mem_size = mem_round(_str_len, MEM_ALIGNMENT);

	string->data = (char*)malloc(_mem_size);

	strncpy(string->data, _str, _str_len);

	string->mem_size = _mem_size;
	string->data[_str_len] = '\0';
	string->data[string->mem_size] = (string->mem_size - _str_len);

	return string;
}

u64 String_size(const String_t* _string)
{
	/*
	 | Returns length of the string including null terminator
	*/
	return (_string->mem_size - _string->data[_string->mem_size]);
}

u64 String_len(const String_t* _string)
{
	/*
	 | Returns length of the string excluding null terminator
	*/
	return String_size(_string) - 1;
}

void String_realloc(String_t* _string, const u64 _offset)
{
	// in case the remaining size is 0
	u64 prev_len = String_size(_string);
	_string->data[_string->mem_size] = '0';

	_string->mem_size += mem_round(_offset, MEM_ALIGNMENT);

	_string->data = (char*)realloc(_string->data, _string->mem_size);
	_string->data[_string->mem_size] = _string->mem_size - strlen(_string->data); 
	printf("%u\n", _string->data[_string->mem_size]);
	
	return;
}

void String_clear(String_t* _string) 
{
	_string->data[0] = '\0';
	_string->data[_string->mem_size] = _string->mem_size;
}

char* String_cstr(const String_t* _string)
{
	return _string->data;
}

char* String_owned_cstr(const String_t* _string)
{
	char* cstr = (char*)malloc(_string->mem_size);
	memcpy(cstr, _string->data, String_len(_string));

	return cstr;
}

bool String_slice(String_t* _string, const u64 _start, const u64 _end)
{
	if (_start < 0 || _end > String_len(_string) || _start > _end ) return false;
	
	if (!strncpy(&_string->data[0], &_string->data[_start] + 1, (String_size(_string)))) return false;
	_string->data[_end - _start] = '\0';

	_string->data[_string->mem_size] = _string->mem_size - (_end - _start); 

	return true;
}

char* String_owned_slice(String_t* _string, const u64 _start, const u64 _end)
{
	if (_start < 0 || _end > String_len(_string) || _start > _end ) return NULL;
	
	char* buf = (char*)malloc((_end - _start));
	if (!strncpy(buf, &_string->data[_start], (_end - _start))) return NULL;
	return buf;
}

void String_append(String_t* _string, const char* _str)
{
	u64 new_str_len = strlen(_str);
	u32 prev_remaining = _string->data[_string->mem_size];
	if (_string->data[_string->mem_size] > new_str_len) {
		strncat(_string->data, _str, new_str_len);
		_string->data[_string->mem_size] -= new_str_len;
		return;
	}

	String_realloc(_string,  new_str_len);
	strncat(_string->data, _str, new_str_len);

	return;
}

void String_append_str(String_t* _string, const String_t* _str)
{
	String_append(_string, String_cstr(_str));
}

char* String_at(const String_t* _string, const u64 _idx)
{
	return (_idx <= String_size(_string)) ? &_string->data[_idx] : NULL;
}

char* String_begin(const String_t* _string)
{
	return &_string->data[0];
}

char* String_end(const String_t* _string)
{
	return &_string->data[String_len(_string)];
}

bool String_remove_idx(String_t* _string, const u64 _idx, const u64 _len)
{
	if ((_idx + _len) >= String_size(_string)) return false;
	else if (!strncpy(&_string->data[_idx], &_string->data[_len] + 1, (String_size(_string) - _idx))) return false;

	_string->data[_string->mem_size] += (_len - _idx); 
	return true;
}

bool String_remove(String_t* _string, const char* _str)
{
	u64 _str_len = strlen(_str);
	bool match = false;
	if (_str_len <= 0) return false;
	char* curr = _string->data;
	while ( (curr = strstr(curr, _str)) != NULL ) {
		memmove(curr, (curr + _str_len), (strlen(curr + _str_len) + 1));
		match = true;
	}
	return match;
}

bool String_insert(String_t* _string, const char* _str, const u64 _idx)
{
	if (_idx > String_len(_string)) return false;

	u64 _str_len = strlen(_str);

	if (_str_len + String_len(_string) > _string->mem_size )
		String_realloc(_string, _str_len);


	char* end_buf = (char*)malloc(strlen(&_string->data[_idx]));
	/*
	 |	Save everything after insertion index into a end_buf; 
	*/
	strncpy(end_buf, &_string->data[_idx], (String_len(_string) - _idx));
	/*
	 | Concat the given string after string->data at insertion index;
	*/
	memcpy(&_string->data[_idx], _str, _str_len);
	_string->data[_idx + _str_len] = '\0';
	/*
	 | Concat the ending of string after the string insertion
	*/
	strncat(_string->data, end_buf, strlen(end_buf));

	_string->data[_string->mem_size] = (_string->mem_size - strlen(_string->data)); 

	free(end_buf);
	
	return true;
}

bool String_insert_str(String_t* _string, const String_t* _str, const u64 _idx)
{
	if (!String_insert(_string, String_cstr(_str), _idx)) return false;
	return true;
}

bool String_replace(String_t* _string, const u64 _idx, const char c)
{
	if (_idx > String_len(_string)) return false;
	_string->data[_idx] = c;
	return true;
}

Optional(u64) String_find(const String_t* _string, const char* _str, const u64 _start, const u64 _end)
{
	TODO;
	return None(u64);
}

u64 String_count(const String_t* _string, const char* _str)
{
	TODO;
	return 0;
}

bool String_strip(String_t* _string, const char* _str)
{

	return true;
}

void String_rev(String_t* _string)
{
	TODO;
	return;
}

void String_dump(String_t* _string)
{
	printf("String: %s\nLength: %zu\nMemory: %zu bytes\nFree: %u bytes\n",
			String_cstr(_string), 
			String_size(_string), 
			_string->mem_size,
			_string->data[_string->mem_size]);
}

void String_shrink_to_fit(String_t* _string)
{
	char* new_data = String_owned_cstr(_string);
	
	_string->mem_size = String_size(_string);
	free(_string->data);
	_string->data = new_data;

	return;
}

void String_free(String_t* _string)
{
	free(_string->data);
	_string->data = NULL;
	_string = NULL;
}

String_funcs String = {
	String_owned_from,
	String_from,
	String_size,
	String_len,
	String_realloc,
	String_clear,
	String_cstr,
	String_owned_cstr,
	String_append,
	String_append_str,
	String_at,
	String_begin,
	String_end,
	String_remove_idx,
	String_insert,
	String_insert_str,
	String_replace,
	String_find,
	String_rev,
	String_free,
};

#endif // End _CT_STL_STRING_H
