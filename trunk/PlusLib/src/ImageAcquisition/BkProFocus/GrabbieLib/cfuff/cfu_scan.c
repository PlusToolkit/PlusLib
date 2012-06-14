#ifndef __cfu_scan_c
#define __cfu_scan_c

#include "cfu_scan.h"

/*********************************************************************
  NAME     : cfu_scan.c
  ABSTRACT : Functions for saving and loading data from ultrasound
             scanning.
  VERSION  : 1.0, 15 Dec 1999, Svetoslav Nikolov
**********************************************************************/

/*
 * FUNCTION - scanFillHeader
 * ABSTRACT - Make the header from the input data
 *
 */
 
TScanHeader* scanFillHeader(TScanHeader* hdr,
                            uint no_volumes,
                            uint no_planes,
                            uint no_planes_per_volume, 
                            uint no_lines_per_plane,
                            uint no_samples_per_line,
                            uint no_bits_per_sample)
{ 
   /* 
    * Make all the header entries readable for a normal 
    * file editor
    */
   memset(hdr->who.name,' ',FIELD_NAME_LENGTH);  
   memset(hdr->who.val,' ',FIELD_VAL_LENGTH);

   memset(hdr->version.name,' ',FIELD_NAME_LENGTH);  
   memset(hdr->version.val,' ',FIELD_VAL_LENGTH);

   memset(hdr->no_volumes.name,' ',FIELD_NAME_LENGTH);  
   memset(hdr->no_volumes.val,' ',FIELD_VAL_LENGTH);

   memset(hdr->no_planes.name,' ',FIELD_NAME_LENGTH);  
   memset(hdr->no_planes.val,' ',FIELD_VAL_LENGTH);

   memset(hdr->no_planes_per_volume.name,' ',FIELD_NAME_LENGTH);  
   memset(hdr->no_planes_per_volume.val,' ',FIELD_VAL_LENGTH);

   memset(hdr->no_lines_per_plane.name,' ',FIELD_NAME_LENGTH);  
   memset(hdr->no_lines_per_plane.val,' ',FIELD_VAL_LENGTH);


   memset(hdr->no_samples_per_line.name,' ',FIELD_NAME_LENGTH);  
   memset(hdr->no_samples_per_line.val,' ',FIELD_VAL_LENGTH);
   
   memset(hdr->no_bits_per_sample.name,' ',FIELD_NAME_LENGTH);  
   memset(hdr->no_bits_per_sample.val,' ',FIELD_VAL_LENGTH);
   
   
   hdr->who.val[FIELD_VAL_LENGTH-1] = '\n';

   hdr->version.val[FIELD_VAL_LENGTH-1] = '\n';
   
   hdr->no_volumes.val[FIELD_VAL_LENGTH-1] = '\n';

   hdr->no_planes.val[FIELD_VAL_LENGTH-1] = '\n';
   
   hdr->no_planes_per_volume.val[FIELD_VAL_LENGTH-1] = '\n';
   
   hdr->no_lines_per_plane.val[FIELD_VAL_LENGTH-1] = '\n';

   hdr->no_samples_per_line.val[FIELD_VAL_LENGTH-1] = '\n';

   hdr->no_bits_per_sample.val[FIELD_VAL_LENGTH-1] = '\n';

   /*
    *  Print the values as string in the header entries
    */
    sprintf(hdr->who.name,"Producer: ");
    sprintf(hdr->who.val,"%s \n","CFU Image Sequence");

    sprintf(hdr->version.name,"Version ");
    sprintf(hdr->version.val,"%s ",FORMAT_VERSION);
    
    sprintf(hdr->no_volumes.name,"Number of Volumes ");
    sprintf(hdr->no_volumes.val,"%d ",no_volumes);

    sprintf(hdr->no_planes.name,"Number of Planes ");
    sprintf(hdr->no_planes.val,"%d ",no_planes);
    
    sprintf(hdr->no_planes_per_volume.name,"Number of Planes per Volume ");
    sprintf(hdr->no_planes_per_volume.val,"%d ",no_planes_per_volume);
 
    sprintf(hdr->no_lines_per_plane.name,"Number of Lines per Plane ");
    sprintf(hdr->no_lines_per_plane.val,"%d ",no_lines_per_plane);

    sprintf(hdr->no_samples_per_line.name,"Number of Samples per Line ");
    sprintf(hdr->no_samples_per_line.val,"%d ",no_samples_per_line);
    

    sprintf(hdr->no_bits_per_sample.name,"Number of Bits per Sample ");
    sprintf(hdr->no_bits_per_sample.val,"%d ",no_bits_per_sample);
    
    return hdr;
}


