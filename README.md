# Memory Leak Detector
## What is it?
This is a simple C memory leak detector that checks for any calls to malloc that were not freed with free anywhere in the program. It does not support aligned malloc or any memory that is allocated and returned via a function. However, it is suitable for smaller libraries as well where there is not many heap memory variables returned. 

It does not support a variable that is freed from another alias or 2 variables with the same name. Under that case, if one is freed and the other is not, it will simply state that it is not freed.


NOTE: This has not been tested for large files or on different machines.