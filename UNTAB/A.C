#include <OS2.h>
#include <stdlib.h>
#include <stdio.h>
#include <GnuArg.h>

#include <ctype.h>
#include <time.h>					  /* BP */
#include "frame.h"				  /* common to ALL modules in this
										   * program */
#include "povproto.h"

unsigned int Options;
int   Quality;
int   Case_Sensitive_Flag = CASE_SENSITIVE_DEFAULT;

FILE *bfp;

extern FRAME Frame;

char  Input_File_Name[FILE_NAME_LENGTH],
      Output_File_Name[FILE_NAME_LENGTH],
      Stat_File_Name[FILE_NAME_LENGTH];

#define MAX_LIBRARIES 10
char *Library_Paths[MAX_LIBRARIES];
int   Library_Path_Index;

int   Max_Symbols = 500;

FILE_HANDLE *Output_File_Handle;
int   File_Buffer_Size;
static int Number_Of_Files;
static int inflag,
      outflag;
DBL   VTemp;
DBL   Antialias_Threshold;
int   First_Line,
      Last_Line;
int   Display_Started = FALSE;
int   Shadow_Test_Flag = FALSE;

/* Stats kept by the ray tracer: */
long  Number_Of_Pixels,
      Number_Of_Rays,
      Number_Of_Pixels_Supersampled;
long  Ray_Sphere_Tests,
      Ray_Sphere_Tests_Succeeded;
long  Ray_Box_Tests,
      Ray_Box_Tests_Succeeded;
long  Ray_Blob_Tests,
      Ray_Blob_Tests_Succeeded;
long  Ray_Plane_Tests,
      Ray_Plane_Tests_Succeeded;
long  Ray_Triangle_Tests,
      Ray_Triangle_Tests_Succeeded;
long  Ray_Quadric_Tests,
      Ray_Quadric_Tests_Succeeded;
long  Ray_Poly_Tests,
      Ray_Poly_Tests_Succeeded;
long  Ray_Bicubic_Tests,
      Ray_Bicubic_Tests_Succeeded;
long  Ray_Ht_Field_Tests,
      Ray_Ht_Field_Tests_Succeeded;
long  Ray_Ht_Field_Box_Tests,
      Ray_HField_Box_Tests_Succeeded;
long  Bounding_Region_Tests,
      Bounding_Region_Tests_Succeeded;
long  Clipping_Region_Tests,
      Clipping_Region_Tests_Succeeded;
long  Calls_To_Noise,
      Calls_To_DNoise;
long  Shadow_Ray_Tests,
      Shadow_Rays_Succeeded;
long  Reflected_Rays_Traced,
      Refracted_Rays_Traced;
long  Transmitted_Rays_Traced;
time_t tstart,
      tstop;
DBL   tused;						  /* Trace timer variables. - BP */

char  DisplayFormat,
      OutputFormat,
      VerboseFormat,
      PaletteOption,
      Color_Bits;







