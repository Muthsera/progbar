# progress
Show progress in console

This is a small and simple programm (rather: library) that may be used to
display the progress of a running program in the command line.
In the case of known execution time, a bar can be displayed, together with the
percentage value and the estimated execution time;
otherwise a simple business indicator may be shown.

Useful for time consuming console applications  
when you want an estimate of the execution time.

## Usage
First, an instance of the bar or indicator is created:
`progress_bar bar;`
`progress_indicator ind;`

The progress object has to be initialised with possibly length and label:  
`progress_bar_init(&bar,40);`  
`progress_bar_label_init(&bar,40,"Iteration");`
`progress_indicator_init(&ind);`  
`progress_indicator_label_init(&ind,"Iteration");`

After completion, the progress object is marked as finished:  
`progress_bar_finish(&bar);`
`progress_indicator_finish(&ind);`

This also draws an empty bar. The bar is updated by passing progress  
values in the range [0,1] to it:  
`progress_bar_update(&bar,0.35);`

## Requirements
Since the drawing and updating occurs on a separate thread, the
"pthread"-library is required.
