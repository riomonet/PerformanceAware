#include <stdio.h>
#include <stdlib.h>
FILE* file;

char *reg_table[] = {
    "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh",
    "ax", "cx", "dx", "bx", "sp", "bp", "si", "di"
  };

char *rm_table[] = {
    "bx + si", "bx + di", "bp + si", "bp + di", "si",
    "di", "bp", "bx"
  };

void mem_to_reg_to_mem(unsigned char);
void immediate_to_mem(unsigned char);
void accumulator_ops(unsigned char);
void print_binary(unsigned char);
void immediate_to_reg(unsigned char);
void arithmatic_immediate(unsigned char);
int main(int argc, char* argv[]) {

  char* file_name = argv[1];
  unsigned char opcodeDW;

  file = fopen(file_name, "rb");
  if(!file) {
    printf("File not found: %s", file_name);
    exit(1);
  }
    
  printf("; sim8086  listing_39 instruction disassembly\n");
  printf("bits 16\n");
  
  while (1){

    fread(&opcodeDW,  1, 1, file);
    if(feof(file)) break;

    /* MOV          immediate to register                  */
    if((opcodeDW & 0b10110000) == 0b10110000) {
      printf("mov ");
      immediate_to_reg(opcodeDW);

      /* MOV => reg to mem, reg to reg, mem to reg          */
    } else if ((opcodeDW & 0b10001000) == 0b10001000) {
      printf("mov ");
      mem_to_reg_to_mem(opcodeDW);

      /*MOV =>  Accumulator operations            */
    } else if ((opcodeDW & 0b10100000) == 0b10100000) {
      printf("mov ");
      accumulator_ops(opcodeDW);
      
      /*  MOV => IMMEDIATE TO RM  MEMORY MODE OPS     */
    } else if ((opcodeDW & 0b11000110) == 0b11000110) {
      printf("mov ");
      immediate_to_mem(opcodeDW);
    }
    /* ADD rm->reg->rm */
    else if ((opcodeDW & 0b11111100) == 0b00000000) {
      printf("add ");
      mem_to_reg_to_mem(opcodeDW);
    } /* add immediate */
    else if ((opcodeDW & 0b11111100) == 0b10000000) {
      arithmatic_immediate(opcodeDW);
    } /* add accum */
    else if ((opcodeDW & 0b11111110) == 0b00000100) {
      printf("add  ");
      accumulator_ops(opcodeDW);
    }
  }
  fclose(file);
  printf("\n");
}

void mem_to_reg_to_mem(unsigned char opcodeDW) {
      unsigned char operands;
      fread(&operands, 1, 1, file);

      //reg to reg
      unsigned char D      = (opcodeDW & 0b00000010) >> 1;
      unsigned char W      = (opcodeDW & 0b00000001) << 3;
      unsigned char reg    = (operands & 0b00111000) >> 3;
      unsigned char rm     = operands & 0b00000111;
      
      /* mod 11 reg mode, reg <-> reg no displacement */
      if((operands >> 6) == 0b11) {
	if(D == 0) {
	  /* destintion is reg in reg field */
	  printf("%s, %s\n",
		 reg_table[W | rm],   //destination
		 reg_table[W | reg]); // source
      
	} else {
	  /* destination is reg in rm field */
	  printf("%s, %s\n",
		 reg_table[W | reg],  //destination
		 reg_table[W | rm]); //source
	}
      }
      
      /* mod 00 memory mode, no displacement */
      else if((operands >> 6) == 0) {
	/* mod 00 *special case direct addr 16 bit dispalcemetn*/
	if (rm == 0b110) {
	  unsigned char disp_low;
	  signed char disp_high;
	  fread(&disp_low, 1, 1, file);
	  fread(&disp_high, 1, 1, file);
	  printf("%s, [%d] \n",
		 reg_table[W | reg],
		 (disp_high << 8) | disp_low
		 );
	}
	else
	  /*regular case for 00 no displacement */
	  if(D == 1) {
	  printf("%s, [%s] \n",
 		 reg_table[W | reg], /* destination is register */
		 rm_table[rm]
		 );
	} else {
	  printf("[%s], %s \n", 
		 rm_table[rm],	/* destination is rm */
		 reg_table[W | reg]
		 );
	} 
      }
      /* mod 01 mem mode 8 bit displacement*/
      else if((operands >> 6) == 0b01) {
      
	signed char disp_lo;
	fread(&disp_lo, 1, 1, file);
      
	if(D == 1) {
	  printf("%s, [%s + %d] \n",
		 reg_table[W | reg],
		 rm_table[rm],
		 disp_lo
		 );
	} else {
	  printf("[%s + %d], %s \n",
		 rm_table[rm],
		 disp_lo,
		 reg_table[W | reg]
		 );
	} 
      }

      else if((operands >> 6) == 0b10) {
	unsigned char dips_low;
        signed char disp_high;
        fread(&dips_low, 1, 1, file);
	fread(&disp_high, 1, 1, file);

	if(D == 1) {
	  /* register is destination */
	  printf("%s, [%s + %d] \n",
		 reg_table[W | reg],
		 rm_table[rm],
		 (disp_high << 8) | dips_low
		 );
	} else {
	  /* memory is destination */
	  printf("[%s + %d], %s \n",
		 rm_table[rm],
		 (disp_high << 8) | dips_low,
		 reg_table[W | reg]
		 );
	}
      }
}

 void immediate_to_mem(unsigned char opcodeDW){
  unsigned char operands;
  unsigned char data_low;
  unsigned char data_high;
  unsigned char disp_low;
  unsigned char disp_high;

  char* width = "byte";

  /* mode 01 1 byte displacment */
  if((operands >> 6) == 0b01){
    fread(&disp_low,1,1,file);
  }

  /* mode 10 2 byte displacemnet  */
  if((operands >> 6) == 0b10){
    fread(&disp_low,1,1,file);
    fread(&disp_high,1,1,file);
  }

  /* first byte of immediate data */
  fread(&data_low,1,1,file);
  int immediate = data_low;

  /* if w = 1 immediate data read 2nd byte of immediate data */
  if((opcodeDW & 0b00000001) == 1){
    fread(&data_high,1,1,file);
    immediate = ((data_high << 8) | data_low);
     
    width = "word";
  }
      
  unsigned char rm = operands & 0b00000111;
  if((operands >> 6) == 0b00){
    printf("[%s], %s %d \n",rm_table[rm], width, immediate);
  } else {
    printf("[%s + %d], %s %d \n",
	   rm_table[rm],
	   ((disp_high<<8) | disp_low),
	   width,
	   immediate);
  }
}
      
