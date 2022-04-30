#ifndef _CT_STACK_STRING_H
#define _CT_STACK_STRING_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "todo.h"
#include "bit_manip.h"
#include "optional.h"

#define Stack_Size 32

static char TmpStringBuf[Stack_Size];

typedef union SS_t { 
	char data[Stack_Size]; 
} SS_t;

Optional_t(SS_t);
Optional_t(u16);

typedef struct StackString {
	Optional(SS_t) (*owned_from)(const char* restrict);
	const u16	   (*len)(const SS_t* restrict);
	const u16	   (*size)(const SS_t* restrict);
	char*		   (*cstr)(SS_t* restrict);
	char*		   (*owned_cstr)(const SS_t* restrict);
	char*		   (*at)(SS_t* restrict, const register u16);
	char*		   (*begin)(SS_t*);
	char*		   (*end)(SS_t*);
	const bool	   (*slice)(SS_t* restrict, const register u16, const register u16);
	Optional(SS_t) (*owned_slice)(SS_t* restrict, const register u16, const register u16);
	const bool	   (*append)(SS_t* restrict, const char* restrict);
	const bool	   (*insert)(SS_t* restrict, const char* restrict, const register u16);
	const bool	   (*toupper)(SS_t* restrict);
	const bool	   (*tolower)(SS_t* restrict);
	Optional(u16)  (*find)(const SS_t* restrict, const char* restrict);
	const bool	   (*replace)(SS_t* restrict, const char, const register u16 _idx);
	const bool	   (*strip)(SS_t* restrict, const char*);
	const bool	   (*clear)(SS_t* restrict);
} StackString;

Optional(SS_t) StackString_owned_from(const char* restrict _str)
/*
 | Returns an optionally owned StackString initialized with the given _str
*/
{	
	if (!_str) return None(SS_t);
	const register u16 _str_len = strlen(_str);
	if (_str_len > Stack_Size) return None(SS_t);

	SS_t string;

	if(!strncpy(string.data, _str, _str_len)) return None(SS_t);	
	string.data[Stack_Size-1] = (Stack_Size - _str_len);

	return Some(SS_t, string);
}

const u16 StackString_len(const SS_t* restrict _string)
/*
 | Returns the length of the string within the StackString 
*/
{
	return _string ? (Stack_Size - _string->data[Stack_Size-1]) : -1;
}

const u16 StackString_mem_size(const SS_t* restrict _string)
{
	return _string ? Stack_Size : -1;
}

char* StackString_cstr(SS_t* restrict _string)
/*
 | Returns a pointer to the string stored within the given StackString
*/
{
	return _string ? _string->data : NULL;
}

char* StackString_owned_cstr(const SS_t* restrict _string)
/*
 | Returns a pointer to a copy of the string stored in the given StackString
*/
{
	const register u16 _string_len = StackString_len(_string);
	char* cstr_buf = (char*)malloc(_string_len+1);
	strncpy(cstr_buf, _string->data, _string_len);
	return cstr_buf;
}

char* StackString_at(SS_t* restrict _string, const register u16 _idx)
/*
 | Returns a pointer to the given SS at the given _idx
*/
{
	return (_string || _idx > StackString_len(_string)) ? &_string->data[_idx] : NULL;
}

char* StackString_begin(SS_t* restrict _string)
/*
 | Returns a pointer to the begining of the given SS
*/
{
	return (_string) ? &_string->data[0] : NULL;
}

char* StackString_end(SS_t* restrict _string)
/*
 | Returns a pointer to the end of the given SS
*/
{
	return (_string) ? &_string->data[StackString_len(_string)] : NULL;
}

const bool StackString_slice(SS_t* restrict _string, const register u16 _start, const register u16 _end)
/*
 | Slices the string in the given StackString to the starting and ending indicies
*/
{
	if (_end > StackString_len(_string) || _end > StackString_len(_string) || _start < 0 ) return false;
	for ( u16 idx = 0; idx <= _end; ++idx)
		_string->data[idx] = _string->data[idx+_start];
	_string->data[_end+1] = '\0';

	_string->data[Stack_Size-1] = (_end - _start);

	return true;
}

Optional(SS_t) StackString_owned_slice(SS_t* restrict _string, const register u16 _start, const register u16 _end)
/*
 | Slices the string in the given StackString given the starting and ending indicies and returns an pointer to an owned optional StackString
*/
{
	if (_end > StackString_len(_string) || _end > StackString_len(_string) || _start < 0 ) return None(SS_t);
	SS_t new_string;
	strncpy(new_string.data, _string->data+_start, _end);

	new_string.data[Stack_Size-1] = (_end - _start);

	return Some(SS_t, new_string);
}

const bool StackString_append(SS_t* restrict _string, const char* restrict _str)
/*
 | Appends a string to the end of the given Stack String
*/
{
	const register u16 _str_len = strlen(_str);
	const register u16 old_len = StackString_len(_string); 
	if ((StackString_len(_string) + _str_len) > Stack_Size) return false;
	if (!strncat(_string->data, _str, _str_len)) return false;
	_string->data[Stack_Size-1] = (Stack_Size - (_str_len + old_len));

	return true;
}

const bool StackString_insert(SS_t* restrict _string, const char* restrict _str, const register u16 _idx)
/*
 | Inserts a string into the given Stack String at a specified index
*/
{
	const register u16 _str_len = strlen(_str);
	const register u16 new_len = StackString_len(_string) + _str_len;
	if (StackString_len(_string + _str_len) > Stack_Size) return false;	
	
	/*
	 | strncpy should only fail if glibc allocater fails. so this code is "safe"
	*/
	strncpy(TmpStringBuf, _string->data+_idx, (StackString_len(_string) - _idx));
	strncpy(_string->data+_idx, _str, _str_len);
	strncpy(_string->data+(_idx+_str_len), TmpStringBuf, new_len);

	_string->data[Stack_Size-1] = (Stack_Size - new_len);

	return true;
}

