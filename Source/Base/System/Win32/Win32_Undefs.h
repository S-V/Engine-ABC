/*
=============================================================================
	File:	Win32_Undefs.h
	Desc:	Undefines some Windows macros.
=============================================================================
*/

// Fix Windows defines.

#undef BOOL
#undef WORD
#undef DWORD
#undef INT
#undef FLOAT

#undef CDECL

#ifdef CreateEvent
#undef CreateEvent
#endif

#ifdef CreateFile
#undef CreateFile
#endif

#ifdef CopyFile
#undef CopyFile
#endif

#ifdef GetObject
#undef GetObject
#endif

#ifdef GetClassName
#undef GetClassName
#endif

#ifdef RegisterClass
#undef RegisterClass
#endif

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