void  init_vars (void)
{
	Output_File_Handle = NULL;
	File_Buffer_Size = 0;
	Options = 0;
   Quality = 9;
	Number_Of_Files = 0;
	First_Line = 0;
	Last_Line = -1;
	Color_Bits = 8;

	Number_Of_Pixels = 0L;
	Number_Of_Rays = 0L;
	Number_Of_Pixels_Supersampled = 0L;
	Ray_Ht_Field_Tests = 0L;
	Ray_Ht_Field_Tests_Succeeded = 0L;
	Ray_Ht_Field_Box_Tests = 0L;
	Ray_HField_Box_Tests_Succeeded = 0L;
	Ray_Bicubic_Tests = 0L;
	Ray_Bicubic_Tests_Succeeded = 0L;
	Ray_Blob_Tests = 0L;
	Ray_Blob_Tests_Succeeded = 0L;
	Ray_Box_Tests = 0L;
	Ray_Box_Tests_Succeeded = 0L;
	Ray_Sphere_Tests = 0L;
	Ray_Sphere_Tests_Succeeded = 0L;
	Ray_Plane_Tests = 0L;
	Ray_Plane_Tests_Succeeded = 0L;
	Ray_Triangle_Tests = 0L;
	Ray_Triangle_Tests_Succeeded = 0L;
	Ray_Quadric_Tests = 0L;
	Ray_Quadric_Tests_Succeeded = 0L;
	Ray_Poly_Tests = 0L;
	Ray_Poly_Tests_Succeeded = 0L;
	Bounding_Region_Tests = 0L;
	Bounding_Region_Tests_Succeeded = 0L;
	Clipping_Region_Tests = 0L;
	Clipping_Region_Tests_Succeeded = 0L;
	Calls_To_Noise = 0L;
	Calls_To_DNoise = 0L;
	Shadow_Ray_Tests = 0L;
	Shadow_Rays_Succeeded = 0L;
	Reflected_Rays_Traced = 0L;
	Refracted_Rays_Traced = 0L;
	Transmitted_Rays_Traced = 0L;

	Frame.Screen_Height = 200;
	Frame.Screen_Width = 320;
	Antialias_Threshold = 0.3;

	Library_Paths[0] = NULL;
	Library_Path_Index = 0;
	*Output_File_Name = '\0';
	*Input_File_Name = '\0';

	return;
}



void  Print_Options ()
{
	int   i;

	fprintf (stdout, "\nPOV-Ray	Options in effect: ");

	if (Options & CONTINUE_TRACE)
		fprintf (stdout, "+c ");

	if (Options & DISPLAY)
		fprintf (stdout, "+d%c%c ", DisplayFormat, PaletteOption);

	if (Options & VERBOSE)
		fprintf (stdout, "+v%c ", VerboseFormat);

	if (Options & VERBOSE_FILE)
		fprintf (stdout, "+@%s ", Stat_File_Name);

	if (Options & DISKWRITE)
		fprintf (stdout, "+f%c ", OutputFormat);

	if (Options & PROMPTEXIT)
		fprintf (stdout, "+p ");

	if (Options & EXITENABLE)
		fprintf (stdout, "+x ");

	if (Options & ANTIALIAS)
		fprintf (stdout, "+a%f ", Antialias_Threshold);

	if (Options & DEBUGGING)
		fprintf (stdout, "+z ");

	if (File_Buffer_Size != 0)
		fprintf (stdout, "-b%d ", File_Buffer_Size / 1024);

	fprintf (stdout, "-q%d -w%d -h%d -s%d -e%d\n-i%s ",
				Quality, Frame.Screen_Width, Frame.Screen_Height,
				First_Line, Last_Line, Input_File_Name);

	if (Options & DISKWRITE)
		fprintf (stdout, "-o%s ", Output_File_Name);

	for (i = 0; i < Library_Path_Index; i++)
		fprintf (stdout, "-l%s ", Library_Paths[i]);

	fprintf (stdout, "\n");
}




