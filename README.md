# MOS-Operating-System
A Virtual Implemention of routines of OS.  
Project consited of three phases. Phase3 is final phase for project.  
Works for any no. of jobs. Supports input spooling, output spooling, paging and error handling.  
Built entirely in C++  

MOS(Multiprogramming Operating System)  
1. Used for execution of multiple jobs  
2. Job should be defined ,well structured.  
3. System Will Report Error ,For Wrong Jobs With Proper Error Messages.  
4. Series of Jobs should be kept in input.txt.  
5. Afer processing of jobs output can be seen output.txt.  
6. Random memory allocation for program card used using paging.  

## Job can consist of Instruction
GD-Get Data  
PD-Print Data  
LR-Load Register From Memory  
SR-Store Register From Memory  
CR-Compare Register And Memory  
BT-Branch Toggle(Checks Toggle Variable)  
H- Denotes end of job : Hault.  
Job should start with $AMJ  
There is $DTA after declaration of program cards and before data cards  
Job should end with $END  
