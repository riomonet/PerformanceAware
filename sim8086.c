#include <stdio.h>

void print_binary(unsigned char byte) {
    // Iterate through each bit in the byte
    for (int i = 7; i >= 0; i--) {
        // Use bitwise AND with 1 shifted i bits to extract the i-th bit
        if (byte & (1 << i)) {
            printf("1");
        } else {
            printf("0");
        }
    }
    printf("\n");
}

int main(int argc, char* argv[]) {

  char* file_name = argv[1];
  FILE* file;
  FILE* outfile;
  
  unsigned char opcodeDW, modRegRm;

  file = fopen(file_name, "rb");
  printf("; sim8086  listing_39 instruction disassembly\n");
  printf("bits 16\n");
    
  char *reg_table[] = {
    "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh",
    "ax", "cx", "dx", "bx", "sp", "bp", "si", "di"
  };
  
  while (1){

    fread(&opcodeDW,  1, 1, file);
    if(feof(file)) break;

    /*==================================================*/
    /*           immediate to register                  */
    /*==================================================*/
    if((opcodeDW & 0b10110000) == 0b10110000) {

      printf("mov %s, ", reg_table[opcodeDW & 0b00001111]);
      unsigned char lower;
      fread(&lower, 1, 1, file);

      // immediate is wide
      if(opcodeDW & 0b00001000) {
	unsigned char upper;
	fread(&upper, 1, 1, file);
	printf("%d\n", (upper << 8) | lower);
      } else // immediate is a single byte
	printf("%d\n", lower);
    }
    
      /*==================================================*/ 
      /*      reg to mem, reg to reg, mem to reg          */
      /*==================================================*/

    else if ((opcodeDW & 0b10001000) == 0b10001000) {
      unsigned char operands;
      fread(&operands, 1, 1, file);
      printf("mov ");
      unsigned char mod = operands >> 6;
      switch(mod)
      /* need d w mod reg r/m */
      /* need r/m look up table */


      
      /* immediate to reg/memory */
    } else if (opcodeDW & 0b11000110 == 0b11000110) {
      printf("mov ");

      /* memory to accumulator */
    } else if (opcodeDW & 0b10100000 == 0b10100000) {
      printf("mov ");
      
    } else if (opcodeDW & 0b10100010 == 0b10100010) {
      printf("mov ");


    }

    /* unsigned char opcode = opcodeDW & opcode_mask; */
    /* unsigned char D      = opcodeDW & D_mask; */
    /* unsigned char W      = (opcodeDW & W_mask) << 3;  */
    /* unsigned char mod    = modRegRm & mod_mask; */
    /* unsigned char reg    = (modRegRm & reg_mask) >> 3; */
    /* unsigned char rm     = modRegRm & rm_mask; */


    /* if (D == 0){ */
    /*   printf("%s, %s\n", */
    /* 	      reg_table[W | rm],   //destination */
    /* 	      reg_table[W | reg]); // source */
      
    /* } else { */
    /*   printf("%s, %s\n", */
    /* 	      reg_table[W | reg],  //destination */
    /* 	      reg_table[W | rm]); //source     */
    /* } */
    
    
  }
  
  fclose(file);
  printf("\n");
}

// parse opcode

// 100010  0 1
// opcode  D W
//
// 11    011   001
// mod   reg   r/m