/*
 * FUNCTION  : scanFillFileStruct - Fill a file structure with 
 *             the header information
 */

TScanFile* scanFillFileStruct(TScanFile* f, TScanHeader* hdr)
{
   sscanf(hdr->no_volumes.val,"%d",&f->no_volumes);
   sscanf(hdr->no_planes.val,"%d",&f->no_planes);
   sscanf(hdr->no_planes_per_volume.val,"%d",&f->no_planes_per_volume);
   sscanf(hdr->no_lines_per_plane.val,"%d",&f->no_lines_per_plane);
   sscanf(hdr->no_samples_per_line.val,"%d",&f->no_samples_per_line);
   sscanf(hdr->no_bits_per_sample.val,"%d",&f->no_bits_per_sample);
   return f;
}


/*
 *  scanCreateFile - Create a scan file.
 */
 
TScanFile* scanFileCreate(char* file_name,
                         uint no_planes_per_volume,
                         uint no_lines_per_plane,
                         uint no_samples_per_line,
                         uint no_bits_per_sample )
{  
   TScanFile* f;
   TScanHeader hdr;
   size_t size;
   
   if (no_planes_per_volume == 0) no_planes_per_volume = 1;
   
   if ((f = (TScanFile*)malloc(sizeof(TScanFile))) == NULL)
   {
      printf("scanFileCreate:%c\n",7);    /* Ping !!! */
      printf("Cannot allocate memory for file description\n");
      return NULL;
   }
   
   if ((f->fl = fopen(file_name,"wb+")) == NULL)
   {
      printf("scanFileCreate:%c\n",7);
      printf("Cannot create file %s \n", file_name);
      free(f);
      return NULL;
   }
   
   f->no_volumes = 0;
   f->no_planes = 0;
   f->no_planes_per_volume = no_planes_per_volume;
   f->no_lines_per_plane = no_lines_per_plane;
   f->no_samples_per_line = no_samples_per_line;
   f->no_bits_per_sample = no_bits_per_sample;
   
   /*
    *  Make and save header information about the file
    */
    
   scanFillHeader(&hdr, f->no_volumes,f->no_planes, f->no_planes_per_volume,
                       f->no_lines_per_plane, f->no_samples_per_line,
                                              f->no_bits_per_sample);
                                              
   size = fwrite(&hdr, sizeof(TScanHeader), 1, f->fl);
   if (size!=1)
   {
       printf("scanFileCreate:%c\n",7);
       printf("Could not save the header information \n");
   }
   
   /*
    * Make sure that the information is on disk !
    */
    
   fflush(f->fl);
   return f;   
}


/*
 *  FUNCTION  scanFileOpen
 *
 */

TScanFile* scanFileOpen(char* file_name)
{
   TScanFile* f;
   TScanHeader hdr;
   size_t no_read;
      
   if ((f = (TScanFile*)malloc(sizeof(TScanFile))) == NULL)
   {
      printf("scanFileOpen:%c\n",7);    /* Ping !!! */
      printf("Cannot allocate memory for file description\n");
      return NULL;
   }
   
   if ((f->fl = fopen(file_name,"rb+")) == NULL)
   {
      printf("scanFileOpen:%c\n",7);
      printf("Cannot open file %s \n", file_name);
      free(f);
      return NULL;
   }
   
   no_read = fread(&hdr,1, sizeof(TScanHeader), f->fl);

   if (no_read!=sizeof(TScanHeader))
   {
       printf("scanFileOpen:%c\n",7);
       printf("Could not read the header information \n");
   }
   
   if (strncmp(hdr.version.val,FORMAT_VERSION, strlen(FORMAT_VERSION)))
   {
      printf("scanFileOpen:%c\n",7);
      printf("File %s has different version than the supported one\n", file_name);
      printf("The supported version is : %s \n", FORMAT_VERSION);
   }
   scanFillFileStruct(f,&hdr);
   return f;   
   
}

/*
 * scanFileClose  - Close scan file 
 */
 
void scanFileClose(TScanFile* f)
{
   TScanHeader hdr;
   size_t size;

   /*
    *  Update the header of the file and save it
    */
    

   scanFillHeader(&hdr, f->no_volumes, f->no_planes,  f->no_planes_per_volume,
                  f->no_lines_per_plane, f->no_samples_per_line,
                                          f->no_bits_per_sample);
   
   fseek(f->fl, 0, SEEK_SET);  
   size = fwrite(&hdr, 1, sizeof(TScanHeader),  f->fl);
   if (size!=sizeof(TScanHeader))
   {
       printf("scanFileClose:%c\n",7);
       printf("Could not save the header information \n");
   }

  /*
   *  Close the file and save it
   */
   fclose(f->fl);
   free(f);
}


