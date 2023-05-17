## VIM basics

### IN VISUAL MODE (v to enter)
- copy selection: `y`
- paste: `p`
- paste before cursor: `P`
- delete selected: `d`
- delete to end of line: `D`
- select the line: `V`
- copy the line: `yy`
- delete the line: `dd`
- indent the line: `>>` or `<<`
- select the current paragraph: `vap` or `vip`
- delete from the current line to the end of the file: `0dG`

<br>

- go to end of line: `$`

<br>

- `[[` : sections backward or to the previous '{' in the - first column.
- `]]` : sections forward or to the next '{' in the first - column.
- `[]` : sections backward or to the previous '}' in the - first column.
- `][` : sections forward or to the next '}' in the first - column.


### IN NORMAL MODE (Esc to enter)
- quit: `q`
- undo: `:u`
- redo: `Ctrl+r`
- replace pattern with string: `:s/pattern /string /flags`
- search for string forward: `/string`
- repeat search forward/backward: `n` / `N`
- copy next N lines: `Nyj`
