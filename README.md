# progbar
Show progress bar in console

This is a small and simple programm that may be used to display a bar of  
progress in the command line together with a percentage value and the  
estimated time of computation.

Useful for time consuming console applications  
when you want an estimate of the execution time.

## Usage
First a handle for the progress bar must be created:  
`ProgBar bar;`

The bar has to be initialised with its length:  
`InitProgBar(&bar,40);`

Alternatively a label can also be specified:  
`InitProgBarLabel(&bar,40,"Iteration");`

This also draws an empty bar. The bar is updated by passing progress  
values in the range [0;1] to it:
`UpdateProgBar(&bar,0.35);`

After completion, the progress bar is to be deleted and memory is freed:  
`FinishProgBar(&bar);`

If one wishes to resize the bar in the midst of usage, use:  
`ResizeProgBar(&bar,60);`
