//----------------------------------------------------------------------------------------------------------------------
//                                                                                                                      
//                                                      8MMNMM8                                                         
//                                               $MMMMMMMMMMMNMMNNNNN:                                                  
//                                         7MMMMMMMMMMMMMNNNMNNNNNNNMNDDDDD7                                            
//                                        MMMMMMMMMMMMMMNNNNNNNNNNNNNND8DD888                                           
//                                      ZMMMMMMMMMMMMMMMMNNNNNNNNNNNNNNNNDDD8888:                                        
//                                   8MMMMMMMMMMMMMMMMMMMNNNNNNNNNNNNNNNNDDD88888=                                      
//                                 MMMMMMMMMMMMMMMMMMMMMMMNNNMNNNNNNNNNNNNNDD88888O8                                    
//                               MMMMMMMMMMMMMMMMNNNNNNNNNNNDDDDDDDDDDNNDNNNDD888888O8                                  
//                              MMMMMMMMMMMMMMMNNNNNNNNNNNNDDDDDDDDDDDDDDD888ND88888OO8                                 
//                             MMMMMMMMMMMMMMMNNNNNNNNNNNNNNDDDDDDDDDDDDDDD888OO8DDOOZZZ                                
//                            MMMMMMMMMMMMMMMNMMMMMMMMMMMMMMMMMMMMMMNDDDDDD8888OOOZZ$$$$Z                               
//                           ,MMMMMMMMMMMMMMMMMNNNNNMMMMMMMMMMMMMMMDDDDDDMMMDN8OOOOZZZ++I7                              
//                           MMMMMMMMMMMMMMMMMMMNNNNNNNNNNNNNNNNDDDDDDDDMMM88OMMD8OOZZZ$~I                              
//                          MMMMMMMMMMMMMMMMMNND8Z7I+=~::::::::~~=?I$O88DDDDD8MMOO8M8OZZ$$$                             
//                         MMMMMMMMMMMMMNDOI+:,                         ,~+78O888OMMZDMO$$$$                            
//                        MMMMMMMMMMM87=,                                     ,+$8OOOMMZM8$$7                           
//                       MMMMMMMMMO?:                                             :IOZO8M7MO77                          
//                      MMMMMMMDI,                                                   :7ZZZM7MZ7                         
//                     MMMMMMD?,                                                       ,7DZMDMZ7                        
//                    :MMMMN?,                                                           ,$$$MMZ,                       
//                    MMMM8~                                                               +N$MM7                       
//                    MMMZ,                                                                 ~DZMM                       
//                    MMD:                                           8""""8 8                1XD                        
//                      #2IN                eeeee eeeee eeeeeee eeee 8    " 8               2YE                         
//                        1HM               8   8 8  88 8  8  8 8    8e     8e             3ZF                          
//                          0GL             8e  8 8   8 8e 8  8 8eee 88  ee 88            4AG                           
//                            9FK           88  8 8   8 88 8  8 88   88   8 88           5BH                            
//                              8EJ         88ee8 8eee8 88 8  8 88ee 88eee8 88eee       6CJ                             
//                                7DI                                                7DI                                
//                                 OO?~                                        ,~78D                                    
//                                    D8$+:,                             ,~?ZDN                                         
//                                         ,NDO$?=::,             ,:~=?$8DM,                                            
//                                                   ~7NNNNN8NNNNNZ:                                                    
//                                                                                                                      
//                                                                                                                      
//                                                   Copyright 2010                                                     
//                                  Art, Research, Technology and Science Laboratory                                    
//                                             The University of New Mexico                                             
//                                                  Project Home Page                                                   
//                                       <<<<http://artslab.unm.edu/domegl>>>>>                                         
//                                                   Code Repository                                                    
//                                       <<<https://svn.cs.unm.edu/domegl>>>>>>                                         
//                                                                                                                      
//----------------------------------------------------------------------------------------------------------------------

#include <fileGutz.h>
#include <sys/stat.h>
#include <errno.h>

namespace gutz
{
   bool filecheck(const std::string& filename) throw (gutz::Exception)
   { 
      struct stat fileInfo; 
      bool goodness = false; 
      std::string errMsg = "The file \"";
      errMsg += filename;
      errMsg += "\" could not be accessed because: ";
      
      // Attempt to get the file attributes 
      int err = stat(filename.c_str(), &fileInfo);
      
      if(err == 0)
      {
            // We're good!
            goodness = true; 
      }
      else if(err == EACCES)
      {
         errMsg += "Search permission is denied for a component of the path prefix.";
      }
      else if(err == EFAULT)
      {
         errMsg += "Internal filecheck() error occurred: filename or fileInfo points to an invalid address.";
      }
      else if(err == EIO)
      {
         errMsg += "An I/O error occurs while reading from or writing to the file system.";
      }
      else if(err == ELOOP)
      {
         errMsg += "Too many symbolic links are encountered in translating the pathname.  This is taken to be indicative of a looping symbolic link.";
      }            
      else if(err == ENAMETOOLONG)
      {
         errMsg += "A component of a pathname exceeds {NAME_MAX} characters, or an entire path name exceeds {PATH_MAX} characters.";
      }
      else if(err == ENOENT)
      {
            errMsg += "the named file does not exist.";
      }
      else if(err == ENOTDIR)
      {
            errMsg += "A component of the path prefix is not a directory.";
      }
      else if(err == EOVERFLOW)
      {  
            errMsg += "The file size in bytes or the number of blocks allocated to the file or the file serial number cannot be represented correctly in the structure pointed to by buf.";
      }
      else
      {
            errMsg += "An undefined error occurred. Check to make sure the file exists. Sorry for the lame error. if this is OS X, it's because they didn't implement stat() correctly";
      }

      // If there ain't no goodness, throw an exception
      if(!goodness) throw gutz::Exception(errMsg);

      // Life is good
      return(goodness); 
   }
}
