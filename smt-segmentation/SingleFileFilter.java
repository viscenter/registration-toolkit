/***************************************************************************************************
*                                                                                                  *
*                                          EDUCE PROJECT                                           *
*                                                                                                  *
****************************************************************************************************
*                                                                                                  *
*                                      Author: Daniel G Olson                                      *
*                                      Email: dgolso0@uky.edu                                      *
*                                 Completed - February 12th, 2004                                  *
*                                                                                                  *
*    Filename: "SingleFileFilter.java"                                                             *
*                                                                                                  *
*    Purpose:                                                                                      *
*             This class is a file filter for generic files.                                       *
*                                                                                                  *
*    PUBLIC VARIABLES:                                                                             *
*       None                                                                                       *
*                                                                                                  *
*    PROTECTED VARIABLES:                                                                          *
*       None                                                                                       *
*                                                                                                  *
*    PRIVATE VARIABLES:                                                                            *
*       String:                                                                                    *
*          fileExtension - string containing acceptable file extension.                            *
*          fileDescription - string containing description of file extension.                      *
*                                                                                                  *
*    CONSTRUCTOR FUNTIONS:                                                                         *
*       None                                                                                       *
*                                                                                                  *
*    PUBLIC FUNTIONS:                                                                              *
*       boolean accept( File )                                                                     *
*          - Returns true if file extension is the same as fileExtension.                          *
*       String getDescription()                                                                    *
*          - Returns fileDescription.                                                              *
*       String getExtension()                                                                      *
*          - Returns fileExtension.                                                                *
*       static String getExtension( File )                                                         *
*          - Returns the extension of a file - includes the "."                                    *
*    PROTECTED FUNTIONS:                                                                           *
*       None                                                                                       *
*                                                                                                  *
*    PRIVATE FUNTIONS:                                                                             *
*       None                                                                                       *
*                                                                                                  *
***************************************************************************************************/

import java.io.File;
import javax.swing.*;
import javax.swing.filechooser.*;

public class SingleFileFilter extends FileFilter
{
    private String fileExtension, fileDescription;
    
    public SingleFileFilter( String inFileExtension, String inFileDescription )
    {
        fileExtension = inFileExtension;
        fileDescription = inFileDescription;
    }
    
    public boolean accept( File file )
    {
        if( file.isDirectory() ) return true;
        String extension = getExtension( file );
        
        if( extension != null && extension.equals( fileExtension ) ) return true;
        return false;
    } // END OF - public boolean accept( File )
    
    public String getDescription()
    {
        return fileDescription;
    } // END OF - public String getDescription()
    
    public String getExtension()
    {
        return fileExtension;
    } // END OF - public String getDescription()
    
    public static String getExtension( File file )
    {
        try
        {
            String path = file.getAbsolutePath();
            int lastPeriod = path.lastIndexOf( "." );
            return path.substring( lastPeriod );
        }
        catch( NullPointerException exception ){}
        catch( IndexOutOfBoundsException exception ){}
        return null;
    } // END OF - public static String getExtension( File )
} // END OF - public class SingleFileFilter extends FileFilter
