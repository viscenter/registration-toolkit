/***************************************************************************************************
*                                                                                                  *
*                                          EDUCE PROJECT                                           *
*                                                                                                  *
****************************************************************************************************
****************************************************************************************************
*                                                                                                  *


*                                  Last Modified: July 14, 2004                                    *


*                                                                                                  *
*                                                                                                  *
****************************************************************************************************
****************************************************************************************************
*                                                                                                  *
*                                       Author: Daniel G Olson                                     *
*                                        Email: dgo@uky.edu                                        *
*                                      Created: July 14, 2004                                      *
*                                                                                                  *
****************************************************************************************************
****************************************************************************************************
*                                                                                                  *
*   Filename: "educe.java"                                                                         *
*   Java Class: educe                                                                              *
*                                                                                                  *
*   Purpose:    This class parses arguments, from command line and file.  It checks the presence   *
*            and validity of all needed files.  It creates and passes all valid arguments to the   *
*            EduceProject.  After which this class is only used to write output information to     *
*            the desired places.                                                                   *
*                                                                                                  *
*   ***** NOTE *****                                                                               *
*      - Arguments are handle in order. ( descending into file then continuing )  Thus arguments   *
*      can be overruled based on their location.                                                   *
*      - Arguments to parse a argument file located insided an argument file will be ignored.      *
*      - Format of an argument file is as follows:                                                 *
*         EDUCE_ARGUMENT_FILE                                                                      *
*         -argument1                                                                               *
*         -argument2 variable1 variable2 ...                                                       *
*         -argument3 variable1 variable2 ...                                                       *
*         ...                                                                                      *
*         EDUCE_ARGUMENT_END_OF_FILE                                                               *
*      - Arguments after a save arguments argument, will be saved.                                 *
*                                                                                                  *
*   PUBLIC VARIABLES:   (NONE)                                                                     *
*                                                                                                  *
*   PROTECTED VARIABLES:   (NONE)                                                                  *
*                                                                                                  *
*   PRIVATE VARIABLES:                                                                             *
*      boolean:                                                                                    *
*         firstParsingPass - is this the first parse pass. (true)                                  *
*         printErrorMessages - are error messages printed. (false)                                 *
*         printVerboseMessages - are verbose messages printed. (false)                             *
*         saveArguments - does the user want to save the arguments to a file. (false)              *
*      FileWriter:                                                                                 *
*         argumentFile - output file for writing arguments to a file.                              * 
*      int:                                                                                        *
*         windowWidth - used to define the width of the window that is created.                    *
*         windowHeight - used to define the height of the window that is created.                  *
*      PrintStream:                                                                                *
*         verboseMessages - output stream for verbose messages.                                    *
*         errorMessages - output stream for error messages.                                        *
*                                                                                                  *
*   CONSTRUCTOR FUNTIONS:                                                                          *
*      educe( String[] )                                                                           *
*         - Parses arguments, checks file validity, start main program.                            *
*                                                                                                  *
*   PUBLIC FUNTIONS:                                                                               *
*      void println( String, boolean )                                                             *
*         - Prints a message to either errorMessages or verboseMessages.                           *
*      void Quit()                                                                                 *
*         - Closes print streams and exits the program.                                            *
*                                                                                                  *
*   PRIVATE FUNTIONS:                                                                              *
*      boolean CheckExistanceOfFile( String )                                                      *
*         - Checks to see if the given file exists.                                                *
*      void ParseArgument( String )                                                                *
*         - Parses one argument.                                                                   *
*         - firstParsingPass dicates which arguments get skipped.                                  *
*      void ParseArguments( String[] )                                                             *
*         if( firstParsingPass == true )                                                           *
*            - Checks to see if needed file are valid.                                             *
*            - If argument file, checks for window information.                                    *
*            - Opens output print streams if requested.                                            *
*            - Checks arguments for window size information and debug information.                 *
*         else                                                                                     *
*            - Parses project related arguments in command line and argument files.                *
*      void ParseFile( String )                                                                    *
*         - Parses argument file.                                                                  *
*                                                                                                  *
***************************************************************************************************/

import java.io.*;

public class educe
{
	//private EduceProject educeProject;
	private boolean firstParsingPass, printErrorMessages, printVerboseMessages, saveArguments;
	private FileWriter argumentFile;
	private int windowWidth, windowHeight;
	private PrintStream errorMessages, verboseMessages;
	
