#ifndef _OPTIONAL_C
#define _OPTIONAL_C

#include "stdbool.h"
#include "stdlib.h"
#include "assert.h"

#define Optional(_optional_t) optional_##_optional_t##_t

#define Some(_optional_t, _optional_contents) \
	((Optional(_optional_t)) {				  \
		.is_none = false,					  \
		.contents = _optional_contents, } )
// End Some

#define None(_optional_t)	   \
	((Optional(_optional_t)) { \
		.is_none = true, } )
// End None

#define Optional_t(_optional_t) \
	typedef struct {					  \
			bool is_none;				  \
			_optional_t contents;		  \
	} Optional(_optional_t)
// End Optional_t

#define Optional_Named_t(_optional_t, _optional_n) \
	typedef _optional_t _optional_n;			   \
	Optional_t(_optional_n)						   \
// End Optional_Named_t

#define IsNone(_optional) ((*_optional).is_none)
#define IsNone_owned(_optional) ((_optional).is_none)
#define IsSome(_optional) (!(IsNone(_optional)))
#define IsSome_owned(_optional) (!IsNone_owned(_optional))
#define IsNull(_ptr) (_ptr == NULL)

#define Unwrap(_optional_ptr, _val_ptr) \
	if (IsNone(_optional_ptr)) { \
		fprintf(stderr, "%s:%d Unwrap Failed: Assignment to '%s' resulted in None!\n", __FILE__, __LINE__, #_optional_ptr);\
		abort(); \
	} \
	*_val_ptr = (*_optional_ptr).contents;
// End Unwrap

#define Unwrap_owned(_optional, _val_ptr) \
	if (IsNone(&_optional)) { \
		fprintf(stderr, "%s:%d Unwrap Failed: Assignment to '%s' resulted in None!\n", __FILE__, __LINE__, #_optional);\
		abort(); \
	} \
	*_val_ptr = _optional.contents
// End Unwrap_owned

#define Unwrap_or(_optional_ptr, _val_ptr, _fallback_ptr) \
	if (IsNone(_optional_ptr)) *_val_ptr = *_fallback_ptr; \
	*_val_ptr = (*_optioanl_ptr).contents
// End Unwrap_or

#define Unwrap_or_owned(_optional, _val_ptr, _fallback) \
	if (IsNone_owned(_optional)) *_val_ptr = _fallback;\
	*_val_ptr = (_optioanl).contents
// End Unwrap_or_owned

#define Unwrap_null(_ptr, _val_ptr) \
	if (IsNull(_ptr)) { \
		fprintf(stderr, "%s:%d Unwrap Failed: '%s' was assigned a null value!\n", __FILE__, __LINE__, #_ptr); \
	} \
	*_ptr = *_val_ptr
// End Unwrap_null

#define Unwrap_null_owned(_ptr, _val) \
	if (IsNull(_ptr)) { \
		fprintf(stderr, "%s:%d Unwrap Failed: '%s' was assigned a null value!\n", __FILE__, __LINE__, #_ptr); \
	} \
	*_ptr = _val
// End Unwrap_null_owned

#define Unwrap_null_assign(_ptr, _val_ptr) \
	if (IsNull(_ptr)) { \
		fprintf(stderr, "%s:%d Unwrap Failed: '%s' was assigned a null value!\n", __FILE__, __LINE__, #_ptr); \
	} \
	*_val_ptr = *_ptr
// End Unwrap_null_assign

#endif //_OPTIONAL_C
