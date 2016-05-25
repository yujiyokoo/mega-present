# Opcodes list, description and format

## Opcode packing types

Opcode is packed to 32bit uint32_t value. It's decoded corresponding to arguments type applicable to this opcode.

```
1. A:B:C:OP   = 9:9:7: 7
2. A:Bx:OP    = 9:16:  7
3. Ax:OP      = 25:    7
4. A:Bz:Cz:OP = 9:14:2:7

sBx argument is just signed Bx.
```

## List

| OPCode name | Numeric value | Arguments | Description | Implemented |
| ----------- | ------------- | --------- | ----------- | ----------- |
| OP_NOP      | 0x00          |           | do nothing  | +
| OP_MOVE     | 0x01          | A, B      | `R(A) := R(B)`| +
| OP_LOADL    | 0x02          | A, Bx     | `R(A) := Lit(Bx)` | +
| OP_LOADI    | 0x03          | A, sBx    | `R(A) := sBx` | +
| OP_LOADSYM  | 0x04          | A, Bx     | `R(A) := Sym(Bx)` | -
| OP_LOADNIL  | 0x05          | A         | `R(A) := nil` | +
| OP_LOADSELF | 0x06          | A         | `R(A) := self` | +
| OP_LOADT    | 0x07          | A         | `R(A) := true` | +
| OP_LOADF    | 0x08          | A         | `R(A) := false` | +
| OP_GETGLOBAL| 0x09          | A, Bx     | `R(A) := getglobal(Sym(Bx))` | +
| OP_SETGLOBAL| 0x0A          | A, Bx     | `setglobal(Sym(Bx), R(A))` | +
| OP_GETSPECIAL| 0x0B         | A, Bx     | `R(A) := Special[Bx]` | -
| OP_SETSPECIAL| 0x0C         | A, Bx     | `Special[Bx] := R(A)` | -
| OP_GETIV    | 0x0D          | A, Bx     | `R(A) := ivget(Sym(Bx))` | -
| OP_SETIV    | 0x0E          | A, Bx     | `ivset(Sym(Bx),R(A))` | -
| OP_GETCV    | 0x0F          | A, Bx     | `R(A) := cvget(Sym(Bx))` | -
| OP_SETCV    | 0x10          | A, Bx     | `cvset(Sym(Bx),R(A))` | -
| OP_GETCONST | 0x11          | A, Bx     | `R(A) := constget(Sym(Bx))` | +
| OP_SETCONST | 0x12          | A, Bx     | `constset(Sym(Bx),R(A))` | +
| OP_GETMCNST | 0x13          | A, Bx     | `R(A) := R(A)::Sym(B)` | -
| OP_SETMCNST | 0x14          | A, Bx     | `R(A+1)::Sym(B) := R(A)` | -
| OP_GETUPVAR | 0x15          | A, B, C   | `R(A) := uvget(B,C)` | -
| OP_SETUPVAR | 0x16          | A, B, C   | `uvset(B,C,R(A))` | -
| OP_JMP      | 0x17          | sBx       | `pc+=sBx` | +
| OP_JMPIF    | 0x18          | A, sBx    | `if R(A) pc+=sBx` | +
| OP_JMPNOT   | 0x19          | A, sBx    | `if !R(A) pc+=sBx` | +
| OP_ONERR    | 0x1A          | sBx       | `rescue_push(pc+sBx)` | -
| OP_RESCUE   | 0x1B          | A         | `clear(exc); R(A) := exception (ignore when A=0)` | -
| OP_POPERR   | 0x1C          | A         | `A.times{rescue_pop()}` | -
| OP_RAISE    | 0x1D          | A         | `raise(R(A))` | -
| OP_EPUSH    | 0x1E          | Bx        | `ensure_push(SEQ[Bx])` | -
| OP_EPOP     | 0x1F          | A         | `A.times{ensure_pop().call}` | -
| OP_SEND     | 0x20          | A, B, C   | `R(A) := call(R(A),mSym(B),R(A+1),...,R(A+C))` | +
| OP_SENDB    | 0x21          | A, B, C   | `R(A) := call(R(A),mSym(B),R(A+1),...,R(A+C),&R(A+C+1))` |  -
| OP_FSEND    | 0x22          | A, B, C   | `R(A) := fcall(R(A),mSym(B),R(A+1),...,R(A+C-1))` | -
| OP_CALL     | 0x23          | A, B, C   | `R(A) := self.call(R(A),.., R(A+C))` | -
| OP_SUPER    | 0x24          | A, B, C   | `R(A) := super(R(A+1),... ,R(A+C-1))` | -
| OP_ARGARY   | 0x25          | A, Bx     | `R(A) := argument array (16=6:1:5:4) ` | -
| OP_ENTER    | 0x26          | Ax        | `arg setup according to flags (24=5:5:1:5:5:1:1)` | *
| OP_KARG     | 0x27          | A, B, C   | `R(A) := kdict[mSym(B)]; if C kdict.rm(mSym(B))` | -
| OP_KDICT    | 0x28          | A, C      | `R(A) := kdict` | -
| OP_RETURN   | 0x29          | A, B      | `return R(A) (B=normal,in-block return/break)` | +
| OP_TAILCALL | 0x2A          | A, B, C   | `return call(R(A),mSym(B),*R(C))` | -
| OP_BLKPUSH  | 0x2B          | A, Bx     | `R(A) := block (16=6:1:5:4)` | -
| OP_ADD      | 0x2C          | A, B, C   | `R(A) := R(A)+R(A+1) (mSyms[B]=:+,C=1)` | +
| OP_ADDI     | 0x2D          | A, B, C   | `R(A) := R(A)+C (mSyms[B]=:+)` | +
| OP_SUB      | 0x2E          | A, B, C   | `R(A) := R(A)-R(A+1) (mSyms[B]=:-,C=1)` | +
| OP_SUBI     | 0x2F          | A, B, C   | `R(A) := R(A)-C (mSyms[B]=:-)` | +
| OP_MUL      | 0x30          | A, B, C   | `R(A) := R(A)*R(A+1) (mSyms[B]=:*,C=1)` | -
| OP_DIV      | 0x31          | A, B, C   | `R(A) := R(A)/R(A+1) (mSyms[B]=:/,C=1)` | -
| OP_EQ       | 0x32          | A, B, C   | `R(A) := R(A)==R(A+1) (mSyms[B]=:==,C=1)` | +
| OP_LT       | 0x33          | A, B, C   | `R(A) := R(A)<R(A+1)  (mSyms[B]=:<,C=1)` | +
| OP_LE       | 0x34          | A, B, C   | `R(A) := R(A)<=R(A+1) (mSyms[B]=:<=,C=1)` | +
| OP_GT       | 0x35          | A, B, C   | `R(A) := R(A)>R(A+1)  (mSyms[B]=:>,C=1)` | +
| OP_GE       | 0x36          | A, B, C   | `R(A) := R(A)>=R(A+1) (mSyms[B]=:>=,C=1)` | +
| OP_ARRAY    | 0x37          | A, B, C   | `R(A) := ary_new(R(B),R(B+1)..R(B+C))` | +
| OP_ARYCAT   | 0x38          | A, B      | `ary_cat(R(A),R(B))` | -
| OP_ARYPUSH  | 0x39          | A, B      | `ary_push(R(A),R(B))` | -
| OP_AREF     | 0x3A          | A, B, C   | `R(A) := R(B)[C]` | -
| OP_ASET     | 0x3B          | A, B, C   | `R(B)[C] := R(A)` | -
| OP_APOST    | 0x3C          | A, B, C   | `*R(A),R(A+1)..R(A+C) := R(A)` | -
| OP_STRING   | 0x3D          | A, Bx     | `R(A) := str_dup(Lit(Bx))` | -
| OP_STRCAT   | 0x3E          | A, B      | `str_cat(R(A),R(B))` | -
| OP_HASH     | 0x3F          | A, B, C   | `R(A) := hash_new(R(B),R(B+1)..R(B+C))` | -
| OP_LAMBDA   | 0x40          | A, Bz, Cz | `R(A) := lambda(SEQ[Bz],Cm)` | +
| OP_RANGE    | 0x41          | A, B, C   | `R(A) := range_new(R(B),R(B+1),C)` | -
| OP_OCLASS   | 0x42          | A         | `R(A) := ::Object` | -
| OP_CLASS    | 0x43          | A, B      | `R(A) := newclass(R(A),mSym(B),R(A+1))` | +
| OP_MODULE   | 0x44          | A, B      | `R(A) := newmodule(R(A),mSym(B))` | -
| OP_EXEC     | 0x45          | A, Bx     | `R(A) := blockexec(R(A),SEQ[Bx])` | -
| OP_METHOD   | 0x46          | A, B      | `R(A).newmethod(mSym(B),R(A+1))` | +
| OP_SCLASS   | 0x47          | A, B      | `R(A) := R(B).singleton_class` | -
| OP_TCLASS   | 0x48          | A         | `R(A) := target_class` | +
| OP_DEBUG    | 0x49          | A         | `print R(A)` | -
| OP_STOP     | 0x4A          |           | stop VM | +
| OP_ERR      | 0x4B          | Bx        | `raise RuntimeError with message Lit(Bx)` | -
| OP_RSVD1    | 0x4C          |           | reserved | -
| OP_RSVD2    | 0x4D          |           | reserved | -
| OP_RSVD3    | 0x4E          |           | reserved | -
| OP_RSVD4    | 0x4F          |           | reserved | -
| OP_RSVD5    | 0x50          |           | reserved | -
