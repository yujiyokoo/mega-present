# Bytecode Format

| Bytecode Structure |
| ------ |
| HEADER |
| IREP HEADER |
| IREP BODY(1) |
| IREP BODY(2) |
| IREP BODY(...) |


## HEADER

| offset | Title | # of bytes | type | Detail |
| ---- | ---- | ---- | ---- | ---- |
| 00 | RITE_BINARY_IDENT | 4 | string | "RITE" |
| 04 | Binary Version|4|string|"0003" |
| 08 | CRC | 2 | binary | CRC |
| 0A | Binary Size | 4 | binary | |
| 0E | Compiler Name | 4 | string | "MATZ" |
| 12 | Compiler Version | 4 | string | "0000" |
| 16 | - | - | - | - |

## IREP HEADER

| offset | Title | # of bytes | type | Detail |
| ---- | ---- | ---- | ---- | ---- |
| 00 | IREP_IDENT | 4 | string | "IREP" |
| 04 | IREP size | 4 | binary | size of total IREP |
| 08 | Version? | 4 | string | "0000" |
| 0C | - | - | - | - |


## IREP BODY

| offset | Title | # of bytes | type | Detail |
| ---- | ---- | ---- | ---- | ---- |
| 00 | IREP size2 | 4 | string | size of one IREP |
| 04 | nlocals | 2 | binary | number of local variable |
| 06 | nregs | 2 | binary | number of register variable |
| 08 | rlen | 2 | binary | number of child irep ? |
| 0A | ilen | 4 | binary | number of instruction |
| 0E | irep | (ilen)*4 | bunary |  instructions |
| POOL |
| 0E+(ilen)*4 | pools | 4 | binary | number of pools |
| (00) | pool TT | 1 | binary | type of pool |
| (01) | pool size | 2 | binary | size of pool |
| (03) | pool data | (pool size) | string | pool data |
| SYMBOL |
| 0E+(ilen)*4+(pools) | symbols | 4 | binary | number of symbols |
| (00) | symbol size | 2 | binary | symbol size |
| (02) | symbol | (symbol size) + 1 | string | symbol string (with zero terminate) |

