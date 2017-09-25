/*
 * Worldvisions Weaver Software:
 *   Copyright (C) 1997-2002 Net Integration Technologies, Inc.
 * 
 * A class for managing error numbers and strings.  See wverror.h.
 */
#include "wverror.h"
#include <assert.h>

WvErrorBase::~WvErrorBase()
{
    // nothing special
}


// win32's strerror() function is incredibly weak, so we'll provide a better
// one.
WvString WvErrorBase::strerror(int errnum)
{
    assert(errnum >= 0);

    return ::strerror(errnum);
}


WvString WvErrorBase::errstr() const
{
    int errnum = geterr();
    
    if (errnum < 0)
    {
	assert(!!errstring);
	return errstring;
    }
    else
    {
	if (!!errstring) return errstring;
	return WvErrorBase::strerror(errnum);
    }
}


void WvErrorBase::seterr(int _errnum)
{
    if (!errnum)
    {
        assert((_errnum != -1 || !!errstring)
        && "attempt to set errnum to -1 without also setting errstring");
        errnum = _errnum;
    }
}


void WvErrorBase::seterr(WvStringParm specialerr)
{
    assert(!!specialerr);
    if (!errnum)
    {
	errstring = specialerr;
	seterr(-1);
    }
}


void WvErrorBase::seterr(const WvErrorBase &err)
{
    if (err.geterr() > 0)
	seterr(err.geterr());
    else if (err.geterr() < 0)
	seterr(err.errstr());
}


void WvErrorBase::seterr_both(int _errnum, WvStringParm specialerr)
{
    assert(!!specialerr);
    if (!errnum)
    {
	errstring = specialerr;
	seterr(_errnum);
    }
}
