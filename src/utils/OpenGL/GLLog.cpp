#include "GLLog.h"
#include <share.h>   // _SH_DENYNO
//global variable declarations
FILE *gpFile = NULL;

BOOL CreateLogFile(void)
{
	//code
	if (gpFile != NULL)
		return(FALSE);

	// Open with shared read-access so external tools (tail, VS Code preview,
	// PowerShell Get-Content -Wait) holding a read handle don't prevent the
	// app from truncating/creating Log.txt at startup.
	gpFile = _fsopen("Log.txt", "w", _SH_DENYNO);
	if (gpFile == NULL)
	{
		MessageBox(NULL, TEXT("Failed To Create Log File \"Log.txt\" !!! Exitting Now ..."), TEXT("LOG FILE ERROR"), MB_ICONERROR | MB_OK);
		return(FALSE);
	}

	else
	{
		fprintf(gpFile, "============================================================\n");
		fprintf(gpFile, "Log File \"Log.txt\" Has Been Created Successfully !!!\n");
		fprintf(gpFile, "============================================================\n\n");
	}
	return(TRUE);
}

void PrintGLInfo(void)
{
	// variable declarations
	GLint numExtensions, i;

	// code
	fprintf(gpFile, "\n");
	fprintf(gpFile, "OpenGL Related Information : \n");
	fprintf(gpFile, "===============================\n\n");
	fprintf(gpFile, "OpenGL Vendor   : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version  : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version    : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	fprintf(gpFile, "Number Of Supportrd Extensions = %d\n", numExtensions);

	for (i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}

	fprintf(gpFile, "===============================\n\n");
}

void PrintLogFunction(std::string callingFunc, const char *fmt, ...)
{
    va_list arg;
    int ret;

    // Ensure the log file is open
    if (gpFile == NULL)
        return;

    // Print the calling function name first
    fprintf(gpFile, "[%s]: ", callingFunc.c_str());

    // Handle the variadic arguments
    va_start(arg, fmt);
    ret = vfprintf(gpFile, fmt, arg);
    va_end(arg);
    fputc('\n', gpFile);
    fflush(gpFile);
}

void PrintLogString(std::string data)
{
    // Ensure the log file is open
    if (gpFile == NULL)
        return;

    // Write the log string to the file
    fprintf(gpFile, "%s\n", data.c_str());
}


void PrintLog(const char *fmt, ...)
{
	//variable declarations
	va_list arg;
	int ret;

	//code
	if (gpFile == NULL)
		return;
	va_start(arg, fmt);
	ret = vfprintf(gpFile, fmt, arg);
	va_end(arg);
	fflush(gpFile);
}

void CloseLogFile(void)
{
	//code
	if (gpFile == NULL)
		return;

	fprintf(gpFile, "============================================================\n");
	fprintf(gpFile, "Log File \"Log.txt\" Has Been Closed Successfully !!!\n");
	fprintf(gpFile, "============================================================\n");

	fclose(gpFile);
	gpFile = NULL;
}

