syntax match GupFn  /fn/
syntax match GupPub /pub/
syntax match GupAsm /__asm/
syntax match GupU8 /u8/
syntax match GupU16 /u16/
syntax match GupU32 /u32/
syntax match GupU64 /u64/
syntax match GupVoid /void/
syntax match GupLoop /loop/
syntax match GupReturn /return/
syntax match GupStruct /struct/
syntax region GupString start='"' end='"'

highlight GupFn ctermfg=red  guifg=#ff0000
highlight GupPub ctermfg=red  guifg=#ff0000
highlight GupAsm ctermfg=red  guifg=#ff0000
highlight GupU8 ctermfg=red  guifg=#ff0000
highlight GupU16 ctermfg=red  guifg=#ff0000
highlight GupU32 ctermfg=red  guifg=#ff0000
highlight GupU64 ctermfg=red  guifg=#ff0000
highlight GupVoid ctermfg=red  guifg=#ff0000
highlight GupString ctermfg=green guifg=#00dd00
highlight GupReturn ctermfg=red  guifg=#ff0000
highlight GupStruct ctermfg=red  guifg=#ff0000
highlight GupLoop ctermfg=red  guifg=#ff0000
