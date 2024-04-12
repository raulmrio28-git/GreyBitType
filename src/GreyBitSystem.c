/*
** ===========================================================================
** File: GreyBitSystem.c
** Description: GreyBit font library - System-related function handling
** Copyright (c) 2023
** Portions of this software (C) 2010 The FreeType Project (www.freetype.org).
** All rights reserved.
** History:
** when			who				what, where, why
** MM-DD-YYYY-- --------------- --------------------------------
** 09/16/2023	me				Upgrade
** 08/07/2023	me              Init
** ===========================================================================
*/

/*
**----------------------------------------------------------------------------
**  Includes
**----------------------------------------------------------------------------
*/

#include "../GreyBitSystem.h"

/*
**----------------------------------------------------------------------------
**  Definitions
**----------------------------------------------------------------------------
*/

/*
**----------------------------------------------------------------------------
**  Type Definitions
**----------------------------------------------------------------------------
*/

typedef struct _GB_MemStreamRec
{
	GB_BYTE*	pData;
	GB_INT32	size;
	GB_INT32	pos;
} GB_MemStreamRec, * GB_MemStream;

/*
**----------------------------------------------------------------------------
**  Global variables
**----------------------------------------------------------------------------
*/

/*
**----------------------------------------------------------------------------
**  Internal variables
**----------------------------------------------------------------------------
*/

/*
**----------------------------------------------------------------------------
**  Function(internal use only) Declarations
**----------------------------------------------------------------------------
*/

/* externs */
extern
GB_IOHandler GreyBit_Open_Sys(const GB_CHAR * p, GB_BOOL bcreate);
extern
GB_INT32	GreyBit_Read_Sys(GB_IOHandler f, GB_BYTE *p,
							 GB_INT32 size);
extern
GB_INT32	GreyBit_Write_Sys(GB_IOHandler f, GB_BYTE *p,
							  GB_INT32 size);
extern
GB_INT32	GreyBit_Seek_Sys(GB_IOHandler f, GB_INT32 pos);
extern
GB_INT32	GreyBit_GetSize_Sys(GB_IOHandler f);
extern
void		GreyBit_Close_Sys(GB_IOHandler f);
extern
void *		GreyBit_Malloc_Sys(GB_INT32 size);
extern
void *		GreyBit_Realloc_Sys(void * p, GB_INT32 newsize);
extern
void		GreyBit_Free_Sys(void * p);

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Memory_New
** Description: Initialize memory handler
** Input: none
** Output: New memory handler
** Return value: mem
** ---------------------------------------------------------------------------
*/