void  print_stats ()
{
	int   hours,
	      min;
	DBL   sec;
	FILE *stat_out;
	long  Pixels_In_Image;

	if (Options & VERBOSE_FILE)
		stat_out = fopen (Stat_File_Name, "w+t");
	else
		stat_out = stdout;

	Pixels_In_Image = (long) Frame.Screen_Width * (long) Frame.Screen_Height;


	fprintf (stat_out, "\n%s statistics\n", Input_File_Name);
	if (Pixels_In_Image > Number_Of_Pixels)
		fprintf (stat_out, "  Partial Image Rendered");

	fprintf (stat_out, "--------------------------------------\n");
	fprintf (stat_out, "Resolution %d x %d\n", Frame.Screen_Width, Frame.Screen_Height);
	fprintf (stat_out, "# Rays:  %10ld    # Pixels:  %10ld  # Pixels supersampled: %10ld\n",
	Number_Of_Rays, Number_Of_Pixels, Number_Of_Pixels_Supersampled);

	fprintf (stat_out, "  Ray->Shape Intersection Tests:\n");
	fprintf (stat_out, "   Type             Tests    Succeeded   Percentage\n");
	fprintf (stat_out, "  -----------------------------------------------------------\n");
	if (Ray_Sphere_Tests)
		fprintf (stat_out, "  Sphere       %10ld  %10ld  %10.2f\n", Ray_Sphere_Tests, Ray_Sphere_Tests_Succeeded, (((DBL) Ray_Sphere_Tests_Succeeded / (DBL) Ray_Sphere_Tests) * 100.0));
	if (Ray_Plane_Tests)
		fprintf (stat_out, "  Plane        %10ld  %10ld  %10.2f\n", Ray_Plane_Tests, Ray_Plane_Tests_Succeeded, (((DBL) Ray_Plane_Tests_Succeeded / (DBL) Ray_Plane_Tests) * 100.0));
	if (Ray_Triangle_Tests)
		fprintf (stat_out, "  Triangle     %10ld  %10ld  %10.2f\n", Ray_Triangle_Tests, Ray_Triangle_Tests_Succeeded, (((DBL) Ray_Triangle_Tests_Succeeded / (DBL) Ray_Triangle_Tests) * 100.0));
	if (Ray_Quadric_Tests)
		fprintf (stat_out, "  Quadric      %10ld  %10ld  %10.2f\n", Ray_Quadric_Tests, Ray_Quadric_Tests_Succeeded, (((DBL) Ray_Quadric_Tests_Succeeded / (DBL) Ray_Quadric_Tests) * 100.0));
	if (Ray_Blob_Tests)
		fprintf (stat_out, "  Blob         %10ld  %10ld  %10.2f\n", Ray_Blob_Tests, Ray_Blob_Tests_Succeeded, (((DBL) Ray_Blob_Tests_Succeeded / (DBL) Ray_Blob_Tests) * 100.0));
	if (Ray_Box_Tests)
		fprintf (stat_out, "  Box          %10ld  %10ld  %10.2f\n", Ray_Box_Tests, Ray_Box_Tests_Succeeded, (((DBL) Ray_Box_Tests_Succeeded / (DBL) Ray_Box_Tests) * 100.0));
	if (Ray_Poly_Tests)
		fprintf (stat_out, "  Quartic\\Poly %10ld  %10ld  %10.2f\n", Ray_Poly_Tests, Ray_Poly_Tests_Succeeded, (((DBL) Ray_Poly_Tests_Succeeded / (DBL) Ray_Poly_Tests) * 100.0));
	if (Ray_Bicubic_Tests)
		fprintf (stat_out, "  Bezier Patch %10ld  %10ld  %10.2f\n", Ray_Bicubic_Tests, Ray_Bicubic_Tests_Succeeded, (((DBL) Ray_Bicubic_Tests_Succeeded / (DBL) Ray_Bicubic_Tests) * 100.0));
	if (Ray_Ht_Field_Tests)
		fprintf (stat_out, "  Height Fld   %10ld  %10ld  %10.2f\n", Ray_Ht_Field_Tests, Ray_Ht_Field_Tests_Succeeded, (((DBL) Ray_Ht_Field_Tests_Succeeded / (DBL) Ray_Ht_Field_Tests) * 100.0));
	if (Ray_Ht_Field_Box_Tests)
		fprintf (stat_out, "  Hght Fld Box %10ld  %10ld  %10.2f\n", Ray_Ht_Field_Box_Tests, Ray_HField_Box_Tests_Succeeded, (((DBL) Ray_HField_Box_Tests_Succeeded / (DBL) Ray_Ht_Field_Box_Tests) * 100.0));
	if (Bounding_Region_Tests)
		fprintf (stat_out, "  Bounds       %10ld  %10ld  %10.2f\n", Bounding_Region_Tests, Bounding_Region_Tests_Succeeded, (((DBL) Bounding_Region_Tests_Succeeded / (DBL) Bounding_Region_Tests) * 100.0));
	if (Clipping_Region_Tests)
		fprintf (stat_out, "  Clips        %10ld  %10ld  %10.2f\n", Clipping_Region_Tests, Clipping_Region_Tests_Succeeded, (((DBL) Clipping_Region_Tests_Succeeded / (DBL) Clipping_Region_Tests) * 100.0));

	if (Calls_To_Noise)

		fprintf (stat_out, "  Calls to Noise:   %10ld\n", Calls_To_Noise);
	if (Calls_To_DNoise)
		fprintf (stat_out, "  Calls to DNoise:  %10ld\n", Calls_To_DNoise);
	if (Shadow_Ray_Tests)
		fprintf (stat_out, "  Shadow Ray Tests: %10ld     Blocking Objects Found:  %10ld\n",
					Shadow_Ray_Tests, Shadow_Rays_Succeeded);
	if (Reflected_Rays_Traced)
		fprintf (stat_out, "  Reflected Rays:   %10ld\n", Reflected_Rays_Traced);
	if (Refracted_Rays_Traced)
		fprintf (stat_out, "  Refracted Rays:   %10ld\n", Refracted_Rays_Traced);
	if (Transmitted_Rays_Traced)
		fprintf (stat_out, "  Transmitted Rays: %10ld\n", Transmitted_Rays_Traced);

	if (tused == 0)
	{
		STOP_TIME					  /* Get trace done time. */
			tused = TIME_ELAPSED	  /* Calc. elapsed time. Define
										   * TIME_ELAPSED as */
		/* 0 in your specific CONFIG.H if unsupported */
	}
	if (tused != 0)
	{
		/* Convert seconds to hours, min & sec. CdW */
		hours = (int) tused / 3600;
		min = (int) (tused - hours * 3600) / 60;
		sec = tused - (DBL) (hours * 3600 + min * 60);
		fprintf (stat_out, "  Time For Trace:   %2d hours %2d minutes %4.2f seconds\n", hours, min, sec);
	}
	if (Options & VERBOSE_FILE)
		fclose (stat_out);

}