	public educe( String arguments[] )
	{
		// Defualts
		firstParsingPass = true;
		printErrorMessages = false;
		printVerboseMessages = false;
		saveArguments = false;
		windowWidth = 1024;
		windowHeight = 768;
		verboseMessages = System.out;
		errorMessages = System.out;
		
		// Parsing Pass for non-EduceProject arguments
		ParseArguments( arguments );
		firstParsingPass = false;
		
		// Create Educe Project Frame
		println( "Creating EDUCE Window.", false );
		EduceProject educeProject = new EduceProject( windowWidth, windowHeight, this );
		
		// Parse and send arguments to educeProject
		println( "Parsing Arguments.", false );
		ParseArguments( arguments );
		
		// Close output file if open
		if( saveArguments )
		{
			try
			{
				argumentFile.close();
			}
			catch( IOException exception ){}
			saveArguments = false;
		}
	} // END OF - public educe( String[] )
	
	public void println( String message, boolean error )
	{
		if( error )
		{
			if( printErrorMessages ) errorMessages.println( message );
		}
		else
		{
			if( printVerboseMessages ) verboseMessages.println( message );
		}
	} // END OF - public void println( String, boolean )
	
	public void Quit()
	{
		println( "EDUCE Program - Done.", false );
		if( verboseMessages != System.out ) verboseMessages.close();
		if( errorMessages != System.out ) errorMessages.close();
		if( saveArguments )
		{
			try
			{
				argumentFile.close();
			}
			catch( IOException exception ){}
		}
		
		System.exit(0);
	} // END OF - public void Quit()
	
	
	
	private boolean CheckExistanceOfFile( String filename )
	{
		
		
		
		return true;
	} // END OF - private boolean CheckExistanceOfFile( String )
	
	private void ParseArgument( String argument )
	{
		if( firstParsingPass )
		{
			//  Handle input/output plus window information
			if( argument.indexOf( "-arg " ) >= 0 )
			{
				int index1 = argument.indexOf( " " ) + 1;
				int index2 = argument.indexOf( " ", index1 );
				
				ParseFile( argument.substring( index1, index2 ) );
			}
			else if( argument.indexOf( "-saveArg " ) >= 0 )
			{
				int index1 = argument.indexOf( " " ) + 1;
				int index2 = argument.indexOf( " ", index1 );
				
				if( saveArguments )
				{
					try
					{
						argumentFile.close();
					}
					catch( IOException exception ){}
				}
				String filename = argument.substring( index1, index2 );
				
				boolean printError = false;
				try
				{
					File file = new File( filename );
					if( !file.exists() )
					{
						argumentFile = new FileWriter( file );
						saveArguments = true;
					}
				}
				catch( FileNotFoundException exception ){ printError = true; }
				catch( SecurityException exception ){ printError = true; }
				catch( IOException exception ){ printError = true; }
				
				if( printError )
				{
					println( "Unable to create: \"" + filename + "\" for saving arguments.", true );
				}
			}
			else if( argument.indexOf( "-e" ) >= 0 )
			{
				if( argument.indexOf( "-e " ) >= 0 || argument.indexOf( "-errors " ) >= 0 )
				{
					printErrorMessages = true;
				}
			}
			else if( argument.indexOf( "-E" ) >= 0 )
			{
				if( argument.indexOf( "-E " ) >= 0 || argument.indexOf( "-Errors " ) >= 0 )
				{
					int index1 = argument.indexOf( " " ) + 1;
					int index2 = argument.indexOf( " ", index1 );
					
					String filename = argument.substring( index1, index2 );
					boolean printError = true;
					try
					{
						printErrorMessages = true;
						File file = new File( filename );
						if( !file.exists() )
						{
							errorMessages = new PrintStream( new FileOutputStream( file ) );
						}
						else printError = true;
					}
					catch( FileNotFoundException exception ){ printError = true; }
					catch( SecurityException exception ){ printError = true; }
					
					if( printError )
					{
						println( "Unable to create: \"" + filename + "\" for outputing error messages.", true );
					}
				}
			}
			else if( argument.indexOf( "-v" ) >= 0 )
			{
				if( argument.indexOf( "-v " ) >= 0 || argument.indexOf( "-verbose " ) >= 0 )
				{
					printVerboseMessages = true;
				}
			}
			else if( argument.indexOf( "-V" ) >= 0 )
			{
				if( argument.indexOf( "-V " ) >= 0 || argument.indexOf( "-Verbose " ) >= 0 )
				{
					int index1 = argument.indexOf( " " ) + 1;
					int index2 = argument.indexOf( " ", index1 );
					
					String filename = argument.substring( index1, index2 );
					boolean printError = false;
					try
					{
						printVerboseMessages = true;
						File file = new File( filename );
						if( !file.exists() )
						{
							verboseMessages = new PrintStream( new FileOutputStream( file ) );
						}
						else printError = true;
					}
					catch( FileNotFoundException exception ){ printError = true; }
					catch( SecurityException exception ){ printError = true; }
					
					if( printError )
					{
						println( "Unable to create: \"" + filename + "\" for outputing verbose messages.", true );
					}
				}
			}
			else if( argument.indexOf( "-w" ) >= 0 )
			{
				if( argument.indexOf( "-w " ) >= 0 || argument.indexOf( "-windowSize " ) >= 0 )
				{
					int index1 = argument.indexOf( " " ) + 1;
					int index2 = argument.indexOf( " ", index1 );
					
					int temporary = Integer.parseInt( argument.substring( index1, index2 ) );
					if ( temporary <= 0 || temporary > 10000 )
					{
						println( "Invalid value for window width.", true );
					}
					else windowWidth = temporary;
					
					index1 = index2 + 1;
					index2 = argument.indexOf( " ", index1 );
					
					temporary = Integer.parseInt( argument.substring( index1, index2 ) );
					if ( temporary <= 0 || temporary > 10000 )
					{
						println( "Invalid value for window height.", true );
					}
					else windowHeight = temporary;
				}
			}
		}
		else
		{
		//	println( "Make belive argument: \"" + argument + "\"", false );
		}
	} // END OF - private void ParseArgument( String )
	