const bool StackString_toupper(SS_t* restrict _string)
/*
 | Converts the given string to all uppercase characters
 | Can be optimized in the future using SIMD instructions
*/
{

	if (!_string) return false;
	char curr;
	for ( u16 idx = 0; (curr = _string->data[idx]) != '\0'; ++idx ) {
		_string->data[idx] = (curr >= 97 && curr <= 122 ) ? curr-32 : curr;
	}
	return true;
}

const bool StackString_tolower(SS_t* restrict _string)
/*
 | Converts the given string to all lowercase characters
*/
{
	if (!_string) return false;
	char curr;
	for ( u16 idx = 0; (curr = _string->data[idx]) != '\0'; ++idx ) {
		_string->data[idx] = (curr >= 65 && curr <= 90 ) ? curr+32 : curr;
	}
	return true;
}

static void StackString_compute_lps(const char* _pattern, const u16 _pattern_len)
{
	/*
	 | In this case TmpStringBuf will serve double duty by storing all of the prefix indicies
	*/
	u16 prev_suffix_len = 0;
	
	u16 i = 1;
	while ( i < _pattern_len ) {
		if (_pattern[i] == _pattern[_pattern_len]) {
			TmpStringBuf[i] = ++prev_suffix_len;
			i++;
		
		} else {
			if (prev_suffix_len != 0)
				prev_suffix_len = TmpStringBuf[prev_suffix_len-1];
			else {
				TmpStringBuf[i] = 0;
				i++;
			}
		}
	}
}

static Optional(u16) string_find(const char* restrict _haystack, const char* restrict _needle, const u16 _haystack_len)
/*
 | returns the index of the first occurance of _str in the given _string
 | using an implementation of the KMP search algorigthm: https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
*/
{
	const u16 _needle_len = strlen(_needle);

	StackString_compute_lps(_haystack, _haystack_len);

	u16 _haystack_idx = 0; 
	u16 _needle_idx = 0;
	while ( _haystack_idx < _haystack_len ) {
		if (_needle[_needle_idx] == _haystack[_haystack_idx]) {
			++_haystack_idx;
			++_needle_idx;
		}
		if (_needle_idx == _needle_len) {
			return Some(u16, (_haystack_idx - _needle_idx));
			_needle_idx = TmpStringBuf[_needle_idx-1];
		
		} else if (_haystack_idx < _haystack_len && _needle[_needle_idx] != _haystack[_haystack_idx]) {
			if (_needle_idx != 0)
				_needle_idx = TmpStringBuf[_needle_idx-1];
			else
				_haystack_idx++;
		}
	}

	return None(u16);
}

Optional(u16) StackString_find(const SS_t* restrict _haystack, const char* restrict _needle)
{
	return string_find(_haystack->data, _needle, StackString_len(_haystack));
}

const bool StackString_remove(SS_t* restrict _string, const char* _str)
/*
 | Removes all instances of a substir
 | Credit: chqrlie
 | Source: https://stackoverflow.com/questions/47116974/remove-a-substring-from-a-string-in-c
 | In the future, replace strstr with StackString_find
*/
{
	const u16 _str_len = strlen(_str);
	const u16 _string_len = StackString_len(_string);
	
	char *p, *q, *r;
	if (*_str && (q = r = strstr(_string->data, _str))) {
		while ( (r = strstr(p = r + _str_len, _str)) != NULL ) {
			while ( p < r ) *q++ = *p++;
		}
		while ( (*q++ = *p++) != '\0')
			continue;
	}

	_string->data[Stack_Size-1] = (Stack_Size - strlen(_string->data));
	return true;
}

const bool StackString_replace(SS_t* restrict _string, const char _c, const register u16 _idx)
/*
 | Replaces the string at _idx with character _c
*/
{
	const u16 _string_len = StackString_len(_string);
	if (_idx > _string_len) return false;
	_string->data[_idx] = _c;
	return true;
}

const bool StackString_strip(SS_t* restrict _string, const char* _delim)
/*
 | Removes all instances of each character in _delim
*/
{
	TmpStringBuf[1] = '\0';
	for ( ; *_delim != '\0'; _delim++ ) {
		TmpStringBuf[0] = *_delim;
		if (!StackString_remove(_string, TmpStringBuf)) return false;
	}
	
	_string->data[Stack_Size-1] = (Stack_Size - strlen(_string->data));

	return true;
}

const bool StackString_clear(SS_t* restrict _string)
/*
 | Clears the given string
*/
{
	if (!_string) return false;
	_string->data[0] = '\0';
	_string->data[Stack_Size-1] = Stack_Size;
	return true;
}

void StackString_dump(const SS_t* restrict _string)
{
	printf("String: %s\nLength: %u\nSize: %u\n",
			_string->data,
			StackString_len(_string),
			StackString_mem_size(_string)
			);	
}

StackString SS = {
	StackString_owned_from,
	StackString_len,
	StackString_mem_size,
	StackString_cstr,
	StackString_owned_cstr,
	StackString_at,
	StackString_begin,
	StackString_end,
	StackString_slice,
	StackString_owned_slice,
	StackString_append,
	StackString_insert,
	StackString_toupper,
	StackString_tolower,
	StackString_find,
	StackString_replace,
	StackString_strip,
	StackString_clear,
};

#endif // End _CT_STACK_STRING_H