/* Find a file in the search path. */

FILE *Locate_File (filename, mode)
char *filename,
     *mode;
{
	FILE *f;
	int   i;
	char  pathname[FILE_NAME_LENGTH];

	/* Check the current directory first. */
	if ((f = fopen (filename, mode)) != NULL)
		return (f);

	for (i = 0; i < Library_Path_Index; i++)
	{
		strcpy (pathname, Library_Paths[i]);
		if (FILENAME_SEPARATOR != NULL)
			strcat (pathname, FILENAME_SEPARATOR);
		strcat (pathname, filename);
		if ((f = fopen (pathname, mode)) != NULL)
			return (f);
	}

	return (NULL);
}
void  print_credits ()
{
	fprintf (stderr, "\n");
	fprintf (stderr, "  Persistence of Vision Raytracer Ver 1.0%s\n", COMPILER_VER);
	fprintf (stderr, "  Copyright 1992 POV-Team\n");
	fprintf (stderr, "  ----------------------------------------------------------------------\n");
	fprintf (stderr, "  POV-Ray is based on DKBTrace 2.12 by David K. Buck & Aaron A. Collins.\n");
	fprintf (stderr, "  \n");
	fprintf (stderr, "  Contributing Authors: (Alphabetically)\n");
	fprintf (stderr, "  \n");
	fprintf (stderr, "  Steve A. Bennett   David K. Buck      Aaron A. Collins\n");
	fprintf (stderr, "  Alexander Enzmann  Dan Farmer         Girish T. Hagan\n");
	fprintf (stderr, "  Douglas Muir       Bill Pulver        Robert Skinner\n");
	fprintf (stderr, "  Scott Taylor       Drew Wells         Chris Young\n");
	fprintf (stderr, "  ----------------------------------------------------------------------\n");
	fprintf (stderr, "  Other contributors listed in the documentation.\n");
}


































