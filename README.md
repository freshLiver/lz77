# A Simple LZ77 Implementation

## Introduce

The original code is taken from [andyherbert/lz1](https://github.com/andyherbert/lz1), but apply some changes:

- Readability (rename variables and define structures)
- Border checks during encoding
- Alignment problems

## Limitations

## Example

```shell
$ make -B TEST_FILE=/dev/zero                                                           master [86244c1] modified
clang -std=c11 -g -O3 -Wall -Wextra -Werror -o lz.o -c lz.c
clang -std=c11 -g -O3 -Wall -Wextra -Werror -o test.o -c test.c
clang -o main lz.o test.o -fsanitize=undefined,leak,address,integer,implicit-conversion
Compressing 2 bytes... (load time: 668 ns)
        [01]: Compressed to 11 B (385 ns)
        [02]: Compressed to 11 B (79 ns)
        [03]: Compressed to 11 B (56 ns)
        [04]: Compressed to 11 B (39 ns)
        [05]: Compressed to 11 B (45 ns)
        [06]: Compressed to 11 B (26 ns)
        [07]: Compressed to 11 B (26 ns)
        [08]: Compressed to 11 B (25 ns)
        [09]: Compressed to 11 B (29 ns)
        [10]: Compressed to 11 B (30 ns)
        [11]: Compressed to 11 B (26 ns)
        [12]: Compressed to 11 B (29 ns)
        [13]: Compressed to 11 B (30 ns)
        [14]: Compressed to 11 B (29 ns)
        [15]: Compressed to 11 B (45 ns)
Compressing 4 bytes... (load time: 606 ns)
        [01]: Compressed to 11 B (189 ns)
        [02]: Compressed to 11 B (72 ns)
        [03]: Compressed to 11 B (26 ns)
        [04]: Compressed to 11 B (25 ns)
        [05]: Compressed to 11 B (25 ns)
        [06]: Compressed to 11 B (25 ns)
        [07]: Compressed to 11 B (25 ns)
        [08]: Compressed to 11 B (26 ns)
        [09]: Compressed to 11 B (27 ns)
        [10]: Compressed to 11 B (27 ns)
        [11]: Compressed to 11 B (25 ns)
        [12]: Compressed to 11 B (26 ns)
        [13]: Compressed to 11 B (26 ns)
        [14]: Compressed to 11 B (26 ns)
        [15]: Compressed to 11 B (45 ns)
[...]
Compressing 536898077 bytes... (load time: 90121326 ns)
        [01]: Compressed to 536898086 B (726719351 ns)
        [02]: Compressed to 322138856 B (508410906 ns)
        [03]: Compressed to 178966034 B (381415306 ns)
        [04]: Compressed to 94746728 B (319744986 ns)
        [05]: Compressed to 48808925 B (304457316 ns)
        [06]: Compressed to 24779921 B (327318812 ns)
        [07]: Compressed to 12486011 B (291172195 ns)
        [08]: Compressed to 6267302 B (269948971 ns)
        [09]: Compressed to 3139763 B (261648291 ns)
        [10]: Compressed to 1571420 B (258942765 ns)
        [11]: Compressed to 786098 B (258733838 ns)
        [12]: Compressed to 393149 B (266095070 ns)
        [13]: Compressed to 196604 B (268007488 ns)
        [14]: Compressed to 98312 B (258536448 ns)
        [15]: Compressed to 49163 B (259205474 ns)
Compressing 1073767820 bytes... (load time: 175204678 ns)
        [01]: Compressed to 1073767829 B (1451918975 ns)
        [02]: Compressed to 644260700 B (1009911266 ns)
        [03]: Compressed to 357922616 B (753377932 ns)
        [04]: Compressed to 189488447 B (634307110 ns)
        [05]: Compressed to 97615265 B (596667790 ns)
        [06]: Compressed to 49558523 B (651154674 ns)
        [07]: Compressed to 24971354 B (583907052 ns)
        [08]: Compressed to 12534263 B (542005572 ns)
        [09]: Compressed to 6279353 B (523162944 ns)
        [10]: Compressed to 3142745 B (521748195 ns)
        [11]: Compressed to 1572143 B (514262600 ns)
        [12]: Compressed to 786269 B (598983179 ns)
        [13]: Compressed to 393188 B (520152780 ns)
        [14]: Compressed to 196610 B (517224000 ns)
        [15]: Compressed to 98312 B (517272929 ns)
```
