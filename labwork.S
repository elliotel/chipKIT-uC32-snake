  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
  
.global delay

.global time2string 
 
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #

	.text
hexasc:    
    
    andi $a0,$a0,0xf    # ignores bits for numbers not in the range 0-15
    
    slti $t0,$a0,0xa    # t0 is 1 if return will be a digit, 0 if return will be a letter (0xa = 10)
    bne $t0,$0,digit
    nop
                # nop not required because delayed branching is turned off
                # reached assignment 5, the above comment did not age well
    
    addi $a0,$a0,0x7    # skips the characters between the digits and the letters in the ASCII table
    
digit:    
    addi $a0,$a0,0x30    # skips the characters before 0 in the ASCII table. TRY MOVING FOR NOP
    
    move $v0,$a0
            
    jr $ra
    nop


time2string:
    PUSH     $ra
    PUSH     $s0
    PUSH     $s1
    
    addi     $s0, $0, 0            #from
    addi     $s1, $0, 16           #amount to shift
t2s_loop:                   	   #RENAME
    addi     $s0, $s0, 1
    slti     $t1, $s0, 5               #check if $s0 is smaller than 4
    addi     $s1, $s1, -4             #shift with 12, then 8, then 4, then 0
    
    bne     $s0, 3, t2s_j             # jumps unless a colon should be stored
    nop
    li     $t0, 0x3A                #Den här bör vara lugn
    sb     $t0, ($a0)
    addi     $a0, $a0, 1
t2s_j:
    bne     $t1, $0, t2s_convert     #as long as $t1 is smaller than 4, loop
    nop
    li     $t0, 0x00
    sb    $t0, ($a0)
    
    POP     $s1
    POP     $s0
    POP     $ra
    jr     $ra    
    nop
t2s_convert:
    PUSH     $a0
    srlv     $a0, $a1, $s1    #shift with set amount
    jal     hexasc         
    nop
    POP     $a0,
    sb     $v0, ($a0)    #Increase the adress by one spot
    addi     $a0,$a0, 1
    j    t2s_loop
    nop


delay:
    sgt $t0,$a0,0        #$t0 is set to 1 if $a0 > 0
    bne $t0,1,skip        #if $t0 is not 1 ($a0 <= 0), end loop
    nop    
    
    addi $a0,$a0,-1        #reduce value of $a0 by 1
    
    li $t1,0
innerloop:
    slti $t2,$t1,3350    #$t2 is set to 1 if $t1 < 3
    
    bne $t2,1,delay    #if $t2 is not 1 ($t1 >= 3), end inner loop and reloop main subroutine
    nop
    addi $t1,$t1,1
    j innerloop
    nop
skip:
     jr $ra
     nop

enable_interrupt:

