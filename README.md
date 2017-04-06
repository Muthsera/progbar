# progbar
Show progress bar in console

This is a small and simple programm that may be used to display a bar of  
progress in the command line together with a percentage value and the  
estimated time of computation.

Useful for time consuming console applications  
when you want an estimate of the execution time.

## Usage
First, an instance of the bar is created:
`ProgBar bar;`

The bar has to be initialised with its length and possibly a label:  
`ProgBarInit(&bar,40);`  
`ProgBarInitLabel(&bar,40,"Iteration");`

This also draws an empty bar. The bar is updated by passing progress  
values in the range [0,1] to it:  
`ProgBarUpdate(&bar,0.35);`

After completion, the progress bar is marked as finished:  
`ProgBarFinish(&bar);`

If one wishes to resize or relabel the bar in the midst of usage, use:  
`ProgBarResize(&bar,60);`  
`ProgBarRelabel(&bar,"Other name");`
