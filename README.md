## Simulated-Annealing Cell-Based Placement Tool ##
## Ameer M. Abdelhadi; ameer.abdelhadi@gmail.com ##
## The University of British Columbia (UBC) 2011 ##

## Problem Definition: ##

This is an implemention of a simulated-annealing standard-cell placement tool.
The tool assigns physical locations to each cell in a circuit.

#### Target floorplan: ####
The cells are to be laid out in ny rows, each with nx cell sites:

```
  <----------- nx sites ----------->
^  .----. .----. .----.       .----.
|  |Cell| |Cell| |Cell| . . . |Cell|
|  |Site| |Site| |Site|       |Site|
|  '----' '----' '----'       '----'
n           Routing Channel
y  .----. .----. .----.       .----.
|  |Cell| |Cell| |Cell| . . . |Cell|
r  |Site| |Site| |Site|       |Site|
o  '----' '----' '----'       '----'
w           Routing Channel
s                  .
|                  .
|                  .
|  .----. .----. .----.       .----.
|  |Cell| |Cell| |Cell| . . . |Cell|
|  |Site| |Site| |Site|       |Site|
v  '----' '----' '----'       '----'
```

#### Objectives: ####
The optimization goal is to minimize the half-perimeter of the smallest bounding box containing all pins for each net, summed over all nets.


#### Assumptions: ####
1. Each cell is square and is the same size.
2. No site is assigned more than one cell.
3. The routing channel is the same height as one row of cells.
4. The distance between two cells is measured from the center of one cell to the center of the other.
5. There is no extra, artificial, penalty for a net which crosses a row.
6. The product nx*ny should be at least as large as the number of cells in the circuit.


#### Input circuit format: ####
The first line contains 4 values:
1. the number of cells to be placed.
2. the number of connections between the cells.
3. the number of rows ny upon which the circuit should be placed.
4. the number of columns nx upon which the circuit should be placed.
In the following example, there are 3 cells, 3 connections between the cells, and the circuit should be placed on a chip with two rows of two cells each.

The netlist file then contains one line per net.
Each net can connect to two or more logic blocks.
For each line, the first number is the number of logic blocks to which this net connects.
The remaining numbers are the block numbers connected to this net.
Note that blocks are numbered from 0.
So, in the following example, there are three nets:
1. one that connects blocks 0, 1 and 2,
2. one that connects block 2 to block 0,
3. one that connects block 1 to block 2.

**example netlist:**
```
3 3 2 2
3 0 1 2
2 2 0
2 1 2
```

**corresponding circuit:**
```
                .---.
            .-->| 1 |--.
            |   '---'  |
    .---.   |          |
.-->| 0 |---|          |
|   '---'   |          |
|           |          '-->.---.
|           |              | 2 |--.
|           '------------->'---'  |
|                                 |
'---------------------------------'
```

**target chip:**
```
 _________________
|                 |
|  .----. .----.  |
|  |Cell| |Cell|  |
|  |Site| |Site|  |
|  '----' '----'  |
|  .----. .----.  |
|  |Cell| |Cell|  |
|  |Site| |Site|  |
|  '----' '----'  |
|_________________|
```

- - - -

## Tool Installation: ##

```
make
make clean
```

- - - -

## Tool Usage: ##

```
  annealing INFILE [OPTIONS]
```

**Options:**
```
  -help       (also -h): Print this message
  -gui        (also -g): Enable graphical user interface (GUI) 
                         Automatically enable if postscript is enabled
  -verbose    (also -v): Enable verbose logging,
                         Automatically enable if GUI is disabled
  -postscript (also -p): Generate PostScript every screen refresh
                         Gill enable GUI mode
  -refresh    (also -r): Refresh rate, followed by an integer indicating
                         the number of temperature changes loops between
                         screen refresh;  wait for 'proceed' to continue
                         Default is 10
  -inittemp   (also -i): Initial temperature, followed by a double float
                         Default is 200, multiplied by initialCost
                         (same as VPR from utoronto)
  -freezetemp (also -f): Freezing temperature, followed by a double float
                         Default is 5e-6, multiplied by initialCost/net#
                         (same as VPR from utoronto)
  -coolrate   (also -c): Cooling rate, followed by a double float
                         Default is 0.95 (same as VPR from utoronto)
  -movetemp   (also -m): Moves per temperature change, followed by an interger
                         Default is 100, multiplied by (cells#)^(4/3)
                         (same as VPR from utoronto)
```
					 