GB_Memory	GreyBit_Memory_New()
{
	GB_Memory	mem; 

	mem = (GB_Memory)GreyBit_Malloc_Sys(sizeof(GB_MemoryRec));
	if (mem)
	{
		mem->malloc = GreyBit_Malloc_Sys;
		mem->realloc = GreyBit_Realloc_Sys;
		mem->free = GreyBit_Free_Sys;
	}
	return mem;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Malloc
** Description: Allocate memory
** Input: mem - memory
**        size - size
** Output: Allocated memory
** Return value: none
** ---------------------------------------------------------------------------
*/

void *		GreyBit_Malloc(GB_Memory mem, GB_INT32 size)
{
	return mem->malloc(size);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Realloc
** Description: Reallocate memory
** Input: mem - memory
**        p - pointer
**        size - size
** Output: Reallocated memory
** Return value: none
** ---------------------------------------------------------------------------
*/

void *		GreyBit_Realloc(GB_Memory mem, void * p, GB_INT32 newsize)
{
	return mem->realloc(p, newsize);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Free
** Input: mem - memory
**        p - pointer
** Output: Freed memory
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBit_Free(GB_Memory mem, void *p)
{
	mem->free(p);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Memory_Done
** Description: Done with memory handling? Nuke it!
** Input: mem - memory
** Output: Nuked memory handler
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBit_Memory_Done(GB_Memory mem)
{
	GreyBit_Free_Sys(mem);
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Open_Mem
** Description: Open buffer for memstream
** Input: p - pointer
**        size - size
** Output: Open stream
** Return value: memstream
** ---------------------------------------------------------------------------
*/

GB_MemStream GreyBit_Open_Mem(const void * p, GB_INT32 size)
{
	GB_MemStream	memstream; 

	memstream = (GB_MemStream)GreyBit_Malloc_Sys(sizeof(GB_MemStreamRec));
	if (memstream)
	{
		memstream->pData = (GB_BYTE *)p;
		memstream->size = size;
		memstream->pos = 0;
	}
	return memstream;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Read_Mem
** Description: Read from memstream
** Input: f - IO handler
**        p - pointer
**        size - size
** Output: Read data from stream
** Return value: size
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Read_Mem(GB_IOHandler f, GB_BYTE * p, GB_INT32 size)
{
	GB_INT32		i;
	GB_MemStream	me = (GB_MemStream)f;

	if (size + me->pos > me->size)
		size = me->size - me->pos;
	for (i = 0;i < size; i++, me->pos++)
		p[i] = me->pData[me->pos];
	return size;
} 

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Write_Mem
** Description: Write to memstream
** Input: f - IO handler
**        p - pointer
**        size - size
** Output: Written data to stream
** Return value: size
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Write_Mem(GB_IOHandler f, GB_BYTE * p, GB_INT32 size)
{
	GB_INT32		i;
	GB_MemStream	me = (GB_MemStream)f;
	GB_BYTE *		data = me->pData;

	if (size + me->pos > me->size)
		size = me->size - me->pos;
	for (i = 0; i < size; i++, me->pos++)
		data[me->pos] = p[i];
	return size;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Seek_Mem
** Description: Seek in memstream
** Input: f - IO handler
**        pos - offset
** Output: Set posision in stream
** Return value: pos
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Seek_Mem(GB_IOHandler f, GB_INT32 pos)
{
	GB_MemStream	me = (GB_MemStream)f;

	if (pos >= me->size)
		return 0;
	me->pos = pos;
	return me->pos;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Close_Mem
** Description: Close memstream
** Input: f - IO handler
** Output: Closed memstream
** Return value: success/fail
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Close_Mem(GB_IOHandler f)
{
	GreyBit_Free_Sys(f);
	if (f != 0)
		return GB_FAILED;
	return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Stream_New
** Description: Initialize stream from file
** Input: pathname - path
**        bcreate - create?
** Output: New stream
** Return value: stream
** ---------------------------------------------------------------------------
*/

GB_Stream	GreyBit_Stream_New(const char * filepathname, char bcreate)
{
	GB_Stream		stream;
	GB_IOHandler	f;

	f = (GB_IOHandler)GreyBit_Open_Sys(filepathname, bcreate);
	if (!f)
		return 0;
	stream = (GB_Stream)GreyBit_Malloc_Sys(sizeof(GB_StreamRec));
	if (stream)
	{
		stream->parent = 0;
		stream->read = GreyBit_Read_Sys;
		stream->write = GreyBit_Write_Sys;
		stream->seek = GreyBit_Seek_Sys;
		stream->close = GreyBit_Close_Sys;
		stream->handler = f;
		stream->size = GreyBit_GetSize_Sys(stream->handler);
#ifdef ENABLE_ENCODER
		stream->pfilename = (char *)GreyBit_Malloc_Sys(GB_STRLEN(filepathname)
													   + 1);
		GB_STRCPY(stream->pfilename, filepathname);
#endif //ENABLE_ENCODER
		stream->offset = 0;
		stream->refcnt = 1;
	}
	return stream;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Stream_New_Memory
** Description: Initialize stream from memory
** Input: pBuf - buffer
**        nBufSize - size
** Output: New stream
** Return value: stream
** ---------------------------------------------------------------------------
*/

GB_Stream	GreyBit_Stream_New_Memory(const void * pBuf, GB_INT32 nBufSize)
{
	GB_Stream		stream; 
	GB_MemStream	f;

	f = GreyBit_Open_Mem(pBuf, nBufSize);
	if (!f)
		return 0;
	stream = (GB_Stream)GreyBit_Malloc_Sys(sizeof(GB_StreamRec));
	if (stream)
	{
		stream->parent = 0;
		stream->read = GreyBit_Read_Mem;
		stream->write = GreyBit_Write_Mem;
		stream->seek = GreyBit_Seek_Mem;
		stream->close = GreyBit_Close_Mem;
		stream->handler = f;
		stream->size = nBufSize;
#ifdef ENABLE_ENCODER
		stream->pfilename = 0;
#endif //ENABLE_ENCODER
		stream->offset = 0;
		stream->refcnt = 1;
	}
	return stream;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Stream_New_Child
** Description: Initialize child stream
** Input: parent - parent stream
** Output: New child stream
** Return value: stream
** ---------------------------------------------------------------------------
*/

GB_Stream    GreyBit_Stream_New_Child(GB_Stream parent)
{
	GB_Stream	stream;

	if (!parent)
		return 0;
	stream = (GB_Stream)GreyBit_Malloc_Sys(sizeof(GB_StreamRec));
	if (stream)
	{
		++parent->refcnt;
		stream->read = GreyBit_Read_Sys;
		stream->parent = parent;
		stream->write = GreyBit_Write_Sys;
		stream->seek = GreyBit_Seek_Sys;
		stream->close = GreyBit_Close_Sys;
		stream->handler = parent->handler;
		stream->size = parent->size;
		stream->offset = parent->offset;
		stream->refcnt = 1;
	}
	return stream;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Stream_Read
** Description: Read from stream
** Input: stream - stream
**        p - pointer
**        size - size
** Output: Read data from stream
** Return value: stream->read/success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Stream_Read(GB_Stream stream, GB_BYTE * p, GB_INT32 size)
{
	if (stream->read)
		return stream->read(stream->handler, p, size);
	else
		return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Stream_Write
** Description: Write to stream
** Input: stream - stream
**        p - pointer
**        size - size
** Output: Written data to stream
** Return value: stream->write/success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Stream_Write(GB_Stream stream, GB_BYTE * p, GB_INT32 size)
{
	if (stream->write)
		return stream->write(stream->handler, p, size);
	else
		return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Stream_Seek
** Description: Seek in stream
** Input: stream - stream
**        pos - offset
** Output: Set posision in stream
** Return value: stream->seek/success
** ---------------------------------------------------------------------------
*/

GB_INT32	GreyBit_Stream_Seek(GB_Stream stream, GB_INT32 pos)
{
	if (stream->seek)
		return stream->seek(stream->handler, stream->offset + pos);
	else
		return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Stream_Offset
** Description: Set offset stream
** Input: stream - stream
**        offset - offset
**        size - size
** Output: Offset in stream
** Return value: stream->seek/success
** ---------------------------------------------------------------------------
*/

GB_INT32     GreyBit_Stream_Offset(GB_Stream stream, GB_INT32 offset,
								   GB_INT32 size)
{
	if (stream->parent)
	{
		stream->offset = stream->parent->offset + offset;
		if (size)
			stream->size = size;
		else
			stream->size = stream->parent->size - stream->offset;
	}
	else
	{
		stream->offset = offset;
	}
	if (stream->seek)
		return stream->seek(stream->handler, stream->offset);
	else
		return GB_SUCCESS;
}

/*
** ---------------------------------------------------------------------------
** Function: GreyBit_Stream_Done
** Description: Done with stream? Nuke it!
** Input: stream - stream
** Output: nuked stream
** Return value: none
** ---------------------------------------------------------------------------
*/

void		GreyBit_Stream_Done(GB_Stream stream)
{
	stream->refcnt--;
	if (stream->refcnt <= 0)
	{
		if (stream->parent)
			GreyBit_Stream_Done(stream->parent);
		else
		{
			if (stream->close)
				stream->close(stream->handler);
#ifdef ENABLE_ENCODER
			if (stream->pfilename)
				GreyBit_Free_Sys(stream->pfilename);
#endif //ENABLE_ENCODER
		}
		GreyBit_Free_Sys(stream);
	}
}