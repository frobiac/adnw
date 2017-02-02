rm ./passhash
# using -O0 fails, need to check
FLAGS="-g -O -W"
CC=gcc
mkdir .build 2>/dev/null
(
  cd .build
  $CC $FLAGS -c ../sha1.c      &&
  $CC $FLAGS -c ../hmac-sha1.c &&
  $CC $FLAGS -c ../passhash.c  &&
  $CC $FLAGS -c ../main.c &&

  $CC main.o passhash.o sha1.o hmac-sha1.o -o ../passhash
)

#########
if [ ! -f passhash ]; then
        echo "Binary not found"
        exit 1
fi

# test code
passed=0
run=0
test()
{
    run=$(($run +1));
    pw=$(./passhash $2 $3 $4 $5 $6)
    if [ "$1" != "$pw" ]; then
        echo ""
        echo "FAIL $1 != \"$pw\" from ./passhash $2 $3 \"$4\" \"$5\" \"$6\""
    else
        passed=$(($passed+1))
        echo -n .
        #echo "OK   $@ = $res"
    fi
}

test '3!Ke' 4 1 0 1 0
test 'O00BtcHlhd1yoX2J!sRWCXfsv' 25 1 'x-gl5' '/.-ail026' 'tag'
test 'zDcDtvo5lJv8Yi/uweHdPHJpyJ' 26 1 'secret' 'key' 'aa'

test '*Zw4' 4 1 'x-gl5' '1' '0'
test 3546   4 3 0 '1' '0'
test 'Ag8)' 4 1 'secret' 'key' 'tag'
test 'Ag8q' 4 2 'secret' 'key' 'tag'
test '8608' 4 3 'secret' 'key' 'tag'

