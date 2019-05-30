# EasySaper
Saper

...but easier to play.


## Progress
- [x] Basic version of saper.
- [ ] Adding easiness.
- [ ] Tests.


## The idea
The idea is to change the position of bombs to let play user as long as possible.

If user choose cell `x` to discover:
- If\* there is a bomb in `x`: the user die.
- If\* there could be a bomb in `x`, but there is a chance that there isn't: the cell is discovered and there is no bomb in `x`.
- If\* there is no bomb in `x`: the cell `x` is discovered and there is no bomb in `x`.

\*according to the information shown to user


## Examples

### Example 1:
User never die in first move.

### Example 2:
For this board:
```
   1 2 3 4 5

1  0 0 0 0 0
2  2 3 3 3 2
3  ? ? ? ? ?
4  ? ? ? ? ?
```

If there are 6 bombs, the user:
- can know that there are 5 bombs on 3rd row,
- have no idea where is the last bomb.

So if user choose cell (row: 3, column: 1) will die, becouse certainly there is a bomb.

But if user choose cell (row: 4, column: 2) will not die.


## Note
This is only an example of my code, not the biggest program I've ever written.
