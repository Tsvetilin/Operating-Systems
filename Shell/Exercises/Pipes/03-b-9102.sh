#Да се изведат само имената на песните.

find songs -mindepth 1 | cut -d '-' -f2 | cut -c 2- | cut -d '(' -f1

#or

find songs -iname '*.ogg' | sed -E 's/.*- (.*)\(.*/\1/g'

#or

find songs -mindepth 1 -printf "%f\n" | sed -E 's/.* - (.*) \(.*/\1/g'