test '8.0q7wKK8Av' 11 1 'secret' 'key' 'tag'
test '8X0q7wKK8Av' 11 2 'secret' 'key' 'tag'
test '86087555868' 11 3 'secret' 'key' 'tag'
test '8X0q7wK!8AvpiqU6P0' 18 1 'secret' 'key' 'tag'
test '8X0q7wKK8AvpiqU6P0' 18 2 'secret' 'key' 'tag'
test '860875558682535640' 18 3 'secret' 'key' 'tag'
test '8X0q7wKK8Av(iqU6P0khzRdvT' 25 1 'secret' 'key' 'tag'
test '8X0q7wKK8AvpiqU6P0khzRdvT' 25 2 'secret' 'key' 'tag'
test '8608755586825356408422084' 25 3 'secret' 'key' 'tag'
test 'Nx8,' 4 1 'secret' 'key' '2'
test 'Nx8u' 4 2 'secret' 'key' '2'
test '7781' 4 3 'secret' 'key' '2'
test 'gqJuj23zE/5' 11 1 'secret' 'key' '2'
test 'gqJuj23zEO5' 11 2 'secret' 'key' '2'
test '77810234315' 11 3 'secret' 'key' '2'
test 'gqJuj23zE/5I0rgqoj' 18 1 'secret' 'key' '2'
test 'gqJuj23zEO5I0rgqoj' 18 2 'secret' 'key' '2'
test '778102343157027750' 18 3 'secret' 'key' '2'
test 'gqJuj23zE/5I0rgqojG6k7lUf' 25 1 'secret' 'key' '2'
test 'gqJuj23zEO5I0rgqojG6k7lUf' 25 2 'secret' 'key' '2'
test '7781023431570277505687996' 25 3 'secret' 'key' '2'
test 'a9%C' 4 1 'secret' 'key' 'g?/123x-'
test 'a9FC' 4 2 'secret' 'key' 'g?/123x-'
test '4993' 4 3 'secret' 'key' 'g?/123x-'
test 'KnF3c9acc,M' 11 1 'secret' 'key' 'g?/123x-'
test 'KnF3c9accMM' 11 2 'secret' 'key' 'g?/123x-'
test '49930968866' 11 3 'secret' 'key' 'g?/123x-'
test 'KnF0c9accMMQQqm/+W' 18 1 'secret' 'key' 'g?/123x-'
test 'KnF0c9accMMQQqmDTW' 18 2 'secret' 'key' 'g?/123x-'
test '499309688660028626' 18 3 'secret' 'key' 'g?/123x-'
test 'KnF3c9accMMQQqm/+W2r+REpA' 25 1 'secret' 'key' 'g?/123x-'
test 'KnF3c9accMMQQqmDTW2rUREpA' 25 2 'secret' 'key' 'g?/123x-'
test '4993096886600286262921814' 25 3 'secret' 'key' 'g?/123x-'
test 'Xr3)' 4 1 'secret' '1' 'tag'
test 'Xr3j' 4 2 'secret' '1' 'tag'
test '5738' 4 3 'secret' '1' 'tag'
test '+sojESYsC0%' 11 1 'secret' '1' 'tag'
test 'GsojESYsC09' 11 2 'secret' '1' 'tag'
test '57381517909' 11 3 'secret' '1' 'tag'
test '+sojESYsCb97bpns9n' 18 1 'secret' '1' 'tag'
test 'GsojESYsCb97bpns9n' 18 2 'secret' '1' 'tag'
test '573815179097942799' 18 3 'secret' '1' 'tag'
test '+sojESYsCb97bpns9nMH8J05G' 25 1 'secret' '1' 'tag'
test 'GsojESYsCb97bpns9nMH8J05G' 25 2 'secret' '1' 'tag'
test '5738151790979427999488050' 25 3 'secret' '1' 'tag'
test '7)Fg' 4 1 'secret' '1' '2'
test '7gFg' 4 2 'secret' '1' '2'
test '7334' 4 3 'secret' '1' '2'
test '(g34kVbepkH' 11 1 'secret' '1' '2'
test 'Wg34kVbepkH' 11 2 'secret' '1' '2'
test '73341681272' 11 3 'secret' '1' '2'
test 'Wg34kVbepkHfdn1fu!' 18 1 'secret' '1' '2'
test 'Wg34kVbepkHfdn1fu2' 18 2 'secret' '1' '2'
test '733416812722001972' 18 3 'secret' '1' '2'
test 'Wg34kVbepkHfdn1fu2AM2*BbX' 25 1 'secret' '1' '2'
test 'Wg34kVbepkHfdn1fu2AM2hBbX' 25 2 'secret' '1' '2'
test '7334168127220019720720688' 25 3 'secret' '1' '2'
test '%Xd6' 4 1 'secret' '1' 'g?/123x-'
test 'LXd6' 4 2 'secret' '1' 'g?/123x-'
test '9355' 4 3 'secret' '1' 'g?/123x-'
test 'LFf5jJ7Q$kj' 11 1 'secret' '1' 'g?/123x-'
test 'LFf5jJ7QZkj' 11 2 'secret' '1' 'g?/123x-'
test '93556778309' 11 3 'secret' '1' 'g?/123x-'
test 'LFf5%J7QZkj0VR1Dsq' 18 1 'secret' '1' 'g?/123x-'
test 'LFf5jJ7QZkj0VR1Dsq' 18 2 'secret' '1' 'g?/123x-'
test '935567783090151286' 18 3 'secret' '1' 'g?/123x-'
test 'LFf5jJ7QZkj0VR(DsqwQ916W9' 25 1 'secret' '1' 'g?/123x-'
test 'LFf5jJ7QZkj0VR1DsqwQ916W9' 25 2 'secret' '1' 'g?/123x-'
test '9355677830901512862491609' 25 3 'secret' '1' 'g?/123x-'
test '1(Ic' 4 1 'secret' '/.-ail026' 'tag'
test '1dIc' 4 2 'secret' '/.-ail026' 'tag'
test '1226' 4 3 'secret' '/.-ail026' 'tag'
test "mdnhiih9X'C" 11 1 'secret' '/.-ail026' 'tag'
test 'mdnhiih9X6C' 11 2 'secret' '/.-ail026' 'tag'
test '12267769966' 11 3 'secret' '/.-ail026' 'tag'
test 'mdnhiih*X6Ngz0w7lP' 18 1 'secret' '/.-ail026' 'tag'
test 'mdnhiih9X6Ngz0w7lP' 18 2 'secret' '/.-ail026' 'tag'
test 'Ag8)' 4 1 'secret' 'key' 'tag'
test 'Ag8q' 4 2 'secret' 'key' 'tag'
test '8608' 4 3 'secret' 'key' 'tag'
test '8.0q7wKK8Av' 11 1 'secret' 'key' 'tag'
test '8X0q7wKK8Av' 11 2 'secret' 'key' 'tag'
test '86087555868' 11 3 'secret' 'key' 'tag'
test '8X0q7wK!8AvpiqU6P0' 18 1 'secret' 'key' 'tag'
test '8X0q7wKK8AvpiqU6P0' 18 2 'secret' 'key' 'tag'
test '860875558682535640' 18 3 'secret' 'key' 'tag'
test '8X0q7wKK8Av(iqU6P0khzRdvT' 25 1 'secret' 'key' 'tag'
test '8X0q7wKK8AvpiqU6P0khzRdvT' 25 2 'secret' 'key' 'tag'
test '8608755586825356408422084' 25 3 'secret' 'key' 'tag'
test 'Nx8,' 4 1 'secret' 'key' '2'
test 'Nx8u' 4 2 'secret' 'key' '2'
test '7781' 4 3 'secret' 'key' '2'
test 'gqJuj23zE/5' 11 1 'secret' 'key' '2'
test 'gqJuj23zEO5' 11 2 'secret' 'key' '2'
test '77810234315' 11 3 'secret' 'key' '2'
test 'gqJuj23zE/5I0rgqoj' 18 1 'secret' 'key' '2'
test 'gqJuj23zEO5I0rgqoj' 18 2 'secret' 'key' '2'
test '778102343157027750' 18 3 'secret' 'key' '2'
test 'gqJuj23zE/5I0rgqojG6k7lUf' 25 1 'secret' 'key' '2'
test 'gqJuj23zEO5I0rgqojG6k7lUf' 25 2 'secret' 'key' '2'
test '7781023431570277505687996' 25 3 'secret' 'key' '2'
test 'a9%C' 4 1 'secret' 'key' 'g?/123x-'
test 'a9FC' 4 2 'secret' 'key' 'g?/123x-'
test '4993' 4 3 'secret' 'key' 'g?/123x-'
test 'KnF3c9acc,M' 11 1 'secret' 'key' 'g?/123x-'
test 'KnF3c9accMM' 11 2 'secret' 'key' 'g?/123x-'
test '49930968866' 11 3 'secret' 'key' 'g?/123x-'
test 'KnF0c9accMMQQqm/+W' 18 1 'secret' 'key' 'g?/123x-'
test 'KnF0c9accMMQQqmDTW' 18 2 'secret' 'key' 'g?/123x-'
test '499309688660028626' 18 3 'secret' 'key' 'g?/123x-'
test 'KnF3c9accMMQQqm/+W2r+REpA' 25 1 'secret' 'key' 'g?/123x-'
test 'KnF3c9accMMQQqmDTW2rUREpA' 25 2 'secret' 'key' 'g?/123x-'
test '4993096886600286262921814' 25 3 'secret' 'key' 'g?/123x-'
test 'Xr3)' 4 1 'secret' '1' 'tag'
test 'Xr3j' 4 2 'secret' '1' 'tag'
test '5738' 4 3 'secret' '1' 'tag'
test '+sojESYsC0%' 11 1 'secret' '1' 'tag'
test 'GsojESYsC09' 11 2 'secret' '1' 'tag'
test '57381517909' 11 3 'secret' '1' 'tag'
test '+sojESYsCb97bpns9n' 18 1 'secret' '1' 'tag'
test 'GsojESYsCb97bpns9n' 18 2 'secret' '1' 'tag'
test '573815179097942799' 18 3 'secret' '1' 'tag'
test '+sojESYsCb97bpns9nMH8J05G' 25 1 'secret' '1' 'tag'
test 'GsojESYsCb97bpns9nMH8J05G' 25 2 'secret' '1' 'tag'
test '5738151790979427999488050' 25 3 'secret' '1' 'tag'
test '7)Fg' 4 1 'secret' '1' '2'
test '7gFg' 4 2 'secret' '1' '2'
test '7334' 4 3 'secret' '1' '2'
test '(g34kVbepkH' 11 1 'secret' '1' '2'
test 'Wg34kVbepkH' 11 2 'secret' '1' '2'
test '73341681272' 11 3 'secret' '1' '2'
test 'Wg34kVbepkHfdn1fu!' 18 1 'secret' '1' '2'
test 'Wg34kVbepkHfdn1fu2' 18 2 'secret' '1' '2'
test '733416812722001972' 18 3 'secret' '1' '2'
test 'Wg34kVbepkHfdn1fu2AM2*BbX' 25 1 'secret' '1' '2'
test 'Wg34kVbepkHfdn1fu2AM2hBbX' 25 2 'secret' '1' '2'
test '7334168127220019720720688' 25 3 'secret' '1' '2'
test '%Xd6' 4 1 'secret' '1' 'g?/123x-'
test 'LXd6' 4 2 'secret' '1' 'g?/123x-'
test '9355' 4 3 'secret' '1' 'g?/123x-'
test 'LFf5jJ7Q$kj' 11 1 'secret' '1' 'g?/123x-'
test 'LFf5jJ7QZkj' 11 2 'secret' '1' 'g?/123x-'
test '93556778309' 11 3 'secret' '1' 'g?/123x-'
test 'LFf5%J7QZkj0VR1Dsq' 18 1 'secret' '1' 'g?/123x-'
test 'LFf5jJ7QZkj0VR1Dsq' 18 2 'secret' '1' 'g?/123x-'
test '935567783090151286' 18 3 'secret' '1' 'g?/123x-'
test 'LFf5jJ7QZkj0VR(DsqwQ916W9' 25 1 'secret' '1' 'g?/123x-'
test 'LFf5jJ7QZkj0VR1DsqwQ916W9' 25 2 'secret' '1' 'g?/123x-'
test '9355677830901512862491609' 25 3 'secret' '1' 'g?/123x-'
test '1(Ic' 4 1 'secret' '/.-ail026' 'tag'
test '1dIc' 4 2 'secret' '/.-ail026' 'tag'
test '1226' 4 3 'secret' '/.-ail026' 'tag'
test "mdnhiih9X'C" 11 1 'secret' '/.-ail026' 'tag'
test 'mdnhiih9X6C' 11 2 'secret' '/.-ail026' 'tag'
test '12267769966' 11 3 'secret' '/.-ail026' 'tag'
test 'mdnhiih*X6Ngz0w7lP' 18 1 'secret' '/.-ail026' 'tag'
test 'mdnhiih9X6Ngz0w7lP' 18 2 'secret' '/.-ail026' 'tag'
test '122677699665400772' 18 3 'secret' '/.-ail026' 'tag'
test 'mdnhiih9X6Ngz0w7lPklaDY(v' 25 1 'secret' '/.-ail026' 'tag'
test 'mdnhiih9X6Ngz0w7lPklaDY7v' 25 2 'secret' '/.-ail026' 'tag'
test '1226776996654007729090177' 25 3 'secret' '/.-ail026' 'tag'
test '&Qg6' 4 1 'secret' '/.-ail026' '2'
test '3Qg6' 4 2 'secret' '/.-ail026' '2'
test '3018' 4 3 'secret' '/.-ail026' '2'
test '3o1/hqg2XCE' 11 1 'secret' '/.-ail026' '2'
test '3o1Jhqg2XCE' 11 2 'secret' '/.-ail026' '2'
test '30183222966' 11 3 'secret' '/.-ail026' '2'
test '3o1/hqg2XC/8nt6wY+' 18 1 'secret' '/.-ail026' '2'
test '3o1Jhqg2XCN8nt6wYU' 18 2 'secret' '/.-ail026' '2'
test '301832229668556382' 18 3 'secret' '/.-ail026' '2'
test '3o1/hqg2XC/8nt6wY+MZvl3x6' 25 1 'secret' '/.-ail026' '2'
test '3o1Jhqg2XCN8nt6wYUMZvl3x6' 25 2 'secret' '/.-ail026' '2'
test '3018322296685563826977306' 25 3 'secret' '/.-ail026' '2'
test ',Hz8' 4 1 'secret' '/.-ail026' 'g?/123x-'
test 'cHz8' 4 2 'secret' '/.-ail026' 'g?/123x-'
test '6563' 4 3 'secret' '/.-ail026' 'g?/123x-'
test 'cX63KShh&tA' 11 1 'secret' '/.-ail026' 'g?/123x-'
test 'cX63KShhNtA' 11 2 'secret' '/.-ail026' 'g?/123x-'
test '65631011532' 11 3 'secret' '/.-ail026' 'g?/123x-'
test 'cX63KShhNtAr(UfYxc' 18 1 'secret' '/.-ail026' 'g?/123x-'
test 'cX63KShhNtArAUfYxc' 18 2 'secret' '/.-ail026' 'g?/123x-'
test '656310115321229676' 18 3 'secret' '/.-ail026' 'g?/123x-'
test 'cX63KShh&tArAUfYxc9H78HCQ' 25 1 'secret' '/.-ail026' 'g?/123x-'
test 'cX63KShhNtArAUfYxc9H78HCQ' 25 2 'secret' '/.-ail026' 'g?/123x-'
test '6563101153212296769478248' 25 3 'secret' '/.-ail026' 'g?/123x-'
test 'Ib2*' 4 1 '0' 'key' 'tag'
test 'Ib2M' 4 2 '0' 'key' 'tag'
test '0329' 4 3 '0' 'key' 'tag'
test 'le2*irkqJzN' 11 1 '0' 'key' 'tag'
test 'le2MirkqJzN' 11 2 '0' 'key' 'tag'
test '03297695640' 11 3 '0' 'key' 'tag'
test 'leZMirkqJzN%5Da8vo' 18 1 '0' 'key' 'tag'
test 'leZMirkqJzNW5Da8vo' 18 2 '0' 'key' 'tag'
test '032976956409559883' 18 3 '0' 'key' 'tag'
test 'leZMirkqJzNW5Da8vobLNzH/C' 25 1 '0' 'key' 'tag'
test 'leZMirkqJzNW5Da8vobLNzHpC' 25 2 '0' 'key' 'tag'
test '0329769564095598830804449' 25 3 '0' 'key' 'tag'
test '5-Qr' 4 1 '0' 'key' '2'
test '5SQr' 4 2 '0' 'key' '2'
test '5727' 4 3 '0' 'key' '2'
test 'oSbIQUu1+OR' 11 1 '0' 'key' '2'
test 'oSbIQUu1oOR' 11 2 '0' 'key' '2'
test '57275911536' 11 3 '0' 'key' '2'
test 'oSbIQUuMoORyXGG+9O' 18 1 '0' 'key' '2'
test 'oSbIQUuMoORyXGGW9O' 18 2 '0' 'key' '2'
test '572759115365255793' 18 3 '0' 'key' '2'
test 'oSbIQUuMoORyXGG+AO3wMOas1' 25 1 '0' 'key' '2'
test 'oSbIQUuMoORyXGGWAO3wMOas1' 25 2 '0' 'key' '2'
test '5727591153652557933513191' 25 3 '0' 'key' '2'
test '3$No' 4 1 '0' 'key' 'g?/123x-'
test '3gNo' 4 2 '0' 'key' 'g?/123x-'
test '5356' 4 3 '0' 'key' 'g?/123x-'
test '5g&jiIMbdSo' 11 1 '0' 'key' 'g?/123x-'
test '5gijiIMbdSo' 11 2 '0' 'key' 'g?/123x-'
test '53565378031' 11 3 '0' 'key' 'g?/123x-'
test '5gijiIMbdSooEu1%Nz' 18 1 '0' 'key' 'g?/123x-'
test '5gijiIMbdSooEu1JNz' 18 2 '0' 'key' 'g?/123x-'
test '535653780311971982' 18 3 '0' 'key' 'g?/123x-'
test '5gijiIMbdSooEu1JNzG/1QDiU' 25 1 '0' 'key' 'g?/123x-'
test '5gijiIMbdSooEu1JNzGM1QDiU' 25 2 '0' 'key' 'g?/123x-'
test '5356537803119719821719855' 25 3 '0' 'key' 'g?/123x-'
test 'j2#A' 4 1 '0' '1' 'tag'
test 'j2iA' 4 2 '0' '1' 'tag'
test '1720' 4 3 '0' '1' 'tag'
test 'h7i+b9aMfsa' 11 1 '0' '1' 'tag'
test 'h7iJb9aMfsa' 11 2 '0' '1' 'tag'
test '17205944924' 11 3 '0' '1' 'tag'
test 'h7i+b9aMfsaORHGVgl' 18 1 '0' '1' 'tag'
test 'h7iJb9aMfsaORHGVgl' 18 2 '0' '1' 'tag'
test '172059449246998305' 18 3 '0' '1' 'tag'
test 'h7i+b9aMfsaORHGVglK2fmHSU' 25 1 '0' '1' 'tag'
test 'h7iJb9aMfsaORHGVglK2fmHSU' 25 2 '0' '1' 'tag'
test '1720594492469983052276902' 25 3 '0' '1' 'tag'
test 'e6"U' 4 1 '0' '1' '2'
test 'e6PU' 4 2 '0' '1' '2'
test '1016' 4 3 '0' '1' '2'
test '0sPAJumIy+U' 11 1 '0' '1' '2'
test '0sPAJumIyHU' 11 2 '0' '1' '2'
test '10165804246' 11 3 '0' '1' '2'
test '1sPAJumIy+U6+OEeey' 18 1 '0' '1' '2'
test '1sPAJumIyHU6ROEeey' 18 2 '0' '1' '2'
test '101658042466500222' 18 3 '0' '1' '2'
test '1sPAJumIy+U6+OEeeyqsM3EUa' 25 1 '0' '1' '2'
test '1sPAJumIyHU6ROEeeyqsM3EUa' 25 2 '0' '1' '2'
test '1016580424665002224683268' 25 3 '0' '1' '2'
test 'h2&G' 4 1 '0' '1' 'g?/123x-'
test 'h2iG' 4 2 '0' '1' 'g?/123x-'
test '7204' 4 3 '0' '1' 'g?/123x-'
test 'zCi4-E9WaAS' 11 1 '0' '1' 'g?/123x-'
test 'zCi44E9WaAS' 11 2 '0' '1' 'g?/123x-'
test '72044792208' 11 3 '0' '1' 'g?/123x-'
test 'zCiE4E9WaASA)k+cLy' 18 1 '0' '1' 'g?/123x-'
test 'zCiE4E9WaASA0kPcLy' 18 2 '0' '1' 'g?/123x-'
test '720447922080038416' 18 3 '0' '1' 'g?/123x-'
test 'zCiE4E9WaASA0k+cLyc/8P3xy' 25 1 '0' '1' 'g?/123x-'
test 'zCiE4E9WaASA0kPcLycE8P3xy' 25 2 '0' '1' 'g?/123x-'
test '7204479220800384164281366' 25 3 '0' '1' 'g?/123x-'
test 'Ps0$' 4 1 '0' '/.-ail026' 'tag'
test 'Ps0s' 4 2 '0' '/.-ail026' 'tag'
test '7603' 4 3 '0' '/.-ail026' 'tag'
test 'YvzsfpX/94m' 11 1 '0' '/.-ail026' 'tag'
test 'YvzsfpXE94m' 11 2 '0' '/.-ail026' 'tag'
test '76030065349' 11 3 '0' '/.-ail026' 'tag'
test 'YvzsfpX/34muSV8N2c' 18 1 '0' '/.-ail026' 'tag'
test 'YvzsfpXE34muSV8N2c' 18 2 '0' '/.-ail026' 'tag'
test '760300653495148428' 18 3 '0' '/.-ail026' 'tag'
test 'YvzsfpX/34muSV8N2c7Z4Fpbj' 25 1 '0' '/.-ail026' 'tag'
test 'YvzsfpXE34muSV8N2c7Z4Fpbj' 25 2 '0' '/.-ail026' 'tag'
test '7603006534951484287340064' 25 3 '0' '/.-ail026' 'tag'
test '2-Ra' 4 1 '0' '/.-ail026' '2'
test '2QRa' 4 2 '0' '/.-ail026' '2'
test '2750' 4 3 '0' '/.-ail026' '2'
test '#QEhFfl9YBB' 11 1 '0' '/.-ail026' '2'
test 'tQEhFfl9YBB' 11 2 '0' '/.-ail026' '2'
test '27506849322' 11 3 '0' '/.-ail026' '2'
test 'tQEhFfl9YBBni"LSro' 18 1 '0' '/.-ail026' '2'
test 'tQEhFfl9YBBnisLSro' 18 2 '0' '/.-ail026' '2'
test '275068493226112907' 18 3 '0' '/.-ail026' '2'
test "tQEhFfl9YBBnisLSroYXkZ'+U" 25 1 '0' '/.-ail026' '2'
test 'tQEhFfl9YBBnisLSroYXkZxAU' 25 2 '0' '/.-ail026' '2'
test '2750684932261129075436691' 25 3 '0' '/.-ail026' '2'
test '0)Zk' 4 1 '0' '/.-ail026' 'g?/123x-'
test '0ZZk' 4 2 '0' '/.-ail026' 'g?/123x-'
test '0816' 4 3 '0' '/.-ail026' 'g?/123x-'
test 'H)gXF9fXIpr' 11 1 '0' '/.-ail026' 'g?/123x-'
test 'HZgXF9fXIpr' 11 2 '0' '/.-ail026' 'g?/123x-'
test '08168956102' 11 3 '0' '/.-ail026' 'g?/123x-'
test "HZg'F9fXIprqhmh6HJ" 18 1 '0' '/.-ail026' 'g?/123x-'
test 'HZgXF9fXIprqhmh6HJ' 18 2 '0' '/.-ail026' 'g?/123x-'
test '081689561021272622' 18 3 '0' '/.-ail026' 'g?/123x-'
test 'HZgXF9fXIprqhm(6HJRQ5k8Nt' 25 1 '0' '/.-ail026' 'g?/123x-'
test 'HZgXF9fXIprqhmh6HJRQ5k8Nt' 25 2 '0' '/.-ail026' 'g?/123x-'
test '0816895610212726220951844' 25 3 '0' '/.-ail026' 'g?/123x-'
test '%Vf1' 4 1 'x-gl5' 'key' 'tag'
test 'aVf1' 4 2 'x-gl5' 'key' 'tag'
test '4611' 4 3 'x-gl5' 'key' 'tag'
test 'a6ZhZ5iVe$Y' 11 1 'x-gl5' 'key' 'tag'
test 'a6ZhZ5iVeoY' 11 2 'x-gl5' 'key' 'tag'
test '46117503886' 11 3 'x-gl5' 'key' 'tag'
test 'a6Zh-5iVeoYY9tjfeI' 18 1 'x-gl5' 'key' 'tag'
test 'a6ZhZ5iVeoYY9tjfeI' 18 2 'x-gl5' 'key' 'tag'
test '461175038866943980' 18 3 'x-gl5' 'key' 'tag'
test 'a6ZhZ5iV)oYY9tjfeIz93dV4p' 25 1 'x-gl5' 'key' 'tag'
test 'a6ZhZ5iVeoYY9tjfeIz93dV4p' 25 2 'x-gl5' 'key' 'tag'
test '4611750388669439809934349' 25 3 'x-gl5' 'key' 'tag'
test ')So3' 4 1 'x-gl5' 'key' '2'
test 'YSo3' 4 2 'x-gl5' 'key' '2'
test '8023' 4 3 'x-gl5' 'key' '2'
test 'YQgJ29lO+wa' 11 1 'x-gl5' 'key' '2'
test 'YQgJ29lOPwa' 11 2 'x-gl5' 'key' '2'
test '80232998286' 11 3 'x-gl5' 'key' '2'
test 'YQgJ29lO+waueI3M6k' 18 1 'x-gl5' 'key' '2'
test 'YQgJ29lOPwaueI3M6k' 18 2 'x-gl5' 'key' '2'
test '802329982866023063' 18 3 'x-gl5' 'key' '2'
test 'YQgJ29lO+waueI3M6k83dFdOz' 25 1 'x-gl5' 'key' '2'
test 'YQgJ29lOPwaueI3M6k83dFdOz' 25 2 'x-gl5' 'key' '2'
test '8023299828660230638359981' 25 3 'x-gl5' 'key' '2'
test 'Rl2)' 4 1 'x-gl5' 'key' 'g?/123x-'
test 'Rl2F' 4 2 'x-gl5' 'key' 'g?/123x-'
test '7128' 4 3 'x-gl5' 'key' 'g?/123x-'
test "Y'JFoV1ZlqZ" 11 1 'x-gl5' 'key' 'g?/123x-'
test 'YSJFoV1ZlqZ' 11 2 'x-gl5' 'key' 'g?/123x-'
test '71289417618' 11 3 'x-gl5' 'key' 'g?/123x-'
test 'YSJF9*1ZlqZEenaMrR' 18 1 'x-gl5' 'key' 'g?/123x-'
test 'YSJF9V1ZlqZEenaMrR' 18 2 'x-gl5' 'key' 'g?/123x-'
test '712894176187985520' 18 3 'x-gl5' 'key' 'g?/123x-'
test 'YSJFoV1ZlqZEenaMrRr/9sbmK' 25 1 'x-gl5' 'key' 'g?/123x-'
test 'YSJFoV1ZlqZEenaMrRrG9sbmK' 25 2 'x-gl5' 'key' 'g?/123x-'
test '7128941761879855202595673' 25 3 'x-gl5' 'key' 'g?/123x-'
test 'y0"N' 4 1 'x-gl5' '1' 'tag'
test 'y0sN' 4 2 'x-gl5' '1' 'tag'
test '2063' 4 3 'x-gl5' '1' 'tag'
test 'QYsz1JQp"Oa' 11 1 'x-gl5' '1' 'tag'
test 'QYsz1JQpsOa' 11 2 'x-gl5' '1' 'tag'
test '20631023608' 11 3 'x-gl5' '1' 'tag'
test 'QYs31JQpsOaOdE/OZu' 18 1 'x-gl5' '1' 'tag'
test 'QYs31JQpsOaOdEVOZu' 18 2 'x-gl5' '1' 'tag'
test '206310236080108018' 18 3 'x-gl5' '1' 'tag'
test 'QYsz1JQpsOaO"E/OZuy4kInqB' 25 1 'x-gl5' '1' 'tag'
test 'QYsz1JQpsOaOdEVOZuy4kInqB' 25 2 'x-gl5' '1' 'tag'
test '2063102360801080182434147' 25 3 'x-gl5' '1' 'tag'
test '%Hz7' 4 1 'x-gl5' '1' '2'
test 'RHz7' 4 2 'x-gl5' '1' '2'
test '9837' 4 3 'x-gl5' '1' '2'
test 'R8JF3f/DYFo' 11 1 'x-gl5' '1' '2'
test 'R8JF3fDDYFo' 11 2 'x-gl5' '1' '2'
test '98373845678' 11 3 'x-gl5' '1' '2'
test 'R8JF3f/DYForHJvsI6' 18 1 'x-gl5' '1' '2'
test 'R8JF3fDDYForHJvsI6' 18 2 'x-gl5' '1' '2'
test '983738456781915206' 18 3 'x-gl5' '1' '2'
test 'R8JF3f/DYForHJvsI6Vg6AlxI' 25 1 'x-gl5' '1' '2'
test 'R8JF3fDDYForHJvsI6Vg6AlxI' 25 2 'x-gl5' '1' '2'
test '9837384567819152061061570' 25 3 'x-gl5' '1' '2'
test 'Pt3$' 4 1 'x-gl5' '1' 'g?/123x-'
test 'Pt37' 4 2 'x-gl5' '1' 'g?/123x-'
test '7137' 4 3 'x-gl5' '1' 'g?/123x-'
test 'cgs7bENO#Yv' 11 1 'x-gl5' '1' 'g?/123x-'
test 'cgs7bENOTYv' 11 2 'x-gl5' '1' 'g?/123x-'
test '71373767276' 11 3 'x-gl5' '1' 'g?/123x-'
test 'cgs7bENOT(vAeacea4' 18 1 'x-gl5' '1' 'g?/123x-'
test 'cgs7bENOTYvAeacea4' 18 2 'x-gl5' '1' 'g?/123x-'
test '713737672763957954' 18 3 'x-gl5' '1' 'g?/123x-'
test 'cgs7bENOTYvAea#ea4dH5oBdp' 25 1 'x-gl5' '1' 'g?/123x-'
test 'cgs7bENOTYvAeacea4dH5oBdp' 25 2 'x-gl5' '1' 'g?/123x-'
test '7137376727639579540051480' 25 3 'x-gl5' '1' 'g?/123x-'
test '*Xx1' 4 1 'x-gl5' '/.-ail026' 'tag'
test 'OXx1' 4 2 'x-gl5' '/.-ail026' 'tag'
test '4003' 4 3 'x-gl5' '/.-ail026' 'tag'
test 'O00Btc#lhd1' 11 1 'x-gl5' '/.-ail026' 'tag'
test 'O00BtcHlhd1' 11 2 'x-gl5' '/.-ail026' 'tag'
test '40031473951' 11 3 'x-gl5' '/.-ail026' 'tag'
test 'O00BtcHlhd1y,X2J7s' 18 1 'x-gl5' '/.-ail026' 'tag'
test 'O00BtcHlhd1yoX2J7s' 18 2 'x-gl5' '/.-ail026' 'tag'
test '400314739514632570' 18 3 'x-gl5' '/.-ail026' 'tag'
test 'O00BtcHlhd1yoX2J!sRWCXfsv' 25 1 'x-gl5' '/.-ail026' 'tag'
test 'O00BtcHlhd1yoX2J7sRWCXfsv' 25 2 'x-gl5' '/.-ail026' 'tag'
test '4003147395146325707223703' 25 3 'x-gl5' '/.-ail026' 'tag'
test 'y6(D' 4 1 'x-gl5' '/.-ail026' '2'
test 'y6KD' 4 2 'x-gl5' '/.-ail026' '2'
test '6625' 4 3 'x-gl5' '/.-ail026' '2'
test 'mEKXem4.tfq' 11 1 'x-gl5' '/.-ail026' '2'
test 'mEKXem4Btfq' 11 2 'x-gl5' '/.-ail026' '2'
test '66258643390' 11 3 'x-gl5' '/.-ail026' '2'
test 'mEKXemMBtfq+xo1Rad' 18 1 'x-gl5' '/.-ail026' '2'
test 'mEKXemMBtfqTxo1Rad' 18 2 'x-gl5' '/.-ail026' '2'
test '662586433900781647' 18 3 'x-gl5' '/.-ail026' '2'
test 'mEKXemMBtfq+xo1RadB3ghsRw' 25 1 'x-gl5' '/.-ail026' '2'
test 'mEKXemMBtfqTxo1RadB3ghsRw' 25 2 'x-gl5' '/.-ail026' '2'
test '6625864339007816473381296' 25 3 'x-gl5' '/.-ail026' '2'
test 'Pu2#' 4 1 'x-gl5' '/.-ail026' 'g?/123x-'
test 'Pu2i' 4 2 'x-gl5' '/.-ail026' 'g?/123x-'
test '5627' 4 3 'x-gl5' '/.-ail026' 'g?/123x-'
test 'Srniq,jjsP3' 11 1 'x-gl5' '/.-ail026' 'g?/123x-'
test 'SrniqEjjsP3' 11 2 'x-gl5' '/.-ail026' 'g?/123x-'
test '56275188723' 11 3 'x-gl5' '/.-ail026' 'g?/123x-'
test 'SrniqEjj7(3jHTIanp' 18 1 'x-gl5' '/.-ail026' 'g?/123x-'
test 'SrniqEjj7P3jHTIanp' 18 2 'x-gl5' '/.-ail026' 'g?/123x-'
test '562751887233465924' 18 3 'x-gl5' '/.-ail026' 'g?/123x-'
test 'Srn#qEjjsP3jHTIanp8IzA9Ni' 25 1 'x-gl5' '/.-ail026' 'g?/123x-'
test 'SrniqEjjsP3jHTIanp8IzA9Ni' 25 2 'x-gl5' '/.-ail026' 'g?/123x-'
test '5627518872334659248847997' 25 3 'x-gl5' '/.-ail026' 'g?/123x-'

echo -e "\n\n DONE. $passed/$run tests passed."

