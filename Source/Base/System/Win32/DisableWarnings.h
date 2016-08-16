/*
=============================================================================
	File:	DisableWarnings.h
	Desc:	Used to prevent some compiler warnings.
=============================================================================
*/

//	Inhibit certain compiler warnings.

#define _CRT_SECURE_NO_WARNINGS

#pragma warning ( disable: 4127 )	// conditional expression is constant
#pragma warning ( disable: 4100 )	// unreferenced formal parameter

#pragma warning ( disable: 4482 )	// Nonstandard extension: enum used in qualified name.
#pragma warning ( disable: 4201 )	// Nonstandard extension used : nameless struct/union.
#pragma warning ( disable: 4239 )	// Nonstandard extension used : conversion from X to Y.

#pragma warning ( disable: 4245 )	// 'argument': conversion from 'type1' to 'type2', signed/unsigned mismatch
#pragma warning ( disable: 4244 )	// 'argument': conversion from 'double' to 'float', possible loss of data
#pragma warning ( disable: 4267 )	// 'argument': conversion from 'size_t' to 'int', possible loss of data

#pragma warning ( disable: 4311 )	// 'variable' : pointer truncation from 'type1' to 'type2'
#pragma warning ( disable: 4312 )	// 'type cast' : conversion from 'type1' to 'type2' of greater size

#pragma warning ( disable: 4324 )	// structure was padded due to __declspec(align())

#pragma warning ( disable: 4172 )	// returning address of local variable or temporary

#pragma warning ( disable: 4512 )	// assignment operator could not be generated

#pragma warning ( disable: 4996 )	// Deprecation warning: this function or variable may be unsafe.

#pragma warning ( disable: 4099 )	// type name first seen using 'class' now seen using 'struct'

#pragma warning ( disable: 4786 )	// the fully-qualified name of the class you are using is too long to fit into the debug information and will be truncated to 255 characters

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
