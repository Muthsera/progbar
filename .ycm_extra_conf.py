def FlagsForFile( filename, **kwargs ):
    return {
            'flags': [
                '-x', 'c',
                '-std=c99',
                '-Wall', '-Wextra', '-Wpedantic',
                '-D_POSIX_C_SOURCE=199309L',
                '-I', './',
                '-isystem', '/home/piet/.vim/ycm_include'
                ]
            }
