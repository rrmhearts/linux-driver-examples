#include <stdio.h>
#include <string.h>

int main () {
   FILE *fp;
   char buffer[100];

   /* Open file for both reading and writing */
   fp = fopen("/sys/kernel/binattr", "r");

   /* Write data to the file */
//   fwrite(c, strlen(c) + 1, 1, fp);

   /* Seek to the beginning of the file */
//   fseek(fp, 0, SEEK_SET);

   /* Read and display data */
   fread(buffer, 12, 1, fp);
   printf("%s\n", buffer);
   fclose(fp);
   
   return(0);
 }
