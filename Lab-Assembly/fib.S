  .text
  .globl main
  .type	main, @function

main:
  push %rbp
  mov %rsp, %rbp
  
  # Add your code for the main function here

  # Return 0
  movl $0x0, %eax
  pop %rbp
  retq
  .size	main, .-main