void accumulator_ops(unsigned char opcodeDW) {

  unsigned char addr_low;
  unsigned char addr_high;
  fread(&addr_low,1,1,file);

  if ((opcodeDW & 0b11111110) == 0b00000100) {
    if((opcodeDW & 0b00000001) == 1) {
      fread(&addr_high,1,1,file);
      printf("ax, %d\n", (addr_high << 8) | addr_low);
  } else {
    printf("al, %d\n", addr_low);
    }

    return;
  }

  fread(&addr_high,1,1,file);
  char *reg = "al";
  if((opcodeDW & 0b00000001) == 1)
    reg = "ax";
  if(opcodeDW & 0b00000010)
    printf("[%d], reg \n", ((addr_high << 8) | addr_low));
  else
    printf("reg, [%d] \n", ((addr_high << 8) | addr_low));
}

void immediate_to_reg(unsigned char opcodeDW) {
  /* last 4 bits of opcode specify register for destination */
  printf("%s, ", reg_table[opcodeDW & 0b00001111]);
  unsigned char lower;
  fread(&lower, 1, 1, file);

  // immediate is made of low and high if w is 1
  if(opcodeDW & 0b00001000) {
    unsigned char upper;
    fread(&upper, 1, 1, file);
    printf("%d\n", (upper << 8) | lower);
  } else // immediate is a single byte 
	      printf("%d\n", lower);
}

void arithmatic_immediate(unsigned char opcodeDW){

  unsigned char operands;
  fread(&operands,1,1,file);
  
  unsigned char SW  = (opcodeDW & 0b00000011);
  unsigned char W   = (opcodeDW & 0b00000001) << 3;
  unsigned char reg = (operands & 0b00111000) >> 3;
  unsigned char rm  = operands & 0b00000111;
  unsigned char disp_low, disp_high,data_low, data_high;
  char* type;

  switch(reg) {
  case 0:
    type = "add";
    break;
  case 0b101:
    type = "sub";
    break;
  case 0b111:
    type = "cmp";
    break;
  default:
    printf("Handler undefined:");
    exit(1);
  }
    
  if((operands >> 6) == 0b01){
    fread(&disp_low,1,1,file);
    
  }
  /* mode 10 2 byte displacemnet  */
  if((operands >> 6) == 0b10){
    fread(&disp_low,1,1,file);
    fread(&disp_high,1,1,file);
  }
  
  fread(&data_low,1,1,file);
  signed char imm = data_low;
  if(SW == 1) {
    fread(&data_high,1,1,file);
    imm = (data_high << 8) | data_low;
  }
  if((operands >> 6) == 0b11)
    printf("%s %s, %d\n",type, reg_table[W | rm] ,imm);
  
}

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
}