void  Err (PSZ psz1, PSZ psz2)
{
	fcloseall ();
	printf (psz1, psz2);
	putchar ('\n');
	exit (1);
}


void  Usage (void)
{
	printf ("POV Ray ray tracer   (C) someone else\n\n");
	printf ("USAGE:  POVRAY [options] InFile [OutFile]\n\n");
	printf ("WHERE:  Options are 0 or more of the following:\n\n");
	printf ("   Parameter       Default Description       Range\n");
	printf ("   ---------------------------------------------------------------------\n");
	printf ("   /Width:# ..........320  Image Width.      (1-4096)\n");
	printf ("   /Height:# .........200  Image Height.     (1-4096)\n");
	printf ("   /Format:Fmat ......T    Image Format.     (Targa, Dump, Raw, or None)\n");
	printf ("   /Start:# ..........0    Start Trace Line. \n");
	printf ("   /End:# ............end  End Trace Line.\n");
	printf ("   /Continue .........no   Continue aborted trace.\n");
	printf ("   /Pause ............no   Pause when done.  (default=exit when done)\n");
	printf ("   /Buffer:# .........none Output Buffer Size in K.  (0=flush every line)\n");
	printf ("   /Alias:# ..........0.3  Anti-aliasing.    (a:1.0 means no antialiasing)\n");
	printf ("   /Quality:# ........9    Image Quality.    (0=fastest, 9=best quality)\n");
	printf ("   /Library:Path .....none Includes Path.    (defailt is current dir)\n");
	printf ("   /Statistics:# .....none Display Stats while rendering.  (1 or 2)\n");
	printf ("   /Graph ............no   Display Image while rendering.\n");
	printf ("   /Display:Hex ......0    Set Display Type. (Hex Digit, 0=Autodetect)\n");
	printf ("   /Palette:PalChar ..3    Set Palette Type. (3,h,0,G,T)\n\n");
	exit (0);
}



