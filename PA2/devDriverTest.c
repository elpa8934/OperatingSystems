#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>

#define BUFFER_LENGTH 1024

int main(){
	int whence;
	int offset;
	int numBytes;
	char toBeWritten[BUFFER_LENGTH];
	char command = 'a';
	int driver = open("/dev/pa2_char_device", O_RDWR);
        while (command != 'e'){
		printf("Press r to read from device\nPress w to write to the device\nPress s to seek into the device\nPress e to exit from the device\nPress anything else to keep reading or writing from the device\nEnter command:\n");
		scanf(" %c", &command);
		if (command == 'w'){ 
			printf("Enter the data you want to write to the device:\n");
			scanf(" %[^\n]%*c", toBeWritten);
			write(driver, toBeWritten, strlen(toBeWritten));
		} else if (command == 's'){
			printf("Enter an offset value:\n");
			scanf("%d", &offset);
			printf("Enter a value for whence (third paramter):\n");
			scanf("%d", &whence);
			lseek(driver, offset, whence);
		} else if (command == 'r'){
			printf("Enter the number of bytes you want to read:\n");
			scanf("%d", &numBytes);
			char *toRead = malloc(numBytes);
			read(driver, toRead, numBytes);
			printf("Data read from the device: \n");
			printf(toRead);
			printf("\n");
		}
	}
	printf("Good bye!\n");
	close(driver);
	return 0;
}
