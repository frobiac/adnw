Several different keys
----------------------

- [n]ormal key
- [N]ormal key with implicit modifier, like '%' or '@'
- [L]ayer key
- [M]odifier key
- [D]ualRole key


Single press result
-------------------
[n] : emit KC,      repeat as long as pressed
[N] : emit KC+Mod   repeat as long as pressed
[L] : select layer  active as long as pressed
[M] : emit Mod      active as long as pressed
[D] : wait dual delay - if released within limit, emit as [n/N] keycode
                        else emit [M] or select [L]

Multiple presses
----------------