/*
 * scanFilePrintInfo - Print information about a ScanFile
 */
 
void scanFilePrintInfo(TScanFile* f)
{
    printf(" no_volume            %d \n", f->no_volumes);
    printf(" no_planes            %d \n", f->no_planes);
    printf(" no_planes_per_volume %d \n", f->no_planes_per_volume);
    printf(" no_lines_per_plane   %d \n", f->no_lines_per_plane);
    printf(" no_samples_per_line  %d \n", f->no_samples_per_line);
    printf(" no_bits_per_sample   %d \n", f->no_bits_per_sample);
}


/*
 *  FUNCTION : scanFilePlaneAppend
 *  ABSTRACT : Append a plane to the scan file
 *  RETURNS  : 0  - success
 *             -1 - error
 */


int scanFilePlaneAppend(TScanFile* f, const void* data)
{ 
   size_t size;       /* Size of data               */
   size_t no_samples;   /* no_samples to write        */
   size_t no_samples_to_write;
   size_t no_written_samples;
   const void* next;  /* Next chunk of data to write */
   
   
   /*
    *  Set the parameters for the saving operation
    */
   switch(f->no_bits_per_sample)
   {
      case 8:  size = 1; break;
      case 16: size = 2; break;
      default: 
              printf("scanFileSliceAppend:%c\n",7);
              printf("Unknown size of data \n");
              return -1;
   }


   /*
    *  Save the data in pieces of 64K
    */        
   
   no_samples = f->no_samples_per_line * f->no_lines_per_plane;

   next = data;
   fseek(f->fl,0,SEEK_END);

   while(no_samples)
   {
      if ((no_samples * size)>(64*1024))
           no_samples_to_write = 64*1024/size;
      else 
          no_samples_to_write = no_samples;

     no_written_samples = fwrite(next, size, no_samples_to_write, f->fl);
     if (no_written_samples != no_samples_to_write)
     {
         printf("scanFileSliceAppend:%c\n",7);
         printf("Couldn't write to file");
         return -1;
     }
     no_samples -= no_written_samples;
     next = (char*)next + no_written_samples * size;
   }
   
   /*
    *  Update the information of the file with the relevant 
    *  information about the number of slices and volumes
    */
   fflush(f->fl);
   f->no_planes ++;
   f->no_volumes = f->no_planes / f->no_planes_per_volume;
   return 0;
}


/*
 *  FUNCTION : scanFilePlaneRead()
 *  ABSTRACT : Reads a plane from the file
 */

int scanFilePlaneRead(const TScanFile* f, uint plane_no, void* data)
{
   size_t size;       /* Size of data               */
   size_t no_samples;   /* no_samples to write        */
   size_t no_samples_to_read;
   size_t no_read_samples;
   size_t start_pos;     /* Starting position in the file */
   void* next;         /* Next chunk of data to write */

   if (plane_no>=f->no_planes)
   { 
      printf("scanFilePlaneRead:\n");
      printf("plane number must be < %d \n", f->no_planes);
      return -1;
   }
    
   /*
    *  Set the parameters for the saving operation
    */
   switch(f->no_bits_per_sample)
   {
      case 8:  size = 1; break;
      case 16: size = 2; break;
      default: 
              printf("scanFileSliceAppend:%c\n",7);
              printf("Unknown size of data \n");
              return -1;
   }


   /*
    *  Save the data in pieces of 64K
    */        
   
   no_samples = f->no_samples_per_line * f->no_lines_per_plane;

   start_pos = plane_no*f->no_lines_per_plane*f->no_samples_per_line*size;
   start_pos += sizeof(TScanHeader);
   fseek(f->fl, (long)start_pos,SEEK_SET);
   next = data;
   while(no_samples)
   { 
      if ((no_samples * size)>(64*1024))
         no_samples_to_read = 64*1024/size;
      else 
         no_samples_to_read = no_samples;

     no_read_samples = fread(next, size, no_samples_to_read, f->fl);
     if (no_read_samples != no_samples_to_read)
     {
     
         printf("scanFilePlaneRead:%c\n",7);
         printf("Couldn't read from file");
         printf("no_Read_samples %d \n", no_read_samples);
         return -1;
     }
     no_samples -= no_read_samples;
     next = (char*)next + no_read_samples * size;
   }
   
   return 0;
}



#endif
