#include "file.h"
#include <stdio.h>

void saveStorage(Storage storage)
{
   FILE *fp = fopen("storage.bin", "wb");

   // check if file was opened successfully
   if (fp == NULL)
   {
      printf("Error opening file\n");
      return;
   }

   // write the size of the storage to the file
   fwrite(&storage.size, sizeof(int), 1, fp);

   // write each memory block to the file

   fwrite(storage.blocks, sizeof(MemoryBlock) * storage.size, 1, fp);

   // close file
   fclose(fp);
}

void readBinaryFile(Storage *storage)
{
   FILE *fp = fopen("storage.bin", "rb"); // Open file in "read binary" mode
   if (fp == NULL)
   {
      perror("Error opening file");
   }
   else
   {

      // Get the file size
      fseek(fp, 0L, SEEK_END);
      fseek(fp, 0L, SEEK_SET);

      // Calculate the number of blocks in the file
      fread(&storage->size, sizeof(int), 1, fp);
      printf("%d", storage->size);

      // Allocate memory for the Storage struct and its blocks
      storage->blocks = malloc(sizeof(MemoryBlock) * storage->size);

      // Read the blocks from the file into the Storage struct

      fread(storage->blocks, sizeof(MemoryBlock) * storage->size, 1, fp);

      // Close the file
      fclose(fp);
   }
}