	private void ParseArguments( String arguments[] )
	{
		if( !firstParsingPass ) println( "Parsing command line arguments.", false );
		
		boolean sendLastArgument = false;
		String argument = "";
		for( int i = 0; i < arguments.length; i++ )
		{
			if( arguments[i].charAt(0) == '-' && sendLastArgument )
			{
				// Handle help request
				if( argument.indexOf( "-?" ) < 0 && argument.indexOf( "--help" ) < 0 && argument.indexOf( "-h" ) < 0 )
				{
					ParseArgument( argument );
				}
				else
				{
					// Bypassing error output stream to write straight ot the console.
					
					System.out.println( "Usage: java educe [-options]" );
					System.out.println( "Options:" );
					System.out.println( "     -arg (filename)                     - parse additional argument file" );
					System.out.println( "     -saveArg (filename)                 - save arguments to a file" );
					System.out.println( "     -e, -errors                         - output error messages" );
					System.out.println( "     -E, -Errors (filename)              - output error messages to a file" );
					System.out.println( "     -v, -verbose                        - output verbose messages" );
					System.out.println( "     -V, -Verbose (filename)             - output verbose messages to a file" );
					System.out.println( "     -w, -windowSize (width) (height)    - change window size (1024, 768)" );
					
					Quit();
				}
				
				sendLastArgument = false;
				argument = "";
			}
			argument = argument + arguments[i] + " ";
			sendLastArgument = true;
		}
		
		// Parse last argument
		ParseArgument( argument );
		
		if( !firstParsingPass ) println( "Finished parsing command line arguments.", false );
	} // END OF - private void ParseArguments( String[] )
	
	private void ParseFile( String filename )
	{
		if( !firstParsingPass ) println( "Parsing argument file: \"" + filename + "\".", false );
		
		try
		{
			BufferedReader reader = new BufferedReader( new FileReader( filename ) );
			
			String inputLine = reader.readLine();
			
			if( !inputLine.equals( "EDUCE_ARGUMENT_FILE" ) )
			{
				println( "Argument File: \"" + filename + "\" - Not Argument File.", true );
				inputLine = "EDUCE_ARGUMENT_END_OF_FILE"; // short circuit loop
			}
			
			while( !inputLine.equals( "EDUCE_ARGUMENT_END_OF_FILE" ) )
			{
				inputLine = reader.readLine();
				
				// Handle special cases - (skip argument files)
				if( inputLine.indexOf( "-arg" ) < 0 && inputLine.indexOf( "-saveArg" ) < 0 )
				{
					ParseArgument( inputLine );
				}
				else
				{
					println( "Argument File: \"" + filename + "\" - Trying to add/save arguments.", true );
				}
			}
		}
		catch( FileNotFoundException exception )
		{
			println( "Argument File: \"" + filename + "\" - File Not Found Exception.", true );
		}
		catch( IOException exception )
		{
			println( "Argument File: \"" + filename + "\" - IO Exception.", true );
		}
		
		if( !firstParsingPass ) println( "Finished parsing argument file: \"" + filename + "\".", false );
	} // END OF - private void ParseFile( String )
	
	
	/***** Main function for command line - java educe .... *****/
	public static void main( String args[] )
	{
		educe application = new educe( args );
	}
	/***** END OF - public static void main( String args[] ) *****/

}
/*** END OF CLASS ***/
