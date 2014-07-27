/***************************************************************************
        Statics defined in this module
***************************************************************************/

#ifndef _ttsTrace_DEFINITION_H
#define _ttsTrace_DEFINITION_H


#include "stdio.h"
#include "stdarg.h"
#include <assert.h>
#include <time.h>

#define g_szTraceFileName	"tts.dbg"
inline void ttsTrace(const char *szFormatString, ...);

enum TRACE_FLAG{
	tf_trace,
	tf_notrace
};

class CheckIfTraceFileExists
{
public:
	TRACE_FLAG gflag_Trace;
	FILE  *file;

	CheckIfTraceFileExists()
	{

		//Check if the file g_szTraceFileName exists
		  file = fopen (g_szTraceFileName, "r");
		  if (file != NULL)
			{
			fclose(file);
			if( file = fopen (g_szTraceFileName, "w"))
				gflag_Trace = tf_trace;
			else
				gflag_Trace = tf_notrace;
			}
		  else
			{
			gflag_Trace = tf_notrace;
			}

		  // Mark the start of this trace in the tracefile.	
		  ttsTrace ("---------------- START ------------------");
	};

	~CheckIfTraceFileExists()
	{
		ttsTrace ("----------------  END  ------------------");
		if (file)
		{
			fclose(file);
		}
	};
};

extern CheckIfTraceFileExists gTraceControl;
/*	Outputs debug information to a tracefile, named g_szTraceFileName, if one exists. */
inline void ttsTrace(const char *szFormatString, ...)
{
	if( gTraceControl.gflag_Trace == tf_trace)
	{
		if( !gTraceControl.file )
			return;

		// Print the format string, if one was given
		if (szFormatString != NULL)
		{
		    //get current time
		    time_t rawtime;
            struct tm * timeinfo;

            time ( &rawtime );
            timeinfo = localtime ( &rawtime );
            fprintf(gTraceControl.file, "%d/%d %d:%d:%d ", timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec );
            
			va_list argList;
			va_start (argList, szFormatString);
			vfprintf(gTraceControl.file, szFormatString, argList);
		}
		else
		{
			fprintf(gTraceControl.file, "NULL FORMAT STRING");
		}

		// Print a newline
		fprintf(gTraceControl.file, "\n");
		fflush(gTraceControl.file);
	}	
}
#endif