void  ParseOptions (void)
   {
	PSZ   p;
	USHORT zz;

	/*--- Digest Cmd Line and Env Variables ---*/
	if (!IsArg (NULL))
		Err ("No source file specified", "");
	strncpy (Input_File_Name, GetArg (NULL, 0), FILE_NAME_LENGTH);

	if (IsArg (NULL) > 1)
		strncpy (Output_File_Name, GetArg (NULL, 1), FILE_NAME_LENGTH);
	else
	   {
		strncpy (Output_File_Name, Input_File_Name, FILE_NAME_LENGTH);
		if (p = strrchr (Output_File_Name, '.'))
			*p = '\0';

		if (IsArg ("Width"))
			if (!(Frame.Screen_Width = atoi (GetArg ("Width", 0))))
				Err ("Bad Width Value", "");

		if (IsArg ("Height"))
			if (!(Frame.Screen_Height = atoi (GetArg ("Height", 0))))
				Err ("Bad Height Value: %s", GetArg ("Height", 0));

		if (IsArg ("Format"))
		{
			OutputFormat = tolower (*GetArg ("Format", 0));
			if (!strchr ("tdrn", OutputFormat))
				Err ("Bad File Type: %s", GetArg ("Format", 0));

			if (!IsArg (NULL) > 1)  /*---generate outfile name? ---*/
				strcat (Output_File_Name,
						  (OutputFormat == 't' ? ".Tga" :
							(OutputFormat == 'd' ? ".Dmp" :
							 (OutputFormat == 'r' ? ".Raw" : ""))));

			Options |= (OutputFormat == 'n' ? ~DISKWRITE : DISKWRITE);
		}

		if (IsArg ("Start"))
			First_Line = atoi (GetArg ("Start", 0));

		if (IsArg ("End"))
			if (!(Last_Line = atoi (GetArg ("End", 0))))
				Err ("Bad End Value: %s", GetArg ("End", 0));

		if (IsArg ("Continue"))
			Options |= CONTINUE_TRACE;

		if (IsArg ("Pause"))
			Options |= PROMPTEXIT;

		if (IsArg ("Buffer"))
			File_Buffer_Size = max (BUFSIZE, atoi (GetArg ("Buffer", 0)) * 1024);

		if (IsArg ("Alias"))
		{
			Antialias_Threshold = atof (GetArg ("Alias", 0));
			Options |= (Antialias_Threshold < 1 ? ANTIALIAS : ~ANTIALIAS);
		}

		if (IsArg ("Quality"))
			Quality = atoi (GetArg ("Quality", 0));

		if (IsArg ("Library"))
			while (IsArg ("Library") > zz)
			{
				if (Library_Path_Index >= MAX_LIBRARIES)
					Err ("Too many library directories specified.", "");
				Library_Paths[Library_Path_Index] = strdup (GetArg ("Library", zz));
				Library_Path_Index++;
				zz++;
			}

		if (IsArg ("Statistics"))
		{
			VerboseFormat = (char) toupper (*GetArg ("Statistics", 0));
			if (VerboseFormat == '\0')
				VerboseFormat = '1';
			Options |= VERBOSE;
		}

		if (IsArg ("Graph"))
		{
			Options |= DISPLAY;
			DisplayFormat = '0';
			PaletteOption = '3';
		}

		if (IsArg ("Display"))
			DisplayFormat = (char) toupper (*GetArg ("Display", 0));

		if (IsArg ("Palette"))
			DisplayFormat = (char) toupper (*GetArg ("Palette", 0));

		if (IsArg ("X"))
			Options |= EXITENABLE;

		if (IsArg ("T"))
			switch (toupper (*GetArg ("T", 0)))
			{
				case 'Y':
					Case_Sensitive_Flag = 0;
					break;
				case 'N':
					Case_Sensitive_Flag = 1;
					break;
				case 'O':
					Case_Sensitive_Flag = 2;
					break;
				default:
					Case_Sensitive_Flag = 2;
					break;
			}

		if (IsArg ("Max"))
			Max_Symbols = atoi (GetArg ("Max", 0));

		if (IsArg ("Z"))
			Options |= DEBUGGING;


		if (Options & DISKWRITE)
		{
			switch (OutputFormat)
			{
				case 't':
					Output_File_Handle = Get_Targa_File_Handle ();
					break;
				case 'd':
					Output_File_Handle = Get_Dump_File_Handle ();
					break;
				case 'r':
					Output_File_Handle = Get_Raw_File_Handle ();
					break;
				case 'n':
					Output_File_Handle = NULL;
					break;
			}
			if (!Output_File_Handle && OutputFormat != 'n')
				Err ("Unable to open output file: %s", Output_File_Name);
		}

		/*--- Post process parameter data ---*/
		if (Last_Line == -1)
			Last_Line = Frame.Screen_Height;
	}



	main (int argc, char *argv[])
	{
	FILE *stat_file;

	STARTUP_POVRAY

	PRINT_CREDITS

	PRINT_OTHER_CREDITS

	/*--- Define Valid Arguments ---*/
	BuildArgBlk (" *^Width%   *^Height%"
						" *^Format%  *^Start%"
						" *^End%     *^Continue"
						" *^Pause    *^Buffer%"
						" *^Alias%   *^Quality%"
						" *^Library% *^Statistics"
						" *^Graph    *^Display%"
						" *^Palette% *^Help ?"

						" *^X        *^T%"
						" *^Max%     *^Z");


	/*--- Eat Cmd Line and Env Variables ---*/
	if (FillArgBlk2 (getenv ("POVRAYOPT")))
		return printf ("POVRAYOPT: %s", GetArgErr ());
	if (FillArgBlk2 (getenv ("POVOPT")))
		return printf ("POVOPT: %s", GetArgErr ());
	if (FillArgBlk (argv))
		return printf ("POVOPT: %s", GetArgErr ());

	if (IsArg ("?") || IsArg ("Help") || argc < 2)
		Usage ();

	init_vars ();

	ParseOptions ();

	Print_Options ();

	Initialize_Tokenizer (Input_File_Name);
	fprintf (stderr, "Parsing...");
	if (Options & VERBOSE_FILE)
   	{
		stat_file = fopen (Stat_File_Name, "w+t");
		fprintf (stat_file, "Parsing...\n");
		fclose (stat_file);
	   }
	Parse (&Frame);
	Terminate_Tokenizer ();

	if (Options & DISPLAY)
   	{
		printf ("Displaying...\n");
		display_init (Frame.Screen_Width, Frame.Screen_Height);
		Display_Started = TRUE;
   	}

	/* Get things ready for ray tracing */
	if (Options & DISKWRITE)
		if (Options & CONTINUE_TRACE)
	   	{
			if (Open_File (Output_File_Handle, Output_File_Name,
								&Frame.Screen_Width, &Frame.Screen_Height,
								File_Buffer_Size, READ_MODE) != 1)
		   	{
				fprintf (stderr, "Error opening continue trace output file\n");
				fprintf (stderr, "Opening new output file %s.\n", Output_File_Name);
				Options &= ~CONTINUE_TRACE;	/* Turn off continue
															* trace */

				if (Open_File (Output_File_Handle, Output_File_Name,
								&Frame.Screen_Width, &Frame.Screen_Height,
									File_Buffer_Size, WRITE_MODE) != 1)
			   	{
					fprintf (stderr, "Error opening output file\n");
					close_all ();
					exit (1);
				   }
   			}

			Initialize_Renderer ();
			if (Options & CONTINUE_TRACE)
				Read_Rendered_Part ();
	   	}
		else
		   {
			if (Open_File (Output_File_Handle, Output_File_Name,
								&Frame.Screen_Width, &Frame.Screen_Height,
								File_Buffer_Size, WRITE_MODE) != 1)
			   {
				fprintf (stderr, "Error opening output file\n");
				close_all ();
				exit (1);
   			}
			Initialize_Renderer ();
	   	}
	else
		Initialize_Renderer ();

	pq_init ();
	Initialize_Noise ();

	START_TIME					  /* Store start time for trace. Timer
										* macro in CONFIG.H */

	/* Ok, go for it - trace the picture */
		if ((Options & VERBOSE) && (VerboseFormat != '1'))
		printf ("Rendering...\n");
	else if ((Options & VERBOSE) && (VerboseFormat == '1'))
		fprintf (stderr, "POV-Ray rendering %s to %s :\n", Input_File_Name, Output_File_Name);
	if (Options & VERBOSE_FILE)
   	{
		stat_file = fopen (Stat_File_Name, "w+t");
		fprintf (stat_file, "Parsed ok. Now rendering %s to %s :\n", Input_File_Name, Output_File_Name);
		fclose (stat_file);
   	}

	CONFIG_MATH					  /* Macro for setting up any special
										* FP options */
	Start_Tracing ();

	if (Options & VERBOSE && VerboseFormat == '1')
		fprintf (stderr, "\n");

	STOP_TIME   			  /* Get trace done time. */
	tused = TIME_ELAPSED	  /* Calc. elapsed time. Define */
   						     /* TIME_ELAPSED as */

	/* Clean up and leave */
	display_finished ();

	close_all ();

	PRINT_STATS

		if (Options & VERBOSE_FILE)
   	{
		stat_file = fopen (Stat_File_Name, "a+t");
		fprintf (stat_file, "Done Tracing\n");
		fclose (stat_file);
	   }

	FINISH_POVRAY

	// DumpArgs ();
	return 0;
	}