**Infile syntax:**
```
  <CELLS#> <NET#> <ROWS#> <COLUMNS#>
  <#CELLS_CONNECTED_TO_NET_1> <LIST_OF_CELLS_CONNECTED_TO_NET_1>
  <#CELLS_CONNECTED_TO_NET_2> <LIST_OF_CELLS_CONNECTED_TO_NET_2>
  ...
  <#CELLS_CONNECTED_TO_NET_n> <LIST_OF_CELLS_CONNECTED_TO_NET_n>
```

**Examples:**
```
  annealing cps.txt (using default options)
  annealing cps.txt -gui -movetemp 1 (GUI enables, refresh every temperature)
  annealing cps.txt -gui -verbose -postscript -inittemp 300        \
                    -freezetemp 0.000001 -coolrate 0.97 -movetemp 100
  annealing cps.txt -g -v -p -i 200 -f 0.000001 -c 0.97 -m 100 (same as above)
```

- - - -

## Pseudo-code for the annealing procedure: ##

```
CurrentTemperature = InitialTemperature
BestCost           = Cost(CurrentPlacement)
WHILE (CurrentTemperature > FreezingTemperature) DO
  FOR MovesPerTemperature iteraions DO
    OldCost = Cost(CurrentPlacement)
    Generate random integers Place1 and Place2 in [0,GridColumns*GridRows-1]
    Swap Place1 and Place2 places in floorplan	// Swap random places
    NewCost = Cost(CurrentPlacement)
    Gain = NewCost - OldCost
    IF (Gain < 0) THEN	// Bad move, but may be taken
      RandomFloat = Random float number in [0,1]
      IF (RandomFloat >= Exponent(Gain/CurrentTemperature) THEN
        Swap Place1 and Place2 placement in floorplan // Undo swap by reswap
      END
    ELSE	// Good move
      IF (NewCost < BestCost) THEN	// Check if the best
        BestPlacement = CurrentPlacement
        BestCost      = Cost(CurrentPlacement)
      END IF
    END IF
  END FOR
  CurrentTemperatue *= CoolingRate	// Cool temperature
END WHILE
```

- - - -

## Annealing Schedule: ##

As in VPR*, with some scaling for the required cost function, the following parameter are used:

```
InitialTemperature	= 200  * InitialCost
FreezingTemperature = 5e-6 * InitialCost/NetsNumber
CoolingRate			= 0.95
MovesPerTemperature	= 10   * CellsNumber^(4/3)
```

* Alexander Marquardt, Vaughn Betz, and Jonathan Rose. 2000. Timing-driven placement for FPGAs. In Proceedings of the 2000 ACM/SIGDA eighth international symposium on Field programmable gate arrays (FPGA '00). NY, USA.

- - - -

## Experimental Results: ##

Testcase | Cells | Nets | Size  | Cost  | Commandline Parameters | Animation GIF
-------- | ----- | ---- | ----- | ----  | ---------------------- | -------------
cm151a   | 22    | 20   | 8X8   | 45    | -m 100                 | outputs/cm151a.gif
cm138a   | 24    | 16   | 8X4   | 45    | -m 100                 | outputs/cm138a.gif
cm150a   | 36    | 35   | 8X5   | 84    | -m 100 -c 0.98         | outputs/cm150a.gif
cm162a   | 37    | 32   | 9X6   | 99    | -m 100 -c 0.98         | outputs/cm162a.gif
alu2     | 213   | 207  | 25X15 | 1138  | -m 100 -c 0.98         | outputs/alu2.gif
C880     | 260   | 234  | 20X15 | 1363  | -c 0.98 -m 20          | outputs/C880.gif
e64      | 403   | 338  | 26X16 | 2490  | -c 0.97 -m 40          | outputs/e64.gif
apex1    | 786   | 741  | 38X22 | 7951  | -f 5e-6 -c 0.97 -m 20  | outputs/apex1.gif
cps      | 882   | 773  | 40X28 | 6474  | -c 0.97 -m 30          | outputs/cps.gif
paira    | 951   | 814  | 40X30 | 5158  | -c 0.97 -m 30          | outputs/paira.gif
pairb    | 951   | 814  | 70X50 | 5331  | -c 0.98 -m 60 -f 5e-7  | outputs/pairb.gif
apex4    | 1290  | 1271 | 50X28 | 13909 | -c 0.98 -m 60          | outputs/apex4.gif
