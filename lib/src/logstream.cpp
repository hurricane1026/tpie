// Copyright (c) 1994 Darren Erik Vengroff
//
// File: logstream.cpp
// Author: Darren Erik Vengroff <dev@cs.duke.edu>
// Created: 5/12/94
//

// This will go into a library whne it is ready.


static char logstream_id[] = "$Id: logstream.cpp,v 1.6 1999-04-09 19:24:35 rajiv Exp $";


#include <logstream.h>


// Contructor

logstream::logstream(const char *fname, 
		     unsigned int p,
		     unsigned int tp) 
: ofstream(fname), priority(p), threshold(tp) {} 
  

// Output operators.

// A macro to define a log stream output operator for a given type.
// The type can be any type that has an ofstream output operator.
#define _DEFINE_LOGSTREAM_OUTPUT_OPERATOR(T)	\
logstream& logstream::operator<<(const T x)		\
{												\
    if (priority <= threshold) {				\
	ofstream::operator<<(x)<<endl;				\
    }											\
    return *this;								\
}


logstream& logstream::operator<<(const char *x)
{									
    if (priority <= threshold) {					
	ofstream::operator<<(x);					
    } 									
    return *this;							
}


//_DEFINE_LOGSTREAM_OUTPUT_OPERATOR(char *)
_DEFINE_LOGSTREAM_OUTPUT_OPERATOR(char)
_DEFINE_LOGSTREAM_OUTPUT_OPERATOR(int)
_DEFINE_LOGSTREAM_OUTPUT_OPERATOR(unsigned int)
_DEFINE_LOGSTREAM_OUTPUT_OPERATOR(long int)
_DEFINE_LOGSTREAM_OUTPUT_OPERATOR(long unsigned int)
_DEFINE_LOGSTREAM_OUTPUT_OPERATOR(float)
_DEFINE_LOGSTREAM_OUTPUT_OPERATOR(double)
#ifdef _BSD_OFF_T_
  _DEFINE_LOGSTREAM_OUTPUT_OPERATOR(off_t);
#endif


// Setting priority and threshold on the fly with manipulators.

logstream& manip_priority(logstream& tpl, unsigned int p)
{
    tpl.priority = p;
    return tpl;
}

logmanip<unsigned int> setpriority(unsigned int p)
{
   return logmanip<unsigned int>(&manip_priority, p);
} 

logstream& manip_threshold(logstream& tpl, unsigned int p)
{
    tpl.threshold = p;
    return tpl;
}

logmanip<unsigned int> setthreshold(unsigned int p)
{
   return logmanip<unsigned int>(&manip_threshold, p);
} 
