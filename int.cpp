#include <stdio.h>
#include <dos.h>


static void interrupt (*oldvec)(...);


void interrupt  VideoBIOSInterrupt(...)
{
      unsigned char ah_var;
      unsigned char al_var;

      asm
      {
         cmp ah, 0h
         je Old
         cmp ah, 02h
         je Old
         cmp ah, 03h
         je Old
         cmp ah, 06h
         je Old
         cmp ah, 07h
         je Old
         cmp ah, 09h
         je Old
         cmp ah, 0Ah
         je Old
         cmp ah, 0Eh
         je Old

         mov ah_var, ah
         mov al_var, al
         jmp CanNotProcess
      Old:
         call oldvec
         jmp Exit
      }

CanNotProcess:

  		FILE *fp;

		if ((fp = fopen("ints.lst","a")) != NULL)
      {
			fprintf(fp,"ah = %X al = %X\n", ah_var, al_var);
			fclose(fp);
		}
Exit:
}


int
main()
{
	printf("Loaded");

	oldvec = _dos_getvect(0x10);
	_dos_setvect(0x10,VideoBIOSInterrupt);
	_dos_keep(0, (_SS + 100 + (_SP/16) - _psp));

	return 0;
}
