#Да се направят директории с имената на уникалните групи. За улеснение, имената
#от две думи да се напишат слято:
#
#Beatles, PinkFloyd, Madness

find songs -mindepth 1 | cut -d '-' -f1 | cut -d '/' -f2 | sort | uniq | tr -d ' ' | xargs -I{} mkdir {}
