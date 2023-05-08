# Да се преброят/изведат само песните на Beatles и Pink.

find songs | grep -E "Pink -|Beatles -" | cut -d '/' -f2 | sort | wc -l

#or

find songs -mindepth 1 -printf "%f\n" | awk -F '-' '{if ($1 == "Pink ") print $0} {if ($1 == "Beatles ") print $0}'

